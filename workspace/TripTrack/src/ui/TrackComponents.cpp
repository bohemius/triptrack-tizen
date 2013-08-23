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

TrackListElement::TrackListElement() /*:
 __pTracker(tracker) */{

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

	/*
	 * if (canvas.DrawRectangle(rect) != E_SUCCESS) {
	 return false;
	 }*/

	//draw track title
	if (canvas.DrawText(Tizen::Graphics::FloatPoint(rect.x, rect.y+TEXT_MARGIN_Y),
			L"Track title") != E_SUCCESS) {
		return false;
	}

	//draw mileage
	mileageFont.Construct(FONT_STYLE_PLAIN, 20.0f);
	canvas.SetFont(mileageFont);
	if (canvas.DrawText(
			Tizen::Graphics::FloatPoint(rect.x,
					rect.y + rect.height - mileageFont.GetMaxHeightF()),
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
			Tizen::Graphics::FloatPoint(rect.x + rect.width - 200, rect.y+TEXT_MARGIN_Y),
			L"Duration 123.3 Hours") != E_SUCCESS) {
		return false;
	}
	return true;
}

TrackListPanel::TrackListPanel(Rectangle &rect) :
		Panel(), __pTrackListView(null), __pTrackListBackgroundBitmap(null) {
	Panel::Construct(Rectangle(0, 0, rect.width, rect.height));
	//Panel::SetBackgroundColor(Color::GetColor(COLOR_ID_GREY));
}

TrackListPanel::~TrackListPanel(void) {
}

result TrackListPanel::Construct(void) {
	result r = E_SUCCESS;

	r = LoadResources();
	if (r != E_SUCCESS) {
		AppLogException(
				"Error loading resources for track list view: [%s]", GetErrorMessage(r));
		return r;
	}
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
}

void TrackListPanel::OnListViewItemSwept(
		Tizen::Ui::Controls::ListView& listView, int index,
		Tizen::Ui::Controls::SweepDirection direction) {
}

void TrackListPanel::OnListViewItemLongPressed(
		Tizen::Ui::Controls::ListView& listView, int index, int elementId,
		bool& invokeListViewItemCallback) {
}

Tizen::Ui::Controls::ListItemBase* TrackListPanel::CreateItem(int index,
		float itemWidth) {

	CustomItem* pItem = new (std::nothrow) CustomItem();
	TrackListElement* pTrackItem = new TrackListElement();
	FloatRectangle customElementRect(30.0f, 10.0f, 500.0f, 90.0f);

	pItem->Construct(Dimension(itemWidth, 112), LIST_ANNEX_STYLE_ONOFF_SLIDING);
	//pItem->AddElement(Rectangle(80, 25, 150, 50), 10, L"Test", true);
	pItem->AddElement(customElementRect, ID_FORMAT_CUSTOM, *pTrackItem);

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

result TrackListPanel::LoadResources(void) {
	result r = E_SUCCESS;

	AppResource* pAppRes = Application::GetInstance()->GetAppResource();

	return r;
}
