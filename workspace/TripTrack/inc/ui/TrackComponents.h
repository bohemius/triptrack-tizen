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
#include "geo/Tracker.h"

class TrackListElement: public Tizen::Ui::Controls::ICustomElementF {
public:
	TrackListElement(Tracker* tracker);
	~TrackListElement(void);
	bool OnDraw(Tizen::Graphics::Canvas& canvas,
			const Tizen::Graphics::FloatRectangle& rect,
			Tizen::Ui::Controls::ListItemDrawingStatus itemStatus);

private:
	Tracker* __pTracker;
	static const int TEXT_MARGIN_X = 10.0f;
	static const int TEXT_MARGIN_Y = 15.0f;
};

class TrackListPanel: public Tizen::Ui::Controls::Panel,
		public Tizen::Ui::Controls::IListViewItemEventListener,
		public Tizen::Ui::Controls::IListViewItemProviderF {

public:
	TrackListPanel(Tizen::Graphics::Rectangle &rect);
	~TrackListPanel(void);
	result Construct(void);

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

private:
	result LoadResources(void);
	Tizen::Ui::Controls::ListView* __pTrackListView;
	Tizen::Graphics::Bitmap* __pTrackListBackgroundBitmap;
};

#endif /* TRACKCOMPONENTS_H_ */
