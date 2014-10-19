/*
 * PoiMapForm.h
 *
 *  Created on: Sep 30, 2014
 *      Author: bohemius
 */

#ifndef POIMAPFORM_H_
#define POIMAPFORM_H_

#include <FUi.h>
#include <HMaps.h>
#include "dao/POI.h"
#include "ui/MapForm.h"

class PoiThumbnail: public Tizen::Ui::Controls::Panel {
public:
	PoiThumbnail();
	virtual ~PoiThumbnail();

	// override the neccessary base methods
	result OnDraw(void);
	result Construct(const Tizen::Graphics::Rectangle& rect);
	void SetBitmap(Tizen::Graphics::Bitmap* pBitmap);

private:
	Tizen::Graphics::Bitmap* __pThumbnail;
};

class PoiMapForm: public Tizen::Ui::Controls::Form,
		public HMaps::IMapEventListener,
		public HMaps::IMapObjectEventListener,
		public Tizen::Ui::Controls::IFormBackEventListener,
		public Tizen::Ui::Scenes::ISceneEventListener {
public:
	PoiMapForm();
	virtual ~PoiMapForm();

	// IMapEventListener
	virtual void OnMapLongPressed(HMaps::Map &map,
			const Tizen::Locations::Coordinates &coordinate);
	virtual void OnMapRegionChanged(HMaps::Map &map);
	virtual void OnMapTapped(HMaps::Map &map,
			const Tizen::Locations::Coordinates &coordinate);

	// override from Form
	virtual result OnInitializing(void);
	virtual result OnTerminating(void);

	// IFormBackEventListener
	virtual void OnFormBackRequested(Tizen::Ui::Controls::Form &source);

	// ISceneEventListener
	virtual void OnSceneActivatedN(
			const Tizen::Ui::Scenes::SceneId &previousSceneId,
			const Tizen::Ui::Scenes::SceneId &currentSceneId,
			Tizen::Base::Collection::IList *pArgs);
	virtual void OnSceneDeactivated(
			const Tizen::Ui::Scenes::SceneId &currentSceneId,
			const Tizen::Ui::Scenes::SceneId &nextSceneId);

	// IMapObjectEventListener
	virtual void OnMapObjectTapped(HMaps::Map& map, HMaps::MapObject& mapObject,
			const Tizen::Locations::Coordinates& coord);
	virtual void OnMapObjectDoubleTapped(HMaps::Map& map,
			HMaps::MapObject& mapObject,
			const Tizen::Locations::Coordinates& coord);
	virtual void OnMapObjectLongPressed(HMaps::Map& map,
			HMaps::MapObject& mapObject,
			const Tizen::Locations::Coordinates& coord);

private:
	result AddPois(void);
	result InitializeThumbnail(void);
	void HideThumbnail();
	void ShowThumbnail();
	Tizen::Base::String CreatePoiInfo(POI* pPoi);
	POI* GetPoiFromClick(HMaps::MapObject& mapObject,
			Tizen::Locations::Coordinates& coord);

	HMaps::Map* __pMap;

	HMaps::Polyline* __pPolyline;
	PoiThumbnail* __pPoiThumbnail;
	Tizen::Base::Collection::LinkedListT<PoiMarker*>* __pMarkerList;
	Tizen::Graphics::Bitmap *__pPoiMarkerBitmap;
	HMaps::Marker* __pSelectedMarker;
};

#endif /* POIMAPFORM_H_ */
