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
#include "TripTrackForm.h"
#include "LocationManagerThread.h"
#include "AppResourceId.h"

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

TripTrackForm::TripTrackForm(void) :
		__viewType(VIEW_TYPE_NONE) {
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

	const int TEXT_SIZE = 30;

	r = LoadResources();
	if (r != E_SUCCESS) {
		AppLogException("Error loading resources: [%s]", GetErrorMessage(r));
	}
	Rectangle clientBounds = GetClientAreaBounds();

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
	AppLog("OnSceneActivatedN");
}

void TripTrackForm::OnSceneDeactivated(
		const Tizen::Ui::Scenes::SceneId& currentSceneId,
		const Tizen::Ui::Scenes::SceneId& nextSceneId) {
	AppLog("OnSceneDeactivated");
}

void TripTrackForm::OnLocationUpdated(
		const Tizen::Locations::Location& location) {
	//TODO get new data from the tracker manager which runs the thread
}

void TripTrackForm::OnLocationUpdateStatusChanged(
		LocationServiceStatus status) {
	//nothing to do
}

void TripTrackForm::OnRegionEntered(RegionId regionId) {
	//nothing to do
}

void TripTrackForm::OnRegionLeft(RegionId regionId) {
	//nothing to do
}

void TripTrackForm::OnRegionMonitoringStatusChanged(
		LocationServiceStatus status) {
	//nothing to do
}

void TripTrackForm::OnAccuracyChanged(LocationAccuracy accuracy) {
	//nothing to do
}

void TripTrackForm::SetPoiView(void) {

	if (__viewType == VIEW_TYPE_POI_VIEW) {
		return;
	}
	RemoveAllControls();
	__viewType = VIEW_TYPE_POI_VIEW;

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

	RemoveAllControls();
	__viewType = VIEW_TYPE_TRACK_VIEW;
	Rectangle sqr = GetClientAreaBounds();
	__pTrackListPanel = new TrackListPanel(sqr);
	r = __pTrackListPanel->Construct();
	if (r != E_SUCCESS) {
		AppLogException(
				"Error constructing track view panel: [%s]", GetErrorMessage(r));
		return;
	}
	AddControl(*__pTrackListPanel);

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
	editItem.Construct(ID_FOOTER_BUTTON_EDIT_TRACK);
	editItem.SetIcon(FOOTER_ITEM_STATUS_NORMAL, __pEditBitmap);
	pFooter->AddItem(editItem);
	deleteItem.Construct(ID_FOOTER_BUTTON_DELETE_TRACK);
	deleteItem.SetIcon(FOOTER_ITEM_STATUS_NORMAL, __pDeleteBitmap);
	pFooter->AddItem(deleteItem);

	pFooter->AddActionEventListener(*this);
}

void TripTrackForm::ShowPopUp(void) {
	//TODO for data edits
}

void TripTrackForm::ShowMessageBox(const String& title, const String& message) {
	//TODO for confirmations if needed
}

void TripTrackForm::OnUserEventReceivedN(RequestId requestId,
		Tizen::Base::Collection::IList* pArgs) {
	//should redraw and recieve a message from the TrackManager
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

