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
};

class PoiIconListPanel: public Tizen::Ui::Controls::Panel,
		public Tizen::Ui::Controls::IGroupedListViewItemEventListener,
		public Tizen::Ui::Controls::IGroupedListViewItemProvider {

public:
	PoiIconListPanel(Tizen::Graphics::Rectangle &rect);
	~PoiIconListPanel(void);
	result Construct(void);
	result Update(void);

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

private:
	Tizen::Base::String* BuildTileText(POI* pPoi);
	Tizen::Ui::Controls::GroupedListView* __pPoiGroupedListView;
	result LoadResources(void);
	Tizen::Graphics::Rectangle* __pContainerRectangle;
	Tizen::Graphics::FloatRectangle* __pBitmapRectangle;

	Tizen::Base::Collection::HashMapT<long long int,
			Tizen::Base::Collection::LinkedListT<POI*>*>* __pPoiMap;

	static const int ID_FORMAT_CUSTOM = 503;
};

#endif /* POICOMPONENTS_H_ */
