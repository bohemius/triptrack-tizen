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

class PoiIconListPanel : public Tizen::Ui::Controls::Panel, public Tizen::Ui::Controls::IIconListViewItemProvider
, public Tizen::Ui::Controls::IIconListViewItemEventListener {

public:
	PoiIconListPanel(Tizen::Graphics::Rectangle &rect);
	~PoiIconListPanel(void);
	result Construct(void);

// IIconListViewItemEventListener
	virtual void OnIconListViewItemStateChanged(
			Tizen::Ui::Controls::IconListView &view, int index,
			Tizen::Ui::Controls::IconListViewItemStatus status);

//IIconListViewItemProvider
	virtual Tizen::Ui::Controls::IconListViewItem* CreateItem(int index);
	virtual bool DeleteItem(int index,
			Tizen::Ui::Controls::IconListViewItem* pItem);
	virtual int GetItemCount(void);

private:
	result LoadResources(void);
	Tizen::Ui::Controls::IconListView* __pPoiIconListView;

	static const int ID_FORMAT_CUSTOM = 503;
};

#endif /* POICOMPONENTS_H_ */
