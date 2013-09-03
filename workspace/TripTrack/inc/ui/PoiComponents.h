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

class PoiIconListPanel: public Tizen::Ui::Controls::Panel,
		public Tizen::Ui::Controls::IIconListViewItemProvider,
		public Tizen::Ui::Controls::IIconListViewItemEventListener {

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

	result UpdatePoiCollection(void);

private:
	result LoadResources(void);
	Tizen::Ui::Controls::IconListView* __pPoiIconListView;
	Tizen::Base::Collection::LinkedListT<POI*>* __pPoiCollection;

	float tile_width, tile_height;

	static const float TILES_PER_ROW = 2.0f;
	static const float TILES_PER_COLUMN = 2.0f;
	static const float TILES_SPACING_X = 20.0f;
	static const float TILES_SPACING_Y = 30.0f;
	static const float TILES_OFFSET_X = 0.0f;
	static const float TILES_OFFSET_Y = 30.0f;

	static const int ID_FORMAT_CUSTOM = 503;

	Tizen::Graphics::Bitmap* __pTile1;
	Tizen::Graphics::Bitmap* __pTile2;
	Tizen::Graphics::Bitmap* __pTile3;
	Tizen::Graphics::Bitmap* __pTile4;
	Tizen::Graphics::Bitmap* __pTile5;
};

#endif /* POICOMPONENTS_H_ */
