/*
 * PoiComponents.h
 *
 *  Created on: Aug 14, 2013
 *      Author: bohemius
 */

#ifndef POICOMPONENTS_H_
#define POICOMPONENTS_H_

#include <FApp.h>
#include <FUi.h>
#include <FGraphics.h>
#include <FBase.h>
#include "dao/TTMedia.h"
#include "dao/POI.h"

class PoiListElement: public Tizen::Ui::Controls::ICustomElementF {
public:
	PoiListElement(POI* pPoi);
	~PoiListElement(void);
	bool OnDraw(Tizen::Graphics::Canvas& canvas,
			const Tizen::Graphics::FloatRectangle& rect,
			Tizen::Ui::Controls::ListItemDrawingStatus itemStatus);

	static const float BITMAP_PADDING_X = 50;
	static const float BITMAP_PADDING_Y = 20;

private:
	POI* __pPoi;

	Tizen::Graphics::EnrichedText* CreateTextOverLay(
			Tizen::Graphics::FloatDimension overlayDim);
};

class PoiIconListPanel: public Tizen::Ui::Controls::Panel,
		public Tizen::Ui::Controls::IGroupedListViewItemEventListener,
		public Tizen::Ui::Controls::IGroupedListViewItemProvider,
		public IOnDataChangedListener,
		public Tizen::Ui::ITouchEventListener,
		public Tizen::Ui::IActionEventListener {

public:
	PoiIconListPanel(Tizen::Graphics::Rectangle &rect);
	~PoiIconListPanel(void);
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

	// IGroupedListViewItemEventListener
	virtual void OnGroupedListViewContextItemStateChanged(
			Tizen::Ui::Controls::GroupedListView &listView, int groupIndex,
			int itemIndex, int elementId,
			Tizen::Ui::Controls::ListContextItemStatus state);
	virtual void OnGroupedListViewItemStateChanged(
			Tizen::Ui::Controls::GroupedListView &listView, int groupIndex,
			int itemIndex, int elementId,
			Tizen::Ui::Controls::ListItemStatus state);
	virtual void OnGroupedListViewItemSwept(
			Tizen::Ui::Controls::GroupedListView &listView, int groupIndex,
			int itemIndex, Tizen::Ui::Controls::SweepDirection direction);
	virtual void OnGroupedListViewItemLongPressed(
			Tizen::Ui::Controls::GroupedListView &listView, int groupIndex,
			int itemIndex, int elementId, bool &invokeListViewItemCallback);

	// IGroupedListViewItemProvider
	virtual int GetGroupCount(void);
	virtual int GetItemCount(int groupIndex);
	virtual Tizen::Ui::Controls::ListItemBase* CreateItem(int groupIndex,
			int itemIndex, int itemWidth);
	virtual Tizen::Ui::Controls::GroupItem* CreateGroupItem(int groupIndex,
			int itemWidth);
	virtual bool DeleteItem(int groupIndex, int itemIndex,
			Tizen::Ui::Controls::ListItemBase* pItem, int itemWidth);
	virtual bool DeleteGroupItem(int groupIndex,
			Tizen::Ui::Controls::GroupItem* pItem, int itemWidth);

	// IOnDataChangedListener
	virtual result Update(void);

private:
	void DeletePoi(POI* pPoi);
	POI* GetPoiFromClick(void);
	Tizen::Base::String* BuildTileText(POI* pPoi);

	Tizen::Ui::Controls::GroupedListView* __pPoiGroupedListView;
	result LoadResources(void);
	Tizen::Graphics::Rectangle* __pContainerRectangle;
	Tizen::Graphics::FloatRectangle* __pBitmapRectangle;
	Tizen::Ui::Controls::ContextMenu* __pPoiListContextMenu;
	Tizen::Graphics::Point lastClickedPosition;
	Tizen::Graphics::Bitmap* __pDeleteBitmap;

	Tizen::Base::Collection::HashMapT<long long int,
			Tizen::Base::Collection::LinkedListT<POI*>*>* __pPoiMap;

	static const int ID_FORMAT_CUSTOM = 503;
	static const int ID_CONTEXT_ITEM_DELETE = 602;
};

#endif /* POICOMPONENTS_H_ */
