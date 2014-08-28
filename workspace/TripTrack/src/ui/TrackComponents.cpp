/*
 * TrackComponents.cpp
 *
 *  Created on: Aug 14, 2013
 *      Author: bohemius
 */

#include "ui/TrackComponents.h"
#include "AppResourceId.h"
#include "SceneRegister.h"

using namespace Tizen::Graphics;
using namespace Tizen::Ui::Controls;
using namespace Tizen::Ui::Scenes;
using namespace Tizen::App;
using namespace Tizen::Base;
using namespace Tizen::Base::Collection;
using namespace Tizen::Locations;

TrackListElement::TrackListElement() {
}

TrackListElement::TrackListElement(Tracker* pTracker) :
		__pTracker(pTracker) {
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
	//AppLog("Item drawing status: [%d]", itemStatus);
	//if (itemStatus==LIST_ITEM_DRAWING_STATUS_PRESSED)
	//	TrackListElement::STATE == ON ? TrackListElement::STATE == OFF: TrackListElement::STATE == ON;

	//draw track title
	if (canvas.DrawText(
			Tizen::Graphics::FloatPoint(rect.x, rect.y + TEXT_MARGIN_Y),
			*(__pTracker->GetTitle())) != E_SUCCESS) {
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
	if (canvas.FillEllipse(
			__pTracker->GetStatus() == __pTracker->ACTIVE ?
					Color::GetColor(COLOR_ID_GREEN) :
					Color::GetColor(COLOR_ID_RED), diskRect) != E_SUCCESS) {
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

	//Construct location provider
	LocationCriteria criteria;

	criteria.SetAccuracy(LOC_ACCURACY_FINEST);
	__pLocProvider = new (std::nothrow) LocationProvider();
	r = __pLocProvider->Construct(criteria, *this);
	if (r != E_SUCCESS) {
		AppLogException(
				"Error constructing location provider: [%s]", GetErrorMessage(r));
		return r;
	}
	TrackerManager::getInstance()->SetLocationProvider(__pLocProvider);

	//Initialise tracker manager
	__pTrackerMgr = TrackerManager::getInstance();
	r = __pTrackerMgr->Construct();
	if (r != E_SUCCESS) {
		AppLogException(
				"Error error constructing a tracker manager: [%s]", GetErrorMessage(r));
		return r;
	}

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
	__pTrackListContextMenu->AddActionEventListener(*this);

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

	result r = E_SUCCESS;

	AppLog(
			"int index: [%d], int elementId: [%d], status: [%d]", index, elementId, __pTrackListView->IsItemChecked(index));

	Tracker* pTracker;
	r = __pTrackerMgr->GetTracks()->GetAt(index, pTracker);
	if (r != E_SUCCESS) {
		AppLogException(
				"Error getting tracker from tracker manager: [%s]", GetErrorMessage(r));
		return;
	}

	Tracker* currentTracker =
			TrackerManager::getInstance()->GetCurrentTracker();

	LocationProvider* pLocProvider =
			TrackerManager::getInstance()->GetLocationProvider();

	if (currentTracker == 0 || currentTracker == null) {
		pTracker->SetStatus(Tracker::ACTIVE);
		pLocProvider->StartLocationUpdatesByInterval(10);
		TrackerManager::getInstance()->SetCurrentTracker(pTracker);
	} else {
		if (pTracker == currentTracker) {
			pTracker->SetStatus(Tracker::PAUSED);
			pLocProvider->StopLocationUpdates();
			TrackerManager::getInstance()->SetCurrentTracker(null);
		} else {
			//TODO localize this
			String text(
					L"Track "
							+ *(TrackerManager::getInstance()->GetCurrentTracker()->GetTitle())
							+ " is currently active.  Would you like to stop it now and continue with track "
							+ *(pTracker->GetTitle()) + "?");
			MessageBox msgBox;
			msgBox.Construct("Warning", text, MSGBOX_STYLE_YESNO);
			msgBox.SetColor(Color(46, 151, 199));
			msgBox.SetTextColor(Color::GetColor(COLOR_ID_WHITE));

			int result = 0;
			msgBox.ShowAndWait(result);

			if (result == MSGBOX_RESULT_YES) {
				currentTracker->SetStatus(Tracker::PAUSED);
				pTracker->SetStatus(Tracker::ACTIVE);
				TrackerManager::getInstance()->SetCurrentTracker(pTracker);
			}
		}

		Update();
	}
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
	result r = E_SUCCESS;

	Tracker* pTracker;

	//get tracker from the tracker manager and read the track from the database
	r = __pTrackerMgr->GetTracks()->GetAt(index, pTracker);
	if (r != E_SUCCESS) {
		AppLogException(
				"Error getting tracker from tracker manager: [%s]", GetErrorMessage(r));
		return null;
	}
	CustomItem* pItem = new (std::nothrow) CustomItem();

	TrackListElement* pTrackElement = new TrackListElement(pTracker);

	//TODO this might need to recalculate based on real device coors to make it relative, needs to be tested
	FloatRectangle trackElementRect(30.0f, 10.0f, GetBoundsF().width * 0.90f,
			90.0f);

	pItem->Construct(Dimension(itemWidth, 112), LIST_ANNEX_STYLE_NORMAL);
	pItem->AddElement(trackElementRect, ID_TRACK_LIST_ITEM, *pTrackElement);

	return pItem;
}

bool TrackListPanel::DeleteItem(int index,
		Tizen::Ui::Controls::ListItemBase* pItem, float itemWidth) {
	//TODO implement track deletion on the UI
	delete pItem;
	pItem = null;
	return true;
}

int TrackListPanel::GetItemCount(void) {
	return __pTrackerMgr->GetTracks()->GetCount();

}

void TrackListPanel::OnTouchPressed(const Tizen::Ui::Control& source,
		const Tizen::Graphics::Point& currentPosition,
		const Tizen::Ui::TouchEventInfo& touchInfo) {
	if (&source == __pTrackListView) {
		lastClickedPosition = source.ConvertToScreenPosition(currentPosition);
		AppLog(
				"Clicked on track list view, position: [%d] [%d], on list view item with id: [%d]", currentPosition.x, currentPosition.y, __pTrackListView->GetItemIndexFromPosition(source.ConvertToControlPosition(lastClickedPosition)));
	}
}

void TrackListPanel::OnActionPerformed(const Tizen::Ui::Control& source,
		int actionId) {
	switch (actionId) {
	case ID_CONTEXT_ITEM_MAP: {
		AppLog("Setting ID_CONTEXT_ITEM_MAP view");

		//get the selected tracker
		Tracker* pTracker = GetTrackerFromClick();
		DisplayMap(pTracker);
	}
		break;
	case ID_CONTEXT_ITEM_DELETE: {
		//TODO check and display warning and stop tracking before deleting
		Tracker* pTracker = GetTrackerFromClick();
		DeleteTracker(pTracker);
		Update();
	}
		break;
	case ID_CONTEXT_ITEM_EDIT: {
		result r = E_SUCCESS;

		Tracker* pTracker = GetTrackerFromClick();
		EditFormPopup* pEditPopup = new EditFormPopup();

		Rectangle bounds = GetBounds();
		r = pEditPopup->Construct(pTracker, this,
					Dimension((int) bounds.width * 0.90, (int) bounds.height * 0.90),
					I18N::GetLocalizedString(ID_STRING_EDIT_TRACK_POPUP_TITLE));
			if (r != E_SUCCESS)
				AppLogException(
						"Error constructing edit form popup: [%s]", GetErrorMessage(r));

		pEditPopup->Show();
	}
		break;
	}
}

result TrackListPanel::Update(void) {
	result r = E_SUCCESS;

	r = __pTrackListView->UpdateList();
	if (r != E_SUCCESS)
		AppLogException(
				"Error updating track panel: [%s]", GetErrorMessage(r));
	return r;
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

void TrackListPanel::DisplayMap(Tracker* tracker) {

	LinkedList* parameterList = new LinkedList();

	parameterList->Add(tracker);
	SceneManager* pSceneMngr = SceneManager::GetInstance();
	pSceneMngr->GoForward(ForwardSceneTransition(SCENE_MAP_FORM),
			parameterList);
}

void TrackListPanel::DeleteTracker(Tracker* tracker) {
	result r = E_SUCCESS;

	r = __pTrackerMgr->RemoveTracker(tracker);
	if (r != E_SUCCESS)
		AppLogException(
				"Error removing tracker [%ls] with id [%d]: [%s]", tracker->GetTitle()->GetPointer(), tracker->GetTrackerId(), GetErrorMessage(r));
}

void TrackListPanel::OnLocationUpdated(
		const Tizen::Locations::Location& location) {
}

void TrackListPanel::OnLocationUpdateStatusChanged(
		Tizen::Locations::LocationServiceStatus status) {
}

void TrackListPanel::OnRegionEntered(Tizen::Locations::RegionId regionId) {
}

void TrackListPanel::OnRegionLeft(Tizen::Locations::RegionId regionId) {
}

void TrackListPanel::OnRegionMonitoringStatusChanged(
		Tizen::Locations::LocationServiceStatus status) {
}

void TrackListPanel::OnAccuracyChanged(
		Tizen::Locations::LocationAccuracy accuracy) {
}

Tracker* TrackListPanel::GetTrackerFromClick() {
	Tracker* pTracker;

	result r = E_SUCCESS;

	int i = __pTrackListView->GetItemIndexFromPosition(
			__pTrackListView->ConvertToControlPosition(lastClickedPosition));
	AppLog("Id of selected list item: [%d]", i);

	r = TrackerManager::getInstance()->GetTracks()->GetAt(
			__pTrackListView->GetItemIndexFromPosition(
					__pTrackListView->ConvertToControlPosition(
							lastClickedPosition)), pTracker);
	if (r != E_SUCCESS) {
		AppLogException(
				"Error getting tracker from position: [%s]", GetErrorMessage(r));
		return null;
	}
	return pTracker;
}

