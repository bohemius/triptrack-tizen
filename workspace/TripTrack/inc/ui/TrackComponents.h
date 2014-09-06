/*
 * TrackComponents.h
 *
 *  Created on: Aug 14, 2013
 *      Author: bohemius
 */

#ifndef TRACKCOMPONENTS_H_
#define TRACKCOMPONENTS_H_

#include <FApp.h>
#include <FUi.h>
#include <FGraphics.h>
#include <FBase.h>
#include <FLocations.h>
#include "geo/Tracker.h"
#include "geo/TrackerManager.h"

class TrackListElement: public Tizen::Ui::Controls::ICustomElementF {
public:
	TrackListElement(Tracker* pTracker);
	TrackListElement(void);
	~TrackListElement(void);
	bool OnDraw(Tizen::Graphics::Canvas& canvas,
			const Tizen::Graphics::FloatRectangle& rect,
			Tizen::Ui::Controls::ListItemDrawingStatus itemStatus);

private:
	Tizen::Base::String FormatTitle(void);
	Tizen::Base::String FormatDistance(void);
	Tizen::Base::String FormatDuration(void);

	Tracker* __pTracker;
	static const float TEXT_MARGIN_X = 10.0f;
	static const float TEXT_MARGIN_Y = 10.0f;
};

class TrackListPanel: public Tizen::Ui::Controls::Panel,
		public Tizen::Ui::Controls::IListViewItemEventListener,
		public Tizen::Ui::Controls::IListViewItemProviderF,
		public Tizen::Ui::ITouchEventListener,
		public Tizen::Ui::IActionEventListener,
		public IOnDataChangedListener {

public:
	TrackListPanel(Tizen::Graphics::Rectangle &rect);
	~TrackListPanel(void);
	result Construct(void);

	// IActionEventListener
	virtual void OnActionPerformed(const Tizen::Ui::Control &source,
			int actionId);

	// ITouchEventListener
	virtual void OnTouchPressed(const Tizen::Ui::Control &source,
			const Tizen::Graphics::Point &currentPosition,
			const Tizen::Ui::TouchEventInfo &touchInfo);
	virtual void OnTouchFocusIn(const Tizen::Ui::Control &source,
			const Tizen::Graphics::Point &currentPosition,
			const Tizen::Ui::TouchEventInfo &touchInfo) {
	}
	;
	virtual void OnTouchFocusOut(const Tizen::Ui::Control &source,
			const Tizen::Graphics::Point &currentPosition,
			const Tizen::Ui::TouchEventInfo &touchInfo) {
	}
	;
	virtual void OnTouchMoved(const Tizen::Ui::Control &source,
			const Tizen::Graphics::Point &currentPosition,
			const Tizen::Ui::TouchEventInfo &touchInfo) {
	}
	;
	virtual void OnTouchReleased(const Tizen::Ui::Control &source,
			const Tizen::Graphics::Point &currentPosition,
			const Tizen::Ui::TouchEventInfo &touchInfo) {
	}
	;

	// IListViewItemEventListener
	virtual void OnListViewContextItemStateChanged(
			Tizen::Ui::Controls::ListView &listView, int index, int elementId,
			Tizen::Ui::Controls::ListContextItemStatus state);
	virtual void OnListViewItemStateChanged(
			Tizen::Ui::Controls::ListView &listView, int index, int elementId,
			Tizen::Ui::Controls::ListItemStatus status);
	virtual void OnListViewItemSwept(Tizen::Ui::Controls::ListView &listView,
			int index, Tizen::Ui::Controls::SweepDirection direction);
	virtual void OnListViewItemLongPressed(
			Tizen::Ui::Controls::ListView &listView, int index, int elementId,
			bool& invokeListViewItemCallback);

	//IListViewItemProvider
	virtual Tizen::Ui::Controls::ListItemBase* CreateItem(int index,
			float itemWidth);
	virtual bool DeleteItem(int index, Tizen::Ui::Controls::ListItemBase *pItem,
			float itemWidth);
	virtual int GetItemCount(void);

	//IOnTrackChangeListener
	virtual result Update(void);

private:
	result LoadResources(void);
	void DisplayMap(Tracker* tracker);
	void DeleteTracker(Tracker* tracker);
	Tracker* GetTrackerFromClick();

	Tizen::Ui::Controls::ListView* __pTrackListView;
	Tizen::Graphics::Bitmap* __pTrackListBackgroundBitmap; //TODO: looks like it is not getting used
	Tizen::Ui::Controls::ContextMenu* __pTrackListContextMenu;
	Tizen::Graphics::Bitmap *__pMapBitmap, *__pEditBitmap, *__pDeleteBitmap;
	Tizen::Locations::LocationProvider *__pLocProvider;
	int _trackingIndex;
	Tizen::Graphics::Point lastClickedPosition;
	TrackerManager* __pTrackerMgr;

	static const int ID_TRACK_LIST_ITEM = 502;
	static const int ID_FORMAT_CUSTOM = 503;
	static const int ID_CONTEXT_ITEM_MAP = 504;
	static const int ID_CONTEXT_ITEM_EDIT = 505;
	static const int ID_CONTEXT_ITEM_DELETE = 506;

	//temp vars TODO, values from the tracker collection will be used
};

#endif /* TRACKCOMPONENTS_H_ */
