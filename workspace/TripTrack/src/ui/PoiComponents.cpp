/*
 * PoiComponents.cpp
 *
 *  Created on: Aug 14, 2013
 *      Author: bohemius
 */
#include <FMedia.h>
#include "ui/PoiComponents.h"
#include "dao/StorageManager.h"
#include "util/GraphicsUtils.h"

using namespace Tizen::Ui::Controls;
using namespace Tizen::Base;
using namespace Tizen::Base::Collection;
using namespace Tizen::Graphics;
using namespace Tizen::App;
using namespace Tizen::Media;

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
	tile_width = (GetBoundsF().width - TILES_PER_ROW * TILES_SPACING_X
			- TILES_SPACING_X) / TILES_PER_ROW;
	tile_height = (GetBoundsF().height - TILES_PER_COLUMN * TILES_SPACING_Y
			- TILES_SPACING_Y) / TILES_PER_COLUMN;

	r = __pPoiIconListView->Construct(
			FloatRectangle(TILES_OFFSET_X, TILES_OFFSET_Y, GetBoundsF().width,
					GetBoundsF().height),
			FloatDimension(tile_width, tile_height),
			ICON_LIST_VIEW_STYLE_NORMAL,
			ICON_LIST_VIEW_SCROLL_DIRECTION_VERTICAL);
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
			ICON_LIST_VIEW_ITEM_BORDER_STYLE_OUTLINE);
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

	ImageBuffer imgBuf;
	r = imgBuf.Construct(*(pMedia->GetContent()),(int) tile_width, (int) tile_height,IMAGE_SCALING_METHOD_BICUBIC);
	if (r != E_SUCCESS)
		AppLogException("Error constructing image buffer: [%s]", GetErrorMessage(r));
	Bitmap* pPoiTile;
	//r = pPoiTile->Construct(*buf,	Dimension((int) tile_width, (int) tile_height),	BITMAP_PIXEL_FORMAT_RGB565);
	//pPoiTile = img->DecodeN(*buf,BITMAP_PIXEL_FORMAT_RGB565,tile_width, tile_height);
	pPoiTile=imgBuf.GetBitmapN(BITMAP_PIXEL_FORMAT_RGB565, BUFFER_SCALING_AUTO);
	if (r != E_SUCCESS) {
		AppLogException(
				"Error construction bitmap image from media with id [%d]: [%s]", pMedia->GetId(), GetErrorMessage(r));
	}
	IconListViewItem* pPoiItem = new (std::nothrow) IconListViewItem();
	r = pPoiItem->Construct(*pPoiTile, pPoi->GetTitle());
	if (r != E_SUCCESS) {
		AppLogException(
				"Error constructing icon list item for poi [%s]: [%s]", pPoi->GetTitle()->GetPointer(), GetErrorMessage(r));
	}

	delete pMedia;
	delete pPoiTile;

	/*String* text = new String(L"Poi");
	 text->Append(index);
	 switch (index % 5) {
	 case 0:
	 pPoiItem->Construct(*__pTile1, text);
	 break;
	 case 1:
	 pPoiItem->Construct(*__pTile2, text);
	 break;
	 case 2:
	 pPoiItem->Construct(*__pTile3, text);
	 break;
	 case 3:
	 pPoiItem->Construct(*__pTile4, text);
	 break;
	 case 4:
	 pPoiItem->Construct(*__pTile5, text);
	 break;
	 }*/

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

	__pPoiCollection = StorageManager::getInstance()->GetPois();
	AppLog("Loaded [%d] pois", __pPoiCollection->GetCount());

	//load sample bitmaps until i get the reading from db working to show something
	AppResource* pAppRes = Application::GetInstance()->GetAppResource();

	__pTile1 = pAppRes->GetBitmapN(L"sample1.png");
	__pTile2 = pAppRes->GetBitmapN(L"sample2.png");
	__pTile3 = pAppRes->GetBitmapN(L"sample3.png");
	__pTile4 = pAppRes->GetBitmapN(L"sample4.png");
	__pTile5 = pAppRes->GetBitmapN(L"sample5.png");

	return r;
}
