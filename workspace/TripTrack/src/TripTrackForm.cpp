/*
 * TripTrackForm.cpp
 *
 *  Created on: Aug 15, 2013
 *      Author: hsp
 */
#include <FGraphics.h>
#include <FLocales.h>
#include <FLocCoordinates.h>
#include <FSysSettingInfo.h>
#include <FSysSystemTime.h>
#include <FLocations.h>
#include <HMaps.h>
#include "dao/TTMedia.h"
#include "util/GraphicsUtils.h"
#include "ui/CommonComponents.h"
#include "TripTrackForm.h"
#include "LocationManagerThread.h"
#include "AppResourceId.h"
#include "geo/GeoHelper.h"

using namespace std;
using namespace Tizen::App;
using namespace Tizen::Base;
using namespace Tizen::Base::Collection;
using namespace Tizen::Base::Utility;
using namespace Tizen::Graphics;
using namespace Tizen::Locales;
using namespace Tizen::Locations;
using namespace Tizen::Ui;
using namespace Tizen::Ui::Controls;
using namespace Tizen::Ui::Scenes;
using namespace Tizen::Locations;
using namespace HMaps;

TripTrackForm::TripTrackForm(void) :
		__viewType(VIEW_TYPE_NONE), __pTrackListPanel(null), __pPoiListPanel(
				null) {
}

TripTrackForm::~TripTrackForm(void) {
}

bool TripTrackForm::Initialize(void) {
	result r = Form::Construct(
			FORM_STYLE_INDICATOR | FORM_STYLE_FOOTER | FORM_STYLE_HEADER);
	if (IsFailed(r)) {
		return false;
	}

	return true;
}

result TripTrackForm::OnDraw() {
	result r = E_SUCCESS;
	Canvas* pCanvas;

	pCanvas = this->GetCanvasN();
	pCanvas->DrawBitmap(Point(0, 0), *__pBgBitmap);

	delete pCanvas;
	return r;
}

result TripTrackForm::OnInitializing(void) {
	result r = E_SUCCESS;

	r = LoadResources();
	if (r != E_SUCCESS) {
		AppLogException("Error loading resources: [%s]", GetErrorMessage(r));
	}
	Rectangle clientBounds = GetClientAreaBounds();

	// Creates an instance of ProgressPopup
	__pProgressPopup = new (std::nothrow) ProgressPopup();
	__pProgressPopup->Construct(false, true);
	__pProgressPopup->SetText(L"Getting current location and address.");

	Header *pHeader = GetHeader();
	pHeader->SetStyle(HEADER_STYLE_TAB);

	HeaderItem poiView;
	poiView.Construct(ID_HEADER_TAB_POI_VIEW);
	poiView.SetText(I18N::GetLocalizedString(ID_STRING_TIMELINE_VIEW_TITLE));
	pHeader->AddItem(poiView);
	pHeader->AddActionEventListener(*this);

	HeaderItem trackView;
	trackView.Construct(ID_HEADER_TAB_TRACK_VIEW);
	trackView.SetText(I18N::GetLocalizedString(ID_STRING_TRACK_VIEW_TITLE));
	pHeader->AddItem(trackView);
	pHeader->AddActionEventListener(*this);

	Footer* pFooter = GetFooter();
	pFooter->SetStyle(FOOTER_STYLE_BUTTON_ICON);
	pFooter->SetBackButton();

	SetFormBackEventListener(this);
	SetPoiView();

	return r;
}

result TripTrackForm::OnTerminating(void) {

	return E_SUCCESS;
}

void TripTrackForm::OnAppControlCompleteResponseReceived(
		const Tizen::App::AppId& appId, const Tizen::Base::String& operationId,
		Tizen::App::AppCtrlResult appControlResult,
		const Tizen::Base::Collection::IMap* pExtraData) {
	if (appId.Equals(String(L"tizen.camera"))
			&& operationId.Equals(
					String(
							L"http://tizen.org/appcontrol/operation/create_content"))) {
		if (appControlResult == APP_CTRL_RESULT_SUCCEEDED) {
			AppLog("Camera capture succeeded.");
			// Use the captured image
			if (pExtraData) {
				IList* pValueList =
						const_cast<IList*>(dynamic_cast<const IList*>(pExtraData->GetValue(
								String(
										L"http://tizen.org/appcontrol/data/selected"))));
				if (pValueList) {
					String* pValue = dynamic_cast<String*>(pValueList->GetAt(0));
					AppLog("Captured image path: [%ls]", pValue->GetPointer());
					ProcessCameraResult(pValue);
					SetPoiView();
					/*result r = __pPoiListPanel->UpdatePoiCollection();
					 if (r != E_SUCCESS)
					 AppLogException(
					 "Error updating poi icon list panel: [%s]", GetErrorMessage(r));*/
				}
			}
		} else if (appControlResult == APP_CTRL_RESULT_FAILED) {
			AppLog("Camera capture failed.");
		} else if (appControlResult == APP_CTRL_RESULT_CANCELED) {
			AppLog("Camera capture was canceled.");
		} else if (appControlResult == APP_CTRL_RESULT_TERMINATED) {
			AppLog("Camera capture was terminated.");
		} else if (appControlResult == APP_CTRL_RESULT_ABORTED) {
			AppLog("Camera capture was aborted.");
		} else if (appControlResult == APP_CTRL_RESULT_FAILED) {
			AppLog("Camera capture failed.");
		}
	}

}

void TripTrackForm::OpenCamera(void) {
	String mime = L"image/jpg";
	HashMap extraData;
	extraData.Construct();
	String typeKey = L"http://tizen.org/appcontrol/data/camera/allow_switch";
	String typeVal = L"true";
	extraData.Add(&typeKey, &typeVal);

	AppControl* pAc = AppManager::FindAppControlN(L"tizen.camera",
			L"http://tizen.org/appcontrol/operation/create_content");
	if (pAc) {
		pAc->Start(null, &mime, &extraData, this);
		delete pAc;
	}
}

void TripTrackForm::ProcessCameraResult(String* imagePath) {
	result r = E_SUCCESS;

	//get location the picture was taken at
	LocationCriteria* criteria = new LocationCriteria();
	criteria->SetAccuracy(LOC_ACCURACY_TEN_METERS);
	Location location = LocationProvider::GetLocation(*criteria);

	//create a new poi
	POI* pPoi = new POI();
	String title(I18N::GetLocalizedString(ID_STRING_DEFAULT_POI_TITLE));
	String description(
			I18N::GetLocalizedString(ID_STRING_DEFAULT_POI_DESCRIPTION));

	r = pPoi->Construct(title, description, location, *imagePath);
	if (r != E_SUCCESS) {
		AppLogException(
				"Error constructing a new poi from camera capture [%ls]: [%s]", imagePath->GetPointer(), GetErrorMessage(r));
	}

	delete criteria;
}

void TripTrackForm::OnGeocodeQueryCompleted(
		const HMaps::BaseGeocodeQuery& query,
		const HMaps::GeocodeReply& reply) {
	result r = E_SUCCESS;

	String city, street, country;

	IEnumerator* pEnum = reply.GetResultListN()->GetEnumeratorN();
	AppLog("Got [%d] items from query", reply.GetResultListN()->GetCount());
	while (pEnum->MoveNext() == E_SUCCESS) {
		GeocodeResult* pResult =
				static_cast<GeocodeResult*>(pEnum->GetCurrent());
		if (pResult->GetMatchLevel() == GeocodeResult::MATCH_LEVEL_STREET) {
			city = pResult->GetLocation().GetAddress().GetCity();
			street = pResult->GetLocation().GetAddress().GetStreet();
			country = pResult->GetLocation().GetAddress().GetCountry();
			break;
		} else if (pResult->GetMatchLevel()
				== GeocodeResult::MATCH_LEVEL_DISTRICT) {
			city = pResult->GetLocation().GetAddress().GetCity();
			street = pResult->GetLocation().GetAddress().GetDistrict();
			country = pResult->GetLocation().GetAddress().GetCountry();
			break;
		}
	}

	//TODO localize this
	String desc = L"Traveling from " + street + L", " + city + L", " + country;
	String title = L"Tracking from " + city;

	//HMapsFieldProvider* fieldProvider = new HMapsFieldProvider(title, desc);
	//__pTrackListPanel->Update();

	Tracker* pTracker=new Tracker();
	pTracker->SetTitle(new String(desc));
	pTracker->SetDescription(new String(desc));

	__pProgressPopup->SetShowState(false);
	__pProgressPopup->Invalidate(true);

	ShowEditPopUp(pTracker);
}

//TODO update this code to set the box to predefine text (place holder text)
void TripTrackForm::OnQueryFailure(const BaseQuery& query, result r,
		const Tizen::Base::String& errorMsg) {
	if (r != E_SUCCESS) {
		AppLogException(
				"Error processing geocode query: [%s]", GetErrorMessage(r));
		__pProgressPopup->SetShowState(false);
		__pProgressPopup->Invalidate(true);
		return;
	}

	//TODO localize this
	String desc = String(L"Description");
	String title = String(L"Title");

	TrackerManager::getInstance()->AddTracker(title, desc);
	__pTrackListPanel->Update();

	ShowEditPopUp(TrackerManager::getInstance()->GetCurrentTracker());
}

result TripTrackForm::LoadResources(void) {
	result r = E_SUCCESS;

	AppResource* pAppRes = Application::GetInstance()->GetAppResource();

	__pCameraBitmap = pAppRes->GetBitmapN(L"camera.png");
	__pAddBitmap = pAppRes->GetBitmapN(L"add.png");
	__pDeleteBitmap = pAppRes->GetBitmapN(L"delete.png");
	__pEditBitmap = pAppRes->GetBitmapN(L"edit.png");
	__pBgBitmap = pAppRes->GetBitmapN(L"bg_160.jpg");

	return r;
}

void TripTrackForm::OnActionPerformed(const Tizen::Ui::Control& source,
		int actionId) {
	switch (actionId) {
	case ID_HEADER_TAB_POI_VIEW: {
		AppLog("Setting ID_HEADER_TAB_POI_VIEW");
		SetPoiView();
	}
		break;

	case ID_HEADER_TAB_TRACK_VIEW: {
		AppLog("Setting ID_HEADER_TAB_Track_VIEW");
		SetTrackView();
	}
		break;
	case ID_FOOTER_BUTTON_CAMERA_POI: {
		AppLog("Starting camera");
		OpenCamera();
	}
		break;
	case ID_FOOTER_BUTTON_ADD_POI: {
		AppLog("Opening create POI form popup");
	}
		break;
	case ID_FOOTER_BUTTTON_ADD_TRACK: {
		//Create a new POI with predefined or user entered fields
		int result = 0;

		Tracker* currentTracker =
				TrackerManager::getInstance()->GetCurrentTracker();

		if (!(currentTracker == 0 || currentTracker == null)) {
			MessageBox msgBox;
			String text(
					L"Track " + *(currentTracker->GetTitle())
							+ " is currently active.  Would you like to stop it now and start a new track?");

			msgBox.Construct("Warning", text, MSGBOX_STYLE_YESNO);
			msgBox.SetColor(Color(46, 151, 199));
			msgBox.SetTextColor(Color::GetColor(COLOR_ID_WHITE));
			msgBox.ShowAndWait(result);
		} else {
			__pProgressPopup->SetShowState(true);
			__pProgressPopup->Show();
			GeoHelper::GetPresentAddress(this);

			return;
		}

		if (result == MSGBOX_RESULT_YES) {
			currentTracker->SetStatus(Tracker::PAUSED);
			__pProgressPopup->SetShowState(true);
			__pProgressPopup->Show();
			GeoHelper::GetPresentAddress(this);
		}

	}
		break;
	default:
		break;
	}
}

void TripTrackForm::OnFormBackRequested(Tizen::Ui::Controls::Form& source) {
	UiApp* pApp = UiApp::GetInstance();
	AppAssert(pApp);
	pApp->Terminate();
}

void TripTrackForm::OnSceneActivatedN(
		const Tizen::Ui::Scenes::SceneId& previousSceneId,
		const Tizen::Ui::Scenes::SceneId& currentSceneId,
		Tizen::Base::Collection::IList* pArgs) {
	AppLog("Activated MAIN_FORM scene");
}

void TripTrackForm::OnSceneDeactivated(
		const Tizen::Ui::Scenes::SceneId& currentSceneId,
		const Tizen::Ui::Scenes::SceneId& nextSceneId) {
	AppLog("Deactivated MAIN_FORM scene");
}

void TripTrackForm::SetPoiView(void) {
	result r = E_SUCCESS;

	if (__viewType == VIEW_TYPE_POI_VIEW) {
		return;
	}

	if (__viewType == VIEW_TYPE_TRACK_VIEW) {
		__pTrackListPanel->SetShowState(false);
	}

	__viewType = VIEW_TYPE_POI_VIEW;
	if (__pPoiListPanel == null) {
		Rectangle sqr = GetClientAreaBounds();
		__pPoiListPanel = new PoiIconListPanel(sqr);
		r = __pPoiListPanel->Construct();
		if (r != E_SUCCESS) {
			AppLogException(
					"Error constructing poi scroll panel: [%s]", GetErrorMessage(r));
			return;
		}
		r = AddControl(*__pPoiListPanel);
	} else
		__pPoiListPanel->SetShowState(true);

	Footer* pFooter = GetFooter();

	pFooter->RemoveAllItems();
	pFooter->SetStyle(FOOTER_STYLE_BUTTON_ICON);

	Color* itemColor = new Color(46, 151, 199);
	pFooter->SetItemColor(FOOTER_ITEM_STATUS_NORMAL, *itemColor);
	Color* footerColor = new Color(70, 70, 70);
	pFooter->SetColor(*footerColor);

	FooterItem addItem, cameraItem;
	addItem.Construct(ID_FOOTER_BUTTON_ADD_POI);
	addItem.SetIcon(FOOTER_ITEM_STATUS_NORMAL, __pAddBitmap);
	pFooter->AddItem(addItem);
	cameraItem.Construct(ID_FOOTER_BUTTON_CAMERA_POI);
	cameraItem.SetIcon(FOOTER_ITEM_STATUS_NORMAL, __pCameraBitmap);
	pFooter->AddItem(cameraItem);

	pFooter->AddActionEventListener(*this);
}

void TripTrackForm::SetTrackView(void) {
	result r = E_SUCCESS;

	if (__viewType == VIEW_TYPE_TRACK_VIEW) {
		return;
	}
	if (__viewType == VIEW_TYPE_POI_VIEW) {
		__pPoiListPanel->SetShowState(false);
	}

	__viewType = VIEW_TYPE_TRACK_VIEW;
	if (__pTrackListPanel == null) {
		Rectangle sqr = GetClientAreaBounds();
		__pTrackListPanel = new TrackListPanel(sqr);
		r = __pTrackListPanel->Construct();
		if (r != E_SUCCESS) {
			AppLogException(
					"Error constructing track view panel: [%s]", GetErrorMessage(r));
			return;
		}
		AddControl(*__pTrackListPanel);
	} else
		__pTrackListPanel->SetShowState(true);

	Footer* pFooter = GetFooter();

	pFooter->RemoveAllItems();
	pFooter->SetStyle(FOOTER_STYLE_BUTTON_ICON);

	Color* itemColor = new Color(46, 151, 199);
	pFooter->SetItemColor(FOOTER_ITEM_STATUS_NORMAL, *itemColor);
	Color* buttonColor = new Color(*itemColor);
	buttonColor->SetGreen(itemColor->GetGreen() - 20);
	pFooter->SetButtonColor(BUTTON_ITEM_STATUS_NORMAL, *buttonColor);

	FooterItem addItem, editItem, deleteItem;
	addItem.Construct(ID_FOOTER_BUTTTON_ADD_TRACK);
	addItem.SetIcon(FOOTER_ITEM_STATUS_NORMAL, __pAddBitmap);
	pFooter->AddItem(addItem);
	/*editItem.Construct(ID_FOOTER_BUTTON_EDIT_TRACK);
	 editItem.SetIcon(FOOTER_ITEM_STATUS_NORMAL, __pEditBitmap);
	 pFooter->AddItem(editItem);
	 deleteItem.Construct(ID_FOOTER_BUTTON_DELETE_TRACK);
	 deleteItem.SetIcon(FOOTER_ITEM_STATUS_NORMAL, __pDeleteBitmap);
	 pFooter->AddItem(deleteItem);*/

	pFooter->AddActionEventListener(*this);
}

void TripTrackForm::ShowEditPopUp(IFormFieldProvider* pProvider) {
	result r = E_SUCCESS;

	EditFormPopup* pEditPopup = new EditFormPopup();

	Rectangle bounds = GetClientAreaBounds();
	r = pEditPopup->Construct(pProvider, __pTrackListPanel,
			Dimension((int) bounds.width * 0.90, (int) bounds.height * 0.90),
			I18N::GetLocalizedString(ID_STRING_CREATE_TRACK_POPUP_TITLE));
	if (r != E_SUCCESS)
		AppLogException(
				"Error constructing edit form popup: [%s]", GetErrorMessage(r));

	pEditPopup->Show();
}

void TripTrackForm::ShowMessageBox(const String& title, const String& message) {
	//TODO for confirmations if needed
}

void TripTrackForm::LaunchLocationSettings(void) {
	int res;

	MessageBox messageBox;
	messageBox.Construct(L"Information",
			L"Location services are disabled. Enable them in location settings?",
			MSGBOX_STYLE_YESNO);
	messageBox.ShowAndWait(res);

	if (res == MSGBOX_RESULT_YES) {
		HashMap extraData;
		extraData.Construct();
		String categoryKey = L"category";
		String categoryVal = L"Location";
		extraData.Add(&categoryKey, &categoryVal);

		AppControl* pAc = AppManager::FindAppControlN(L"tizen.settings",
				L"http://tizen.org/appcontrol/operation/configure");

		if (pAc) {
			pAc->Start(null, null, &extraData, this);
			delete pAc;
		}
	}
	return;
}

bool TripTrackForm::CheckLocationSetting(void) {
	bool hasPrivilege = false;
	bool gpsEnabled = true;
	bool wpsEnabled = true;

	result gps = Tizen::System::SettingInfo::GetValue(
			L"http://tizen.org/setting/location.gps", gpsEnabled);
	result wps = Tizen::System::SettingInfo::GetValue(
			L"http://tizen.org/setting/location.wps", wpsEnabled);

	hasPrivilege = gpsEnabled | wpsEnabled;
	if (gps != E_SUCCESS || wps != E_SUCCESS || hasPrivilege == false) {
		return false;
	}

	return true;
}

