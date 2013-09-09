/*
 * PoiForm.cpp
 *
 *  Created on: Sep 2, 2013
 *      Author: bohemius
 */

#include <FApp.h>
#include <FGraphics.h>
#include <FMedia.h>
#include "ui/PoiForm.h"
#include "dao/StorageManager.h"
#include "AppResourceId.h"
#include "SceneRegister.h"

using namespace Tizen::Base;
using namespace Tizen::Base::Collection;
using namespace Tizen::Media;
using namespace Tizen::Ui::Controls;
using namespace Tizen::Graphics;
using namespace Tizen::App;
using namespace Tizen::Ui::Scenes;

PoiForm::PoiForm() :
		__pTitleLabel(null), __pDescriptionLabel(null), __pPoiScrollPanel(null), __pMediaIconListView(
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

	__pClientBounds = new Rectangle(GetClientAreaBounds());
	__pTitleRect = new Rectangle(0, 0, __pClientBounds->width,
			__pClientBounds->height / 6);
	__pDescRect = new Rectangle(0, __pClientBounds->height / 6 + 1,
			__pClientBounds->width, __pClientBounds->height / 6);
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

	/*Create the scroll panel*/
	if (__pPoiScrollPanel == null) {
		__pPoiScrollPanel=new ScrollPanel();
		r = __pPoiScrollPanel->Construct(*__pClientBounds);
	} else
		__pPoiScrollPanel->RemoveAllControls();

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
	__pPoiScrollPanel->AddControl(__pTitleLabel);

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
	__pPoiScrollPanel->AddControl(__pDescriptionLabel);

	AddControl(__pPoiScrollPanel);
}

void PoiForm::OnSceneDeactivated(
		const Tizen::Ui::Scenes::SceneId& currentSceneId,
		const Tizen::Ui::Scenes::SceneId& nextSceneId) {
}

void PoiForm::OnAppControlCompleteResponseReceived(
		const Tizen::App::AppId& appId, const Tizen::Base::String& operationId,
		Tizen::App::AppCtrlResult appControlResult,
		const Tizen::Base::Collection::IMap* pExtraData) {
}

void PoiForm::OnIconListViewItemStateChanged(
		Tizen::Ui::Controls::IconListView& view, int index,
		Tizen::Ui::Controls::IconListViewItemStatus status) {
}

Tizen::Ui::Controls::IconListViewItem* PoiForm::CreateItem(int index) {
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

