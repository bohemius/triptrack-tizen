/*
 * PoiComponents.cpp
 *
 *  Created on: Aug 14, 2013
 *      Author: bohemius
 */
#include "ui/PoiComponents.h"
#include "dao/StorageManager.h"
#include "util/GraphicsUtils.h"

using namespace Tizen::Ui::Controls;
using namespace Tizen::Base;
using namespace Tizen::Base::Collection;
using namespace Tizen::Graphics;

PoiIconListPanel::PoiIconListPanel(Tizen::Graphics::Rectangle& rect) :
		Panel(), __pPoiIconListView(null), __pPoiCollection(null) {
	Panel::Construct(Rectangle(0, 0, rect.width, rect.height));
}

PoiIconListPanel::~PoiIconListPanel(void) {
	delete __pPoiCollection;
	delete __pPoiIconListView;
}

result PoiIconListPanel::Construct(void) {
	result r = E_SUCCESS;

	r = LoadResources();
	if (r != E_SUCCESS) {
		AppLogException(
				"Error loading resources for poi icon list view: [%s]", GetErrorMessage(r));
		return r;
	}

	__pPoiIconListView = new IconListView();
	tile_width = (GetBoundsF().width - (TILES_PER_ROW - 1.0f) * TILES_SPACING_X
			+ TILES_SPACING_X) / TILES_PER_ROW;
	tile_height = tile_width * 1.764f;

	r = __pPoiIconListView->Construct(
			FloatRectangle(0.0f, 0.0f, GetBoundsF().width, GetBoundsF().height),
			FloatDimension(tile_width, tile_height), ICON_LIST_VIEW_STYLE_MARK,
			ICON_LIST_VIEW_SCROLL_DIRECTION_VERTICAL);
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
	AddControl(*__pPoiIconListView);

	return r;
}

void PoiIconListPanel::OnIconListViewItemStateChanged(
		Tizen::Ui::Controls::IconListView& view, int index,
		Tizen::Ui::Controls::IconListViewItemStatus status) {
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

	r = pMedia->Construct(pPoi->GetId());
	if (r != E_SUCCESS) {
		AppLogException(
				"Error getting default image data from database for poi [%ls]: [%s]", pPoi->GetTitle()->GetPointer(), GetErrorMessage(r));
	}

	Bitmap* pPoiTile = GraphicsUtils::CreateBitmap(pMedia->GetContent(),
			tile_width, tile_height);
	IconListViewItem* pPoiItem = new (std::nothrow) IconListViewItem();
	r = pPoiItem->Construct(*pPoiTile, pPoi->GetTitle());
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

result PoiIconListPanel::UpdatePoiCollection(void) {
	result r = E_SUCCESS;

	__pPoiCollection->RemoveAll();
	//load pois from the database
	StorageManager* pStore = StorageManager::getInstance();

	__pPoiCollection->AddItems(*pStore->GetPois());

	r = __pPoiIconListView->UpdateList();
	if (r != E_SUCCESS) {
		AppLogException(
				"Error updating poi icon list: [%s]", GetErrorMessage(r));
		return r;
	}
	return r;
}

result PoiIconListPanel::LoadResources(void) {
	result r = E_SUCCESS;

	if (__pPoiCollection == null)
		__pPoiCollection = new LinkedListT<POI*>();

	r = __pPoiCollection->AddItems(*(StorageManager::getInstance()->GetPois()));
	if (r != E_SUCCESS) {
		AppLogException(
				"Error adding items pois to collection: [%s]", GetErrorMessage(r));
		return r;
	}

	return r;
}
