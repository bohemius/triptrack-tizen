/*
 * PoiComponents.cpp
 *
 *  Created on: Aug 14, 2013
 *      Author: bohemius
 */
#include <FMedia.h>
#include <FSystem.h>
#include "ui/PoiComponents.h"
#include "dao/StorageManager.h"
#include "util/GraphicsUtils.h"
#include "SceneRegister.h"

using namespace Tizen::Ui::Controls;
using namespace Tizen::Base;
using namespace Tizen::Base::Collection;
using namespace Tizen::Graphics;
using namespace Tizen::App;
using namespace Tizen::Media;
using namespace Tizen::Ui::Scenes;
using namespace Tizen::Ui;

PoiIconListPanel::PoiIconListPanel(LinkedListT<POI*>* pCollection) :
		Panel(), __pPoiIconListView(null), __pPoiCollection(pCollection) {
}

PoiIconListPanel::~PoiIconListPanel(void) {
	delete __pPoiCollection;
	delete __pPoiIconListView;
}

result PoiIconListPanel::Construct() {
	result r = E_SUCCESS;

	int count = __pPoiCollection->GetCount();
	int rowCount;
	if (count % 2 > 0)
		rowCount = rowCount / 2 + 1;
	else
		rowCount = rowCount / 2;

	__pPoiIconListView = new IconListView();
	tile_width = (GetBoundsF().width
			- (TILES_PER_ROW * TILES_SPACING_X + TILES_SPACING_X))
			/ TILES_PER_ROW;
	tile_height = (GetBoundsF().height - TILES_PER_COLUMN * TILES_SPACING_Y
			- TILES_SPACING_Y) / TILES_PER_COLUMN;

	r = __pPoiIconListView->Construct(
			FloatRectangle(TILES_OFFSET_X, TILES_OFFSET_Y,
					tile_width * TILES_PER_ROW + TILES_SPACING_X,
					(tile_height * 2 * TILES_SPACING_Y) * rowCount
							+ TILES_OFFSET_Y),
			FloatDimension(tile_width, tile_height),
			ICON_LIST_VIEW_STYLE_NORMAL);
	/*r = __pPoiIconListView->Construct(
	 FloatRectangle(0.0f, 0.0f, 720.0f, 1026.0f),
	 FloatDimension(175.0f, 198.0f ), ICON_LIST_VIEW_STYLE_MARK,
	 ICON_LIST_VIEW_SCROLL_DIRECTION_VERTICAL);*/
	if (r != E_SUCCESS) {
		AppLogException(
				"Error constructing poi icon list view: [%s]", GetErrorMessage(r));
		return r;
	}
	r = __pPoiIconListView->SetItemProvider(*this);
	if (r != E_SUCCESS) {
		AppLogException(
				"Error setting poi icon list item provider: [%s]", GetErrorMessage(r));
		return r;
	}
	__pPoiIconListView->AddIconListViewItemEventListener(*this);
	__pPoiIconListView->SetCheckBoxPosition(
			ICON_LIST_VIEW_CHECK_BOX_POSITION_TOP_LEFT);
	__pPoiIconListView->SetItemSpacing(TILES_SPACING_X, TILES_SPACING_Y);
	__pPoiIconListView->SetItemBorderStyle(
			ICON_LIST_VIEW_ITEM_BORDER_STYLE_NONE);
	__pPoiIconListView->SetItemTextSize(100);
	AddControl(*__pPoiIconListView);

	return r;
}

void PoiIconListPanel::OnIconListViewItemStateChanged(
		Tizen::Ui::Controls::IconListView& view, int index,
		Tizen::Ui::Controls::IconListViewItemStatus status) {
	result r = E_SUCCESS;

	POI* selectedPoi;
	LinkedList* parameterList = new LinkedList();

	r = __pPoiCollection->GetAt(index, selectedPoi);
	if (r != E_SUCCESS) {
		AppLogException(
				"Error getting poi with index [%d] from poi collection: [%s]", index, GetErrorMessage(r));
	} else {
		parameterList->Add(selectedPoi);
		SceneManager* pSceneMngr = SceneManager::GetInstance();
		pSceneMngr->GoForward(ForwardSceneTransition(SCENE_POI_FORM),
				parameterList);
	}
}

Tizen::Ui::Controls::IconListViewItem* PoiIconListPanel::CreateItem(int index) {
	result r = E_SUCCESS;

	TTMedia* pMedia = new TTMedia();

	POI* pPoi = null;
	r = __pPoiCollection->GetAt(index, pPoi);
	if (r != E_SUCCESS || pPoi == null) {
		AppLogException(
				"Error setting poi from poi collection: [%s]", GetErrorMessage(r));
	}

	r = pMedia->Construct(pPoi->GetDefImageId());
	if (r != E_SUCCESS) {
		AppLogException(
				"Error getting default image data from database for poi [%ls]: [%s]", pPoi->GetTitle()->GetPointer(), GetErrorMessage(r));
	}

	ImageBuffer imgBuf;
	r = imgBuf.Construct(*(pMedia->GetContent()), (int) tile_width,
			(int) tile_height, IMAGE_SCALING_METHOD_BICUBIC);
	if (r != E_SUCCESS)
		AppLogException(
				"Error constructing image buffer: [%s]", GetErrorMessage(r));
	Bitmap* pPoiTile;
	pPoiTile = imgBuf.GetBitmapN(BITMAP_PIXEL_FORMAT_RGB565,
			BUFFER_SCALING_AUTO);
	if (r != E_SUCCESS) {
		AppLogException(
				"Error construction bitmap image from media with id [%d]: [%s]", pMedia->GetId(), GetErrorMessage(r));
	}
	IconListViewItem* pPoiItem = new (std::nothrow) IconListViewItem();
	r = pPoiItem->Construct(*pPoiTile, BuildTileText(pPoi));
	if (r != E_SUCCESS) {
		AppLogException(
				"Error constructing icon list item for poi [%s]: [%s]", pPoi->GetTitle()->GetPointer(), GetErrorMessage(r));
	}

	delete pMedia;
	delete pPoiTile;

	return pPoiItem;
}

bool PoiIconListPanel::DeleteItem(int index,
		Tizen::Ui::Controls::IconListViewItem* pItem) {
	delete pItem;
	return true;
}

int PoiIconListPanel::GetItemCount(void) {
	return __pPoiCollection->GetCount();
}

Tizen::Base::String* PoiIconListPanel::BuildTileText(POI* pPoi) {
	String* retVal = new String(L"");

	retVal->Append(pPoi->GetTimestamp()->ToString());
	retVal->Append(L"\n\n");
	retVal->Append(*(pPoi->GetTitle()));
	retVal->Append(L"\n");
	retVal->Append(*(pPoi->GetDescription()));

	return retVal;
}

result PoiScrollPanel::UpdatePoiCollection(void) {
	result r = E_SUCCESS;

	if (__pPoiMap == null)
		return E_OBJ_NOT_FOUND;
	else {
		__pPoiMap->RemoveAll();
		delete __pPoiMap;
		__pPoiMap = null;
	}

	//load pois from the database
	StorageManager* pStore = StorageManager::getInstance();

	__pPoiMap = pStore->GetPoiHash();
	if (__pPoiMap == null) {
		AppLogException("Error getting pois from database.");
		return E_FAILURE;
	}

	//TODO update
	/*
	 if (r != E_SUCCESS) {
	 AppLogException(
	 "Error updating poi icon list: [%s]", GetErrorMessage(r));
	 return r;
	 }*/
	return r;
}

PoiScrollPanel::PoiScrollPanel(void) :
		__pPoiMap(null), __pPanelCollection(null) {
}

PoiScrollPanel::~PoiScrollPanel(void) {
}

result PoiScrollPanel::Construct(void) {
	result r = E_SUCCESS;

	__pPanelCollection = new LinkedListT<PoiIconListPanel*>();

	VerticalBoxLayout layout;
	r = layout.Construct(VERTICAL_DIRECTION_DOWNWARD);
	if (r != E_SUCCESS) {
		AppLogException(
				"Error constructing vertical layout", GetErrorMessage(r));
		return r;
	}
	ScrollPanel::Construct(layout, GetBoundsF());
	r = LoadResources();
	if (r != E_SUCCESS) {
		AppLogException(
				"Error loading resources for poi scroll panel: [%s]", GetErrorMessage(r));
		return r;
	}

	int count = 0;
	IEnumeratorT<DateTime>* pEnum = __pPoiMap->GetKeysN()->GetEnumeratorN();
	while (pEnum->MoveNext() == E_SUCCESS) {
		LinkedListT<POI*>* pCollection = null;
		DateTime date;
		r = pEnum->GetCurrent(date);
		if (r != E_SUCCESS) {
			AppLogException(
					"Error getting date from enumerator:", GetErrorMessage(r));
			return r;
		}
		r = __pPoiMap->GetValue(date, pCollection);
		if (r != E_SUCCESS) {
			AppLogException(
					"Error getting poi collection for date [%ls] from hashmap:", date.ToString(). GetPointer(), GetErrorMessage(r));
			return r;
		}
		PoiIconListPanel* pPoiPanel = new PoiIconListPanel(pCollection);
		r = __pPanelCollection->Add(pPoiPanel);
		if (r != E_SUCCESS) {
			AppLogException(
					"Error add poi icon list panel to collection:", GetErrorMessage(r));
			return r;
		}
	}

	return r;
}

result PoiScrollPanel::LoadResources(void) {
	result r = E_SUCCESS;

	__pPoiMap = StorageManager::getInstance()->GetPoiHash();
	AppLog("Loaded [%d] pois", __pPoiMap->GetCount());

	return r;
}

