/*
 * GalleryForm.cpp
 *
 *  Created on: Sep 7, 2014
 *      Author: bohemius
 */

#include <FBase.h>
#include <FGraphics.h>
#include <FUi.h>
#include "SceneRegister.h"
#include "ui/GalleryForm.h"
#include "dao/StorageManager.h"
#include "util/GraphicsUtils.h"

using namespace Tizen::Base;
using namespace Tizen::Base::Collection;
using namespace Tizen::Ui;
using namespace Tizen::Ui::Controls;
using namespace Tizen::Ui::Scenes;
using namespace Tizen::Graphics;

GalleryForm::~GalleryForm() {
	// TODO Auto-generated destructor stub
}

bool GalleryForm::Initialize(void) {
	Construct(FORM_STYLE_NORMAL);
	return true;
}

result GalleryForm::OnInitializing(void) {
	result r = E_SUCCESS;

	// Creates an instance of Gallery
	__pGallery = new Gallery();
	__pGallery->Construct(GetBounds());
	__pGallery->SetItemProvider(*this);
	__pGallery->AddGalleryEventListener(*this);
	SetFormBackEventListener(this);

	AddControl(__pGallery);

	return r;
}

void GalleryForm::OnGalleryCurrentItemChanged(
		Tizen::Ui::Controls::Gallery& gallery, int index) {
}

void GalleryForm::OnGalleryItemClicked(Tizen::Ui::Controls::Gallery& gallery,
		int index) {
}

void GalleryForm::OnGallerySlideShowStarted(
		Tizen::Ui::Controls::Gallery& gallery) {
}

void GalleryForm::OnFormBackRequested(Tizen::Ui::Controls::Form& source) {
	LinkedList* paramList = new LinkedList();
	paramList->Add(__pPoi);

	SceneManager* pSceneMngr = SceneManager::GetInstance();
	pSceneMngr->GoBackward(BackwardSceneTransition(__previousScene),paramList);
}

void GalleryForm::OnSceneActivatedN(
		const Tizen::Ui::Scenes::SceneId& previousSceneId,
		const Tizen::Ui::Scenes::SceneId& currentSceneId,
		Tizen::Base::Collection::IList* pArgs) {
	result r = E_SUCCESS;

	__previousScene=previousSceneId;

	/*Get the poi from passed arguments*/
	Object* pPoiParam = pArgs->GetAt(0);
	__pPoi = static_cast<POI*>(pPoiParam);
	AppLog("PoiForm got POI [%ls]", __pPoi->GetTitle()->GetPointer());
	Object* pSelectedIndexParam = pArgs->GetAt(1);
	__selectedIndex = Integer(*(static_cast<Integer*>(pSelectedIndexParam)));

	__pGallery->SetBackgroundColor(Color::GetColor(COLOR_ID_BLACK));
	__pGallery->UpdateGallery();
	__pGallery->SetCurrentItemIndex(__selectedIndex.value);
}

void GalleryForm::OnSceneDeactivated(
		const Tizen::Ui::Scenes::SceneId& currentSceneId,
		const Tizen::Ui::Scenes::SceneId& nextSceneId) {
}

void GalleryForm::OnGallerySlideShowStopped(
		Tizen::Ui::Controls::Gallery& gallery) {
}

Tizen::Ui::Controls::GalleryItem* GalleryForm::CreateItem(int index) {
	result r = E_SUCCESS;

	TTMedia* pMedia = null;
	r = __pPoi->GetAssociatedMedia()->GetAt(index, pMedia);
	if (r != E_SUCCESS || pMedia == null) {
		AppLogException(
				"Error getting Media for gallery item with index [%d]: ", index, GetErrorMessage(r));
	}

	Bitmap * pImageTemp = GraphicsUtils::CreateBitmap(
			*(pMedia->GetSourceUri()));

	GalleryItem* pGalleryItem = new GalleryItem();
	pGalleryItem->Construct(*pImageTemp);

	// Deallocates the bitmap
	delete pImageTemp;

	return pGalleryItem;
}

bool GalleryForm::DeleteItem(int index,
		Tizen::Ui::Controls::GalleryItem* pItem) {
	delete pItem;
	pItem = null;
	return true;
}

int GalleryForm::GetItemCount(void) {
	if (__pPoi != null)
		return __pPoi->GetAssociatedMedia()->GetCount();
	else
		return 0;
}

