/*
 * PoiForm.cpp
 *
 *  Created on: Sep 2, 2013
 *      Author: bohemius
 */

#include <FApp.h>
#include <FGraphics.h>
#include <FLocations.h>
#include <FMedia.h>
#include "ui/PoiForm.h"
#include "dao/StorageManager.h"
#include "AppResourceId.h"
#include "SceneRegister.h"
#include "util/GraphicsUtils.h"
#include "geo/StaticMap.h"

using namespace Tizen::Base;
using namespace Tizen::Base::Collection;
using namespace Tizen::Base::Utility;
using namespace Tizen::Media;
using namespace Tizen::Ui::Controls;
using namespace Tizen::Graphics;
using namespace Tizen::App;
using namespace Tizen::Ui::Scenes;
using namespace Tizen::Locations;
using namespace Tizen::Ui;
using namespace Tizen::Net::Http;
using namespace Tizen::Web::Json;

PoiForm::PoiForm() :
		__pTitleLabel(null), __pDescriptionLabel(null), __pPostProgressLabel(
				null), __pPoiPanel(null), __pMediaIconListView(null), __pPoi(
				null), __pLongPressDetector(null), __pFbEnum(null), __fbPostCounter(-1) {
}

bool PoiForm::Initialize(void) {
	result r = Form::Construct(FORM_STYLE_INDICATOR | FORM_STYLE_FOOTER);
	if (IsFailed(r)) {
		return false;
	}
	return true;
}

PoiForm::~PoiForm() {
	// TODO Auto-generated destructor stub
}

result PoiForm::OnInitializing(void) {
	result r = E_SUCCESS;

	//load bitmaps and what not
	r = LoadResources();
	if (r != E_SUCCESS) {
		AppLogException("Error loading resources: [%s]", GetErrorMessage(r));
	}

	//define spacing
	__pClientBounds = new Rectangle(GetBounds());
	__pTitleRect = new Rectangle(0, 0, __pClientBounds->width,
			__pClientBounds->height / 6);
	__pDescRect = new Rectangle(0, __pClientBounds->height / 6 + 1,
			__pClientBounds->width, __pClientBounds->height / 6);
	__pListRect = new FloatRectangle(0,
			(double) (__pDescRect->y + __pDescRect->height + 1),
			(double) __pClientBounds->width,
			(double) (__pClientBounds->height - __pDescRect->height
					- __pTitleRect->height - 3));
	tile_width = (__pListRect->width - TILES_PER_ROW * TILES_SPACING_X
			- TILES_SPACING_X) / TILES_PER_ROW;
	tile_height = tile_width * 1.3f;
	Color* itemColor = new Color(46, 151, 199);
	Color* footerColor = new Color(70, 70, 70);

	Footer* pFooter = GetFooter();

	pFooter->SetStyle(FOOTER_STYLE_BUTTON_ICON);
	pFooter->SetBackButton();
	pFooter->SetItemColor(FOOTER_ITEM_STATUS_NORMAL, *itemColor);
	pFooter->SetColor(*footerColor);

	FooterItem editItem, cameraItem, locationItem, fbItem;

	editItem.Construct(ID_FOOTER_BUTTTON_EDIT);
	editItem.SetIcon(FOOTER_ITEM_STATUS_NORMAL, __pEditBitmap);
	pFooter->AddItem(editItem);
	cameraItem.Construct(ID_FOOTER_BUTTON_CAMERA);
	cameraItem.SetIcon(FOOTER_ITEM_STATUS_NORMAL, __pCameraBitmap);
	pFooter->AddItem(cameraItem);
	locationItem.Construct(ID_FOOTER_BUTTON_MAP);
	locationItem.SetIcon(FOOTER_ITEM_STATUS_NORMAL, __pLocationBitmap);
	pFooter->AddItem(locationItem);
	fbItem.Construct(ID_FOOTER_BUTTON_FB);
	fbItem.SetIcon(FOOTER_ITEM_STATUS_NORMAL, __pFbBitmap);
	pFooter->AddItem(fbItem);

	pFooter->AddActionEventListener(*this);

	SetFormBackEventListener(this);
	__pLongPressDetector = new TouchLongPressGestureDetector();
	r = __pLongPressDetector->Construct();
	__pLongPressDetector->AddLongPressGestureEventListener(*this);

	return r;

}

result PoiForm::OnTerminating(void) {
	result r = E_SUCCESS;

	return r;
}

void PoiForm::OnActionPerformed(const Tizen::Ui::Control& source,
		int actionId) {
	switch (actionId) {
	case ID_FOOTER_BUTTTON_EDIT: {
		ShowEditPopup();
	}
		break;
	case ID_FOOTER_BUTTON_MAP: {
		//TODO milestone 2
	}
		break;
	case ID_FOOTER_BUTTON_FB: {
		FacebookAccessToken creds =
				StorageManager::getInstance()->GetFacebookCredentials();

		if (creds.AccessToken == L"" || creds.ExpiryTime < 0) {
			SceneManager* pSceneMngr = SceneManager::GetInstance();
			pSceneMngr->GoForward(ForwardSceneTransition(SCENE_FACEBOOK_FORM),
					null);
		} else {
			AppLog("Already got valid token");
			CreateFacebookAlbum();
		}
		__fbPostCounter = 0;
		__pPostProgressLabel = new Label();
		//TODO localize this
		__pPostProgressLabel->Construct(Rectangle(0, 0, 550, 50),
				L"Posting to Facebook");
		__pPostProgressLabel->SetTextConfig(DESCRIPTION_TEXT_SIZE,
							LABEL_TEXT_STYLE_NORMAL);
		__pPostProgressLabel->SetMargin(5,5);
		__pPostProgressLabel->SetTextVerticalAlignment(ALIGNMENT_TOP);
		__pPostProgressLabel->SetTextHorizontalAlignment(ALIGNMENT_LEFT);
		__pPostProgressLabel->SetBackgroundColor(Color(46, 151, 199));
		__pPostProgressLabel->SetTextColor(Color::GetColor(COLOR_ID_WHITE));
		this->AddControl(__pPostProgressLabel);
		__pPostProgressLabel->SetShowState(true);
	}
		break;
	case ID_FOOTER_BUTTON_CAMERA: {
		AppLog("Starting camera");
		OpenCamera();
	}
		break;
	default:
		break;
	}
}

void PoiForm::OnFormBackRequested(Tizen::Ui::Controls::Form& source) {
	SceneManager* pSceneMngr = SceneManager::GetInstance();
	LinkedList* paramList = new LinkedList();

	if (__previousScene == SCENE_MAP_FORM) {
		paramList->Add(__pPoi);
		pSceneMngr->GoBackward(BackwardSceneTransition(SCENE_MAP_FORM),
				paramList);
	} else
		pSceneMngr->GoBackward(BackwardSceneTransition(SCENE_MAIN_FORM),
				paramList);

}

result PoiForm::LoadResources(void) {
	result r = E_SUCCESS;

	AppResource* pAppRes = Application::GetInstance()->GetAppResource();

	__pCameraBitmap = pAppRes->GetBitmapN(L"camera.png");
	__pDeleteBitmap = pAppRes->GetBitmapN(L"delete.png");
	__pEditBitmap = pAppRes->GetBitmapN(L"edit.png");
	__pFbBitmap = pAppRes->GetBitmapN(L"facebook.png");
	__pLocationBitmap = pAppRes->GetBitmapN(L"location.png");
	__pCheckBitmap = pAppRes->GetBitmapN(L"check.png");

	__pBgBitmap = pAppRes->GetBitmapN(L"bg_160.jpg");

	return r;
}

result PoiForm::OnDraw() {
	result r = E_SUCCESS;
	Canvas* pCanvas;

	pCanvas = this->GetCanvasN();
	pCanvas->DrawBitmap(Point(0, 0), *__pBgBitmap);

	delete pCanvas;
	return r;
}

void PoiForm::OnSceneActivatedN(
		const Tizen::Ui::Scenes::SceneId& previousSceneId,
		const Tizen::Ui::Scenes::SceneId& currentSceneId,
		Tizen::Base::Collection::IList* pArgs) {

	result r = E_SUCCESS;
	__previousScene = previousSceneId;

	if (previousSceneId == SCENE_MAIN_FORM || previousSceneId == SCENE_MAP_FORM) {
		/*Get the poi from passed arguments*/
		Object* param = pArgs->GetAt(0);
		__pPoi = static_cast<POI*>(param);
		AppLog("PoiForm got POI [%ls]", __pPoi->GetTitle()->GetPointer());

		//load the icon list
		r = LoadImageList();
		if (r != E_SUCCESS) {
			AppLogException(
					"Error loading media for poi with id [%ld] from database: [%s]", __pPoi->GetId(), GetErrorMessage(r));
		}

		/*Create the panel*/
		if (__pPoiPanel == null) {
			__pPoiPanel = new Panel();
			r = __pPoiPanel->Construct(*__pClientBounds);
		} else {
			__pPoiPanel->RemoveAllControls();
		}

		/*Create the media for title background*/
		Bitmap* pTitleBgBitmap;

		if (__pPoi->GetDefImageId() > 0) {
			pTitleBgBitmap = CreateTitleBitmap();
		} else {
			AppResource* pAppRes = Application::GetInstance()->GetAppResource();
			pTitleBgBitmap = pAppRes->GetBitmapN(L"BlankPoi.png");
			pTitleBgBitmap->SetScalingQuality(BITMAP_SCALING_QUALITY_HIGH);
			pTitleBgBitmap->Scale(
					Dimension(__pTitleRect->width, __pTitleRect->height));
		}

		/*set the title label*/
		if (__pTitleLabel == null) {
			__pTitleLabel = new Label();
			r = __pTitleLabel->Construct(*__pTitleRect, *(__pPoi->GetTitle()));
			if (r != E_SUCCESS)
				AppLogException(
						"Error constructing title label for poi form: [%s]", GetErrorMessage(r));

			__pTitleLabel->SetBackgroundBitmap(*pTitleBgBitmap);
			__pTitleLabel->SetMargin(TITLE_PADDING_Y, TITLE_PADDING_X);
			__pTitleLabel->SetTextHorizontalAlignment(ALIGNMENT_LEFT);
			__pTitleLabel->SetTextVerticalAlignment(ALIGNMENT_BOTTOM);
			__pTitleLabel->SetTextConfig(TITLE_TEXT_SIZE,
					LABEL_TEXT_STYLE_BOLD);
			__pTitleLabel->SetTextColor(Color::GetColor(COLOR_ID_WHITE));
		} else
			__pTitleLabel->SetText(*(__pPoi->GetTitle()));
		__pPoiPanel->AddControl(__pTitleLabel);

		/*set the description label*/
		if (__pDescriptionLabel == null) {
			__pDescriptionLabel = new Label();
			r = __pDescriptionLabel->Construct(*__pDescRect,
					*(__pPoi->GetDescription()));
			if (r != E_SUCCESS)
				AppLogException(
						"Error constructing description label for poi form: [%s]", GetErrorMessage(r));

			__pDescriptionLabel->SetTextConfig(DESCRIPTION_TEXT_SIZE,
					LABEL_TEXT_STYLE_NORMAL);
			__pDescriptionLabel->SetMargin(DESCRIPTION_PADDING_Y,
					DESCRIPTION_PADDING_X);
			__pDescriptionLabel->SetTextVerticalAlignment(ALIGNMENT_TOP);
			__pDescriptionLabel->SetTextHorizontalAlignment(ALIGNMENT_LEFT);
			__pDescriptionLabel->SetTextColor(Color::GetColor(COLOR_ID_WHITE));
		} else
			__pDescriptionLabel->SetText(*(__pPoi->GetDescription()));
		__pPoiPanel->AddControl(__pDescriptionLabel);

		/*set the tile list for image browsing*/
		if (__pMediaIconListView == null) {
			__pMediaIconListView = new IconListView();
			r = __pMediaIconListView->Construct(*__pListRect,
					FloatDimension(tile_width, tile_height),
					ICON_LIST_VIEW_STYLE_NORMAL,
					ICON_LIST_VIEW_SCROLL_DIRECTION_VERTICAL);
			if (r != E_SUCCESS)
				AppLogException(
						"Error constructing icon list view from media collection: [%s]", GetErrorMessage(r));
		}
		__pMediaIconListView->SetItemProvider(*this);
		__pMediaIconListView->AddIconListViewItemEventListener(*this);
		__pMediaIconListView->AddTouchEventListener(*this);
		__pMediaIconListView->AddGestureDetector(__pLongPressDetector);
		__pMediaIconListView->SetItemBorderStyle(
				ICON_LIST_VIEW_ITEM_BORDER_STYLE_OUTLINE);
		__pMediaIconListView->SetItemSpacing(TILES_SPACING_X, TILES_SPACING_Y);
		__pPoiPanel->AddControl(__pMediaIconListView);

		AddControl(__pPoiPanel);
	} else if (previousSceneId == SCENE_FACEBOOK_FORM)
		CreateFacebookAlbum();
}

void PoiForm::OnSceneDeactivated(
		const Tizen::Ui::Scenes::SceneId& currentSceneId,
		const Tizen::Ui::Scenes::SceneId& nextSceneId) {
}

void PoiForm::OnAppControlCompleteResponseReceived(
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
					__pMediaIconListView->UpdateList();
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

void PoiForm::OnIconListViewItemStateChanged(
		Tizen::Ui::Controls::IconListView& view, int index,
		Tizen::Ui::Controls::IconListViewItemStatus status) {
}

Tizen::Ui::Controls::IconListViewItem* PoiForm::CreateItem(int index) {
	result r = E_SUCCESS;

	TTMedia * pMedia = null;

	r = __pPoi->GetAssociatedMedia()->GetAt(index, pMedia);
	if (r != E_SUCCESS || pMedia == null) {
		AppLogException(
				"Error getting media with index [%d] from media collection: [%s]", index, GetErrorMessage(r));
		return null;
	}

	ImageBuffer imgBuf;
	r = imgBuf.Construct(*(pMedia->GetContent()), (int) tile_width,
			(int) tile_height, IMAGE_SCALING_METHOD_BICUBIC);
	if (r != E_SUCCESS)
		AppLogException(
				"Error constructing image buffer: [%s]", GetErrorMessage(r));
	Bitmap* pMediaTile;
	pMediaTile = imgBuf.GetBitmapN(BITMAP_PIXEL_FORMAT_RGB565,
			BUFFER_SCALING_AUTO);
	if (r != E_SUCCESS) {
		AppLogException(
				"Error construction bitmap image from media with id [%d]: [%s]", pMedia->GetId(), GetErrorMessage(r));
	}
	IconListViewItem* pPoiItem = new (std::nothrow) IconListViewItem();
	r = pPoiItem->Construct(*pMediaTile);
	if (r != E_SUCCESS) {
		AppLogException(
				"Error constructing icon list item for media [%ls]: [%s]", pMedia->GetSourceUri()->GetPointer(), GetErrorMessage(r));
	}
	if (__pPoi->GetDefImageId() != pMedia->GetId())
		r = pPoiItem->SetOverlayBitmap(ID_OVERLAY_BITMAP_DELETE,
				__pDeleteBitmap, ALIGNMENT_RIGHT, ALIGNMENT_TOP);
	else
		r = pPoiItem->SetOverlayBitmap(ID_OVERLAY_BITMAP_DELETE, __pCheckBitmap,
				ALIGNMENT_RIGHT, ALIGNMENT_TOP);
	delete pMediaTile;

	return pPoiItem;
}

bool PoiForm::DeleteItem(int index,
		Tizen::Ui::Controls::IconListViewItem* pItem) {
	delete pItem;
	pItem = null;
	return true;
}

int PoiForm::GetItemCount(void) {
	if (__pPoi != null)
		return __pPoi->GetAssociatedMedia()->GetCount();
	else
		return 0;
}

result PoiForm::LoadImageList(void) {
	result r = E_SUCCESS;

	StorageManager* store = StorageManager::getInstance();
	LinkedListT<TTMedia*>* pMediaList = store->GetMedia(__pPoi->GetId());
	__pPoi->GetAssociatedMedia()->RemoveAll();
	__pPoi->GetAssociatedMedia()->AddItems(*pMediaList);

	return r;
}

void PoiForm::ShowEditPopup(void) {
	result r = E_SUCCESS;

	EditFormPopup* pEditPopup = new EditFormPopup();

	Rectangle bounds = GetClientAreaBounds();
	r = pEditPopup->Construct(__pPoi, this,
			Dimension((int) bounds.width * 0.90, (int) bounds.height * 0.90),
			I18N::GetLocalizedString(ID_STRING_EDIT_POI_POPUP_TITLE));

	if (r != E_SUCCESS) {
		AppLogException(
				"Error constructing edit form popup: [%s]", GetErrorMessage(r));
		return;
	}

	pEditPopup->Show();
}

void PoiForm::OpenCamera(void) {
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

result PoiForm::Update(void) {
	__pTitleLabel->SetText(*(__pPoi->GetTitle()));
	__pTitleLabel->Draw();
	__pDescriptionLabel->SetText(*(__pPoi->GetDescription()));
	__pDescriptionLabel->Draw();
}

void PoiForm::OnIconListViewOverlayBitmapSelected(
		Tizen::Ui::Controls::IconListView& iconListView, int index,
		int overlayBitmapId) {
	result r = E_SUCCESS;
	TTMedia * pMedia = null;

	r = __pPoi->GetAssociatedMedia()->GetAt(index, pMedia);
	if (r != E_SUCCESS || pMedia == null) {
		AppLogException(
				"Error getting media with index [%d] from collection from POI with id [%lld]: ", index, __pPoi->GetId(), GetErrorMessage(r));
		return;
	}

	if (pMedia->GetId() != __pPoi->GetDefImageId()) {
		__pPoi->DeleteMedia(pMedia);
		__pMediaIconListView->RefreshList(index, LIST_REFRESH_TYPE_ITEM_REMOVE);
	}
}

void PoiForm::OnTouchPressed(const Tizen::Ui::Control& source,
		const Tizen::Graphics::Point& currentPosition,
		const Tizen::Ui::TouchEventInfo& touchInfo) {
	__lastPosition = currentPosition;
}

void PoiForm::ProcessCameraResult(Tizen::Base::String* imagePath) {
	result r = E_SUCCESS;

	Bitmap* pCapturedBitmap = GraphicsUtils::CreateBitmap(*imagePath);
	r = pCapturedBitmap->Scale(
			FloatDimension(POI::TILE_IMAGE_WIDTH, POI::TILE_IMAGE_HEIGHT));
	if (r != E_SUCCESS)
		AppLogException(
				"Bitmap [%ls] scaling failed: [%s]", imagePath->GetPointer(), GetErrorMessage(r));
	ByteBuffer* pImgBuffer = GraphicsUtils::CreateImageBuffer(pCapturedBitmap);

	//create a new media
	TTMedia* pMedia = new TTMedia();
	String title(I18N::GetLocalizedString(ID_STRING_DEFAULT_POI_TITLE));
	String description(
			I18N::GetLocalizedString(ID_STRING_DEFAULT_POI_DESCRIPTION));

	r = pMedia->Construct(*imagePath, __pPoi->GetId(), pImgBuffer);
	if (r != E_SUCCESS) {
		AppLogException(
				"Error constructing a new media from camera capture [%ls]: [%s]", imagePath->GetPointer(), GetErrorMessage(r));
	}
	r = LoadImageList();
	if (r != E_SUCCESS)
		AppLogException(
				"Error refreshing image list: [%s]", GetErrorMessage(r));
}

void PoiForm::OnTouchDoublePressed(const Tizen::Ui::Control& source,
		const Tizen::Graphics::Point& currentPosition,
		const Tizen::Ui::TouchEventInfo& touchInfo) {

	LinkedList* parameterList = new LinkedList();
	if (&source == __pMediaIconListView) {
		int index = __pMediaIconListView->GetItemIndexFromPosition(
				currentPosition);
		AppLog("Double clicked on image on poi form on index [%d]", index);

		parameterList->Add(__pPoi);
		parameterList->Add(Integer(index));

		SceneManager* pSceneMngr = SceneManager::GetInstance();
		pSceneMngr->GoForward(ForwardSceneTransition(SCENE_GALLERY_FORM),
				parameterList);
	}
}

void PoiForm::OnLongPressGestureDetected(
		Tizen::Ui::TouchLongPressGestureDetector& gestureDetector) {
	AppLog("Got a long press on poi form");
	TTMedia* pMedia = GetMediaFromClick(__lastPosition);

	__pPoi->SetDefImageId(pMedia->GetId());
	StorageManager::getInstance()->CRUDoperation(__pPoi, I_CRUDable::UPDATE);
	__pTitleLabel->SetBackgroundBitmap(*(CreateTitleBitmap()));
	__pTitleLabel->Draw();
	__pMediaIconListView->UpdateList();
}

TTMedia* PoiForm::GetMediaFromClick(Point point) {
	TTMedia* retVal = null;

	result r = E_SUCCESS;

	int index = __pMediaIconListView->GetItemIndexFromPosition(point);
	r = __pPoi->GetAssociatedMedia()->GetAt(index, retVal);
	if (r != E_SUCCESS || retVal == null) {
		AppLogException(
				"Error getting Media for list item with index [%d]: ", index, GetErrorMessage(r));
	}
	return retVal;
}

void PoiForm::OnTransactionReadyToRead(
		Tizen::Net::Http::HttpSession& httpSession,
		Tizen::Net::Http::HttpTransaction& httpTransaction,
		int availableBodyLen) {
}

void PoiForm::OnTransactionAborted(Tizen::Net::Http::HttpSession& httpSession,
		Tizen::Net::Http::HttpTransaction& httpTransaction, result r) {
}

void PoiForm::OnTransactionReadyToWrite(
		Tizen::Net::Http::HttpSession& httpSession,
		Tizen::Net::Http::HttpTransaction& httpTransaction,
		int recommendedChunkSize) {
}

void PoiForm::OnTransactionHeaderCompleted(
		Tizen::Net::Http::HttpSession& httpSession,
		Tizen::Net::Http::HttpTransaction& httpTransaction, int headerLen,
		bool authRequired) {
}

void PoiForm::OnTransactionCompleted(Tizen::Net::Http::HttpSession& httpSession,
		Tizen::Net::Http::HttpTransaction& httpTransaction) {
	result r = E_SUCCESS;

	if (httpTransaction.GetResponse()->GetHttpStatusCode() == HTTP_STATUS_OK) {
		if (__fbAlbumId < 0) {
			ParseAlbumResponse(httpTransaction.GetResponse()->ReadBodyN());

			StaticMap* pStaticMap = new StaticMap();
			r = pStaticMap->Construct(__pPoi, __fbAlbumId);
			if (r != E_SUCCESS)
				AppLogException(
						"Error creating static map from poi", GetErrorMessage(r));
			else
				AppLog("Static map constructed");
		}

		if (__pFbEnum->MoveNext() == E_SUCCESS) {
			r = CreateFacebookPhoto();
			if (r != E_SUCCESS)
				AppLogException("Error posting photo to face book");
			else {
				TTMedia* pMedia;
				__pFbEnum->GetCurrent(pMedia);
				AppLog(
						"Posted to %ls facebook album with id %lld", pMedia->GetSourceUri()->GetPointer(), __fbAlbumId);
			}
		} else {
			RemoveControl(__pPostProgressLabel);
			__pPostProgressLabel = null;
			__fbAlbumId = -1;
			__fbPostCounter = -1;
			delete __pFbEnum;
			__pFbEnum = null;
		}
	} else {
		AppLogException(
				"Error during transaction: %d", httpTransaction.GetResponse()->GetHttpStatusCode());
		__fbAlbumId = -1;
		delete __pFbEnum;
		__pFbEnum = null;
	}

	delete &httpTransaction;
	delete &httpSession;
}

void PoiForm::OnTransactionCertVerificationRequiredN(
		Tizen::Net::Http::HttpSession& httpSession,
		Tizen::Net::Http::HttpTransaction& httpTransaction,
		Tizen::Base::String* pCert) {
}

void PoiForm::OnHttpUploadInProgress(Tizen::Net::Http::HttpSession& httpSession,
		Tizen::Net::Http::HttpTransaction& httpTransaction,
		long long currentLength, long long totalLength) {
	double perc = ((double) currentLength) / ((double) totalLength) * 100.0;
	AppLog("Percent complete %.2f", perc);
	//TODO localize this
	String progressText=L"Posting to Facebook ";
	progressText.Append(__fbPostCounter);
	progressText.Append(L" of " );
	progressText.Append(__pPoi->GetAssociatedMedia()->GetCount());
	progressText.Append(L" (" );
	progressText.Append(perc);
	progressText.Append(L"%)" );
	__pPostProgressLabel->SetText(progressText);
	__pPostProgressLabel->Draw();
}

void PoiForm::OnHttpDownloadInProgress(
		Tizen::Net::Http::HttpSession& httpSession,
		Tizen::Net::Http::HttpTransaction& httpTransaction,
		long long currentLength, long long totalLength) {
}

Tizen::Graphics::Bitmap* PoiForm::CreateTitleBitmap(void) {
	result r = E_SUCCESS;

	TTMedia* pMedia = new TTMedia();
	r = pMedia->Construct(__pPoi->GetDefImageId());
	if (r != E_SUCCESS)
		AppLogException(
				"Error constructing media with ID [%ld]: [%s]", __pPoi->GetDefImageId(), GetErrorMessage(r));

	ImageBuffer imgBuf;

	r = imgBuf.Construct(*(pMedia->GetSourceUri()), null, false);
	if (r != E_SUCCESS)
		AppLogException(
				"Error constructing title background image from media [%ls]: [%s]", pMedia->GetSourceUri()->GetPointer(), GetErrorMessage(r));

	int deltaH = int(
			(imgBuf.GetWidth() * __pTitleRect->height
					- __pTitleRect->width * imgBuf.GetHeight())
					/ (-1.0 * __pTitleRect->width));
	ImageBuffer* croppedBuf = imgBuf.CropN(0, deltaH / 2, imgBuf.GetWidth(),
			imgBuf.GetHeight() - deltaH / 2);
	r = GetLastResult();
	if (r != E_SUCCESS)
		AppLogException(
				"Error cropping title background image from media [%ls]: [%s]", pMedia->GetSourceUri()->GetPointer(), GetErrorMessage(r));

	ImageBuffer* resizedBuf = croppedBuf->ResizeN(__pTitleRect->width,
			__pTitleRect->height);
	r = GetLastResult();
	if (r != E_SUCCESS)
		AppLogException(
				"Error resizing title background image from media [%ls]: [%s]", pMedia->GetSourceUri()->GetPointer(), GetErrorMessage(r));

	Bitmap* pTitleBgBitmap = resizedBuf->GetBitmapN(BITMAP_PIXEL_FORMAT_RGB565,
			BUFFER_SCALING_AUTO);
	r = GetLastResult();
	if (r != E_SUCCESS)
		AppLogException(
				"Error construction bitmap image from media [%ls]: [%s]", pMedia->GetSourceUri()->GetPointer(), GetErrorMessage(r));

	delete croppedBuf;
	delete resizedBuf;

	return pTitleBgBitmap;
}

result PoiForm::CreateFacebookAlbum(void) {

	result r = E_SUCCESS;

	__pFbEnum = __pPoi->GetAssociatedMedia()->GetEnumeratorN();

	HttpSession* pSession = null;
	HttpTransaction* pTransaction = null;
	HttpRequest* pRequest = null;
	HttpMultipartEntity* pMultipartEntity = null;

	//HttpMultipartEntity* pMultipartEntity = null;
	String hostAddr(L"https://graph.facebook.com");

	String reqUri = String(hostAddr);
	reqUri.Append(L"/v2.1/me/albums?");
	reqUri.Append(L"access_token=");
	reqUri.Append(
			StorageManager::getInstance()->GetFacebookCredentials().AccessToken);

	pMultipartEntity = new HttpMultipartEntity();
	pMultipartEntity->Construct();
	pMultipartEntity->AddStringPart(L"name", *(__pPoi->GetTitle()));
	pMultipartEntity->AddStringPart(L"message", *(__pPoi->GetDescription()));

	AppLog("Creating http post request %ls", reqUri.GetPointer());
	// Creates an HTTP session.
	pSession = new HttpSession();
	r = pSession->Construct(NET_HTTP_SESSION_MODE_NORMAL, null, hostAddr, null);
	if (r != E_SUCCESS) {
		AppLogException( "Error constructing http session", GetErrorMessage(r));
		return r;
	}

	pTransaction = pSession->OpenTransactionN();
	r = pTransaction->AddHttpTransactionListener(*this);
	r = pTransaction->SetHttpProgressListener(*this);

	pRequest = pTransaction->GetRequest();
	pRequest->SetMethod(NET_HTTP_METHOD_POST);
	pRequest->SetEntity(*pMultipartEntity);

	r = pRequest->SetUri(reqUri);
	if (r != E_SUCCESS) {
		AppLogException("Error http request URI", GetErrorMessage(r));
		return r;
	}

	pTransaction->SetUserObject(pMultipartEntity);
	r = pTransaction->Submit();
	if (r != E_SUCCESS) {
		AppLogException(
				"Error submitting http transaction", GetErrorMessage(r));
		return r;
	}

	return r;

}

result PoiForm::CreateFacebookPhoto(void) {
	result r = E_SUCCESS;

	HttpSession* pSession = null;
	HttpTransaction* pTransaction = null;
	HttpRequest* pRequest = null;
	HttpMultipartEntity* pMultipartEntity = null;

	//HttpMultipartEntity* pMultipartEntity = null;
	String hostAddr(L"https://graph.facebook.com");

	String reqUri = String(hostAddr);
	reqUri.Append(L"/v2.1/");
	reqUri.Append(__fbAlbumId);
	reqUri.Append("/photos?");
	reqUri.Append(L"access_token=");
	reqUri.Append(
			StorageManager::getInstance()->GetFacebookCredentials().AccessToken);

	TTMedia* pMedia;
	__pFbEnum->GetCurrent(pMedia);

	AppLog("Creating http post request %ls", reqUri.GetPointer());
	__fbPostCounter++;
	// Creates an HTTP session.
	pSession = new HttpSession();
	r = pSession->Construct(NET_HTTP_SESSION_MODE_NORMAL, null, hostAddr, null);
	if (r != E_SUCCESS) {
		AppLogException( "Error constructing http session", GetErrorMessage(r));
		return r;
	}

	pTransaction = pSession->OpenTransactionN();
	r = pTransaction->AddHttpTransactionListener(*this);
	r = pTransaction->SetHttpProgressListener(*this);

	pRequest = pTransaction->GetRequest();
	pRequest->SetMethod(NET_HTTP_METHOD_POST);

	r = pRequest->SetUri(reqUri);
	if (r != E_SUCCESS) {
		AppLogException("Error http request URI", GetErrorMessage(r));
		return r;
	}

	AppLog(
			"Adding media with id:%lld uri: %ls", pMedia->GetId(), pMedia->GetSourceUri()->GetPointer());
	pMultipartEntity = new HttpMultipartEntity();
	pMultipartEntity->Construct();
	String filePath(*(pMedia->GetSourceUri()));
	int i;
	filePath.LastIndexOf('/', 0, i);
	String fileName;
	filePath.SubString(i + 1, fileName);

	pMultipartEntity->AddFilePart("source", String(*(pMedia->GetSourceUri())));

	pRequest->SetEntity(*pMultipartEntity);
	pTransaction->SetUserObject(pMultipartEntity);
	r = pTransaction->Submit();
	if (r != E_SUCCESS) {
		AppLogException(
				"Error submitting http transaction", GetErrorMessage(r));
		return r;
	}

	return r;
}

result PoiForm::ParseAlbumResponse(Tizen::Base::ByteBuffer* pBuffer) {
	result r = E_SUCCESS;

	int count = 0;
	String body = L"";
	pBuffer->Reset();

	AppLog("Parsing http response body for fb album id");

	while (pBuffer->HasRemaining()) {
		byte val;
		r = pBuffer->GetByte(val);

		if (r != E_SUCCESS)
			AppLogException(
					"Error reading byte number %d", count, GetErrorMessage(r));
		else {
			body.Append((char) val);
			count++;
		}
	}

	AppLog("Read %d bytes.\n", body.GetLength());
	AppLog("%ls", body.GetPointer());

	StringTokenizer tokenizer(body, " {}:\"");
	while (tokenizer.HasMoreTokens()) {
		String token;
		tokenizer.GetNextToken(token);
		if (token == L"id") {
			tokenizer.GetNextToken(token);
			LongLong::Parse(token, __fbAlbumId);
			break;
		}
	}
	return r;
}

