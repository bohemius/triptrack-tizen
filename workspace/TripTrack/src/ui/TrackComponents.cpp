/*
 * TrackComponents.cpp
 *
 *  Created on: Aug 14, 2013
 *      Author: bohemius
 */

#include "ui/TrackComponents.h"

using namespace Tizen::Graphics;
using namespace Tizen::Ui::Controls;
using namespace Tizen::App;
using namespace Tizen::Base;

TrackListElement::TrackListElement() {
}

TrackListElement::~TrackListElement(void) {
}

bool TrackListElement::OnDraw(Canvas& canvas, const FloatRectangle& rect,
		ListItemDrawingStatus itemStatus) {
	Font titleFont, mileageFont, timeFont;
	titleFont.Construct(FONT_STYLE_BOLD, 25.0f);
	canvas.SetFont(titleFont);
	//canvas.SetLineWidth(LINE_WITH);
	canvas.SetForegroundColor(Color::GetColor(COLOR_ID_WHITE));

	//check if the item is checked and set the tracker state
	AppLog("Item drawing status: [%d]", itemStatus);
	if (itemStatus==LIST_ITEM_DRAWING_STATUS_PRESSED)
		TrackListElement::STATE == ON ? TrackListElement::STATE == OFF: TrackListElement::STATE == ON;

	//draw track title
	if (canvas.DrawText(
			Tizen::Graphics::FloatPoint(rect.x, rect.y + TEXT_MARGIN_Y),
			L"Track title") != E_SUCCESS) {
		return false;
	}

	//draw mileage
	mileageFont.Construct(FONT_STYLE_PLAIN, 20.0f);
	canvas.SetFont(mileageFont);
	if (canvas.DrawText(
			Tizen::Graphics::FloatPoint(rect.x,
					rect.y + rect.height - mileageFont.GetMaxHeightF()
							- TEXT_MARGIN_Y),
			L"Total mileage: 1245.7 miles") != E_SUCCESS) {
		return false;
	}

	//draw duration
	canvas.SetForegroundColor(Color::GetColor(COLOR_ID_YELLOW));
	timeFont.Construct(FONT_STYLE_MIN, 20.0f);
	canvas.SetFont(timeFont);
	String durationStr(L"Duration 123.3 Hours");
	float length = (float) durationStr.GetLength();
	if (canvas.DrawText(
			Tizen::Graphics::FloatPoint(rect.x + rect.width - 200,
					rect.y + TEXT_MARGIN_Y),
			L"Duration 123.3 Hours") != E_SUCCESS) {
		return false;
	}

	//calculate indicator rectangle using element height
	float diameterDisk = mileageFont.GetMaxHeightF();
	FloatRectangle diskRect(rect.x + rect.width - diameterDisk,
			rect.y + rect.height - diameterDisk - TEXT_MARGIN_Y, diameterDisk,
			diameterDisk);
	//draw track title
	if (canvas.FillEllipse(STATE==ON ? Color::GetColor(COLOR_ID_GREEN) : Color::GetColor(COLOR_ID_RED), diskRect) != E_SUCCESS) {
		return false;
	}

	return true;
}

TrackListPanel::TrackListPanel(Rectangle &rect) :
		Panel(), __pTrackListView(null) {
	Panel::Construct(Rectangle(0, 0, rect.width, rect.height));
}

TrackListPanel::~TrackListPanel(void) {
}

result TrackListPanel::Construct(void) {
	result r = E_SUCCESS;
	_trackingIndex = -1;

	//load resources
	r = LoadResources();

	if (r != E_SUCCESS) {
		AppLogException(
				"Error loading resources for track list view: [%s]", GetErrorMessage(r));
		return r;
	}

	//construct list context items
	__pTrackListContextItem = new (std::nothrow) ListContextItem();

	r = __pTrackListContextItem->Construct();
	if (r != E_SUCCESS) {
		AppLogException(
				"Error constructing track list context item: [%s]", GetErrorMessage(r));
	}

	__pTrackListContextItem->AddElement(ID_CONTEXT_ITEM_MAP, L"Map",
			*__pMapBitmap, *__pMapBitmap, null, true);
	__pTrackListContextItem->AddElement(ID_CONTEXT_ITEM_EDIT, L"Map",
			*__pEditBitmap, *__pEditBitmap, null, true);
	__pTrackListContextItem->AddElement(ID_CONTEXT_ITEM_DELETE, L"Map",
			*__pDeleteBitmap, *__pDeleteBitmap, null, true);
	__pTrackListContextItem->SetBackgroundColor(Color(46, 151, 199));

	//construct context menu
	__pTrackListContextMenu = new (std::nothrow) ContextMenu();

	r = __pTrackListContextMenu->Construct(FloatPoint(200.0, 600.0),
			CONTEXT_MENU_STYLE_LIST);
	if (r != E_SUCCESS) {
		AppLogException(
				"Error constructing track list context menu: [%s]", GetErrorMessage(r));
	}
	__pTrackListContextMenu->SetFocusable(true);
	__pTrackListContextMenu->AddItem(L"Show map", ID_CONTEXT_ITEM_MAP,
			*__pMapBitmap, __pMapBitmap, __pMapBitmap);
	__pTrackListContextMenu->AddItem(L"Edit", ID_CONTEXT_ITEM_EDIT,
			*__pEditBitmap, __pEditBitmap, __pEditBitmap);
	__pTrackListContextMenu->AddItem(L"Delete", ID_CONTEXT_ITEM_DELETE,
			*__pDeleteBitmap, __pDeleteBitmap, __pDeleteBitmap);
	__pTrackListContextMenu->SetColor(Color(46, 151, 199));
	//__pTrackListContextMenu->AddActionEventListener(*this);

	__pTrackListView = new ListView();

	r = __pTrackListView->Construct(
			Rectangle(0, 0, GetBounds().width, GetBounds().height), true, true);
	if (r != E_SUCCESS) {
		AppLogException(
				"Error constructing track list view: [%s]", GetErrorMessage(r));
		return r;
	}
	r = __pTrackListView->SetItemProvider(*this);
	if (r != E_SUCCESS) {
		AppLogException(
				"Error setting track list item provider: [%s]", GetErrorMessage(r));
		return r;
	}
	__pTrackListView->AddListViewItemEventListener(*this);
	__pTrackListView->AddTouchEventListener(*this);
	AddControl(*__pTrackListView);

	//r = __pTrackListView->SetBackgroundBitmap(__pTrackListBackgroundBitmap);
	//if (r != E_SUCCESS) {
	//	AppLogException(
	//			"Error setting background bitmap for track list view: [%s]", GetErrorMessage(r));
	//	return r;
	//}
	return r;
}

void TrackListPanel::OnListViewContextItemStateChanged(
		Tizen::Ui::Controls::ListView& listView, int index, int elementId,
		Tizen::Ui::Controls::ListContextItemStatus state) {
}

void TrackListPanel::OnListViewItemStateChanged(
		Tizen::Ui::Controls::ListView& listView, int index, int elementId,
		Tizen::Ui::Controls::ListItemStatus status) {

	AppLog("int index: [%d], int elementId: [%d], status: [%d]", index, elementId, __pTrackListView->IsItemChecked(index));



	//__pTrackListView->IsItemChecked(index) ? __pTrackListView->SetItemChecked(index, false) : __pTrackListView->SetItemChecked(index, true);

	//__pTrackListView->RefreshList(index, ID_TRACK_LIST_ITEM);


	/*if (status == LIST_ITEM_STATUS_CHECKED) {
	 for (int i = 0; i < listView.GetItemCount(); i++)
	 if (i != index) {
	 result r = __pTrackListView->SetItemEnabled(i, false);
	 if (r != E_SUCCESS) {
	 AppLogException(
	 "Error setting item:[%d],[%s]", i, GetErrorMessage(r));
	 return;
	 }
	 }
	 } else if (status == LIST_ITEM_STATUS_UNCHECKED) {
	 for (int i = 0; i < listView.GetItemCount(); i++)
	 if (i != index) {
	 result r = __pTrackListView->SetItemEnabled(i, true);
	 if (r != E_SUCCESS) {
	 AppLogException(
	 "Error setting item:[%d],[%s]", i, GetErrorMessage(r));
	 return;
	 }
	 }
	 if (_trackingIndex != -1)
	 listView.SetItemChecked(index, false);
	 else
	 _trackingIndex = index;
	 } else if (status == LIST_ITEM_STATUS_UNCHECKED) {
	 if (_trackingIndex != -1)
	 _trackingIndex = -1;
	 }
	 __pTrackListView->UpdateList();*/
}

void TrackListPanel::OnListViewItemSwept(
		Tizen::Ui::Controls::ListView& listView, int index,
		Tizen::Ui::Controls::SweepDirection direction) {
}

void TrackListPanel::OnListViewItemLongPressed(
		Tizen::Ui::Controls::ListView& listView, int index, int elementId,
		bool& invokeListViewItemCallback) {

	__pTrackListContextMenu->SetAnchorPosition(lastClickedPosition);
	__pTrackListContextMenu->SetShowState(true);
	__pTrackListContextMenu->Show();
}

Tizen::Ui::Controls::ListItemBase* TrackListPanel::CreateItem(int index,
		float itemWidth) {

	CustomItem* pItem = new (std::nothrow) CustomItem();
	TrackListElement* pTrackElement = new TrackListElement();

	//TODO this might need to recalculate based on real device coors to make it relative, needs to be tested
	FloatRectangle trackElementRect(30.0f, 10.0f, GetBoundsF().width * 0.90f,
			90.0f);

	pItem->Construct(Dimension(itemWidth, 112), LIST_ANNEX_STYLE_NORMAL);
	pItem->AddElement(trackElementRect, ID_TRACK_LIST_ITEM, *pTrackElement);

	pItem->SetContextItem(__pTrackListContextItem);

	return pItem;
}

bool TrackListPanel::DeleteItem(int index,
		Tizen::Ui::Controls::ListItemBase* pItem, float itemWidth) {
	delete pItem;
	pItem = null;
	return true;
}

int TrackListPanel::GetItemCount(void) {
	return 10;
}

void TrackListPanel::OnTouchPressed(const Tizen::Ui::Control& source,
		const Tizen::Graphics::Point& currentPosition,
		const Tizen::Ui::TouchEventInfo& touchInfo) {
	if (&source == __pTrackListView) {
		AppLog(
				"Clicked on track list view, position: [%d] [%d]", currentPosition.x, currentPosition.y);
		lastClickedPosition = source.ConvertToScreenPosition(currentPosition);
	}
}

	/*result TrackListPanel::OnInitializing(void) {
	 result r=E_SUCCESS;


	 }*/

	result TrackListPanel::LoadResources(void) {
		result r = E_SUCCESS;

		AppResource* pAppRes = Application::GetInstance()->GetAppResource();

		__pMapBitmap = pAppRes->GetBitmapN(L"location.png");
		__pEditBitmap = pAppRes->GetBitmapN(L"edit.png");
		__pDeleteBitmap = pAppRes->GetBitmapN(L"delete.png");

		return r;
	}
