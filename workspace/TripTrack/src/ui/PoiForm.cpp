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

using namespace Tizen::Base;
using namespace Tizen::Base::Collection;
using namespace Tizen::Media;
using namespace Tizen::Ui::Controls;
using namespace Tizen::Graphics;
using namespace Tizen::App;
using namespace Tizen::Ui::Scenes;
using namespace Tizen::Locations;

PoiForm::PoiForm() :
		__pTitleLabel(null), __pDescriptionLabel(null), __pPoiPanel(null), __pMediaIconListView(
				null), __pPoi(null) {
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
	__pClientBounds = new Rectangle(GetClientAreaBounds());
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
		//todo
	}
		break;
	case ID_FOOTER_BUTTON_MAP: {
		//TODO milestone 2
	}
		break;
	case ID_FOOTER_BUTTON_FB: {
		//TODO milestone 2
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
	pSceneMngr->GoBackward(BackwardSceneTransition(SCENE_MAIN_FORM));
}

result PoiForm::LoadResources(void) {
	result r = E_SUCCESS;

	AppResource* pAppRes = Application::GetInstance()->GetAppResource();

	__pCameraBitmap = pAppRes->GetBitmapN(L"camera.png");
	__pEditBitmap = pAppRes->GetBitmapN(L"edit.png");
	__pFbBitmap = pAppRes->GetBitmapN(L"facebook.png");
	__pLocationBitmap = pAppRes->GetBitmapN(L"location.png");

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
	} else
		__pPoiPanel->RemoveAllControls();

	/*Create the media for title background*/
	TTMedia* pMedia = new TTMedia();
	r = pMedia->Construct(__pPoi->GetDefImageId());
	if (r != E_SUCCESS)
		AppLogException(
				"Error constructing media with ID [%ld]: [%s]", __pPoi->GetDefImageId(), GetErrorMessage(r));

	ImageBuffer imgBuf;
	r = imgBuf.Construct(*(pMedia->GetSourceUri()), __pTitleRect->width,
			__pTitleRect->height, IMAGE_SCALING_METHOD_BICUBIC);
	if (r != E_SUCCESS)
		AppLogException(
				"Error constructing title background image from media [%ls]: [%s]", pMedia->GetSourceUri()->GetPointer(), GetErrorMessage(r));

	Bitmap* pTitleBgBitmap = imgBuf.GetBitmapN(BITMAP_PIXEL_FORMAT_RGB565,
			BUFFER_SCALING_AUTO);
	if (r != E_SUCCESS)
		AppLogException(
				"Error construction bitmap image from media [%ls]: [%s]", pMedia->GetSourceUri()->GetPointer(), GetErrorMessage(r));

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
		__pTitleLabel->SetTextConfig(TITLE_TEXT_SIZE, LABEL_TEXT_STYLE_BOLD);
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
				ICON_LIST_VIEW_STYLE_MARK,
				ICON_LIST_VIEW_SCROLL_DIRECTION_HORIZONTAL);
		if (r != E_SUCCESS)
			AppLogException(
					"Error constructing icon list view from media collection: [%s]", GetErrorMessage(r));
	}
	__pMediaIconListView->SetItemProvider(*this);
	__pMediaIconListView->AddIconListViewItemEventListener(*this);
	__pMediaIconListView->SetItemBorderStyle(ICON_LIST_VIEW_ITEM_BORDER_STYLE_OUTLINE);
	__pMediaIconListView->SetItemSpacing(TILES_SPACING_X, TILES_SPACING_Y);
	__pPoiPanel->AddControl(__pMediaIconListView);

	AddControl(__pPoiPanel);
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

	delete pMedia;
	delete pMediaTile;

	return pPoiItem;
}

bool PoiForm::DeleteItem(int index,
		Tizen::Ui::Controls::IconListViewItem* pItem) {
}

int PoiForm::GetItemCount(void) {
	return __pPoi->GetAssociatedMedia()->GetCount();
}

result PoiForm::LoadImageList(void) {
	result r = E_SUCCESS;

	StorageManager* store = StorageManager::getInstance();
	LinkedListT<TTMedia*>* pMediaList = store->GetMedia(__pPoi->GetId());
	__pPoi->GetAssociatedMedia()->RemoveAll();
	__pPoi->GetAssociatedMedia()->AddItems(*pMediaList);

	return r;
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
