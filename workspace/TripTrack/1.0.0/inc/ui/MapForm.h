/*
 * MapForm.h
 *
 *  Created on: Aug 7, 2014
 *      Author: bohemius
 */

#ifndef MAPFORM_H_
#define MAPFORM_H_

#include <FUi.h>
#include <FBase.h>
#include <FApp.h>
#include <HMaps.h>
#include "geo/Tracker.h"
#include "geo/TrackerManager.h"
#include "dao/POI.h"

class PoiMarker {
public:
	PoiMarker() :
			__pMarker(null), __pPoi(null) {
	}
	PoiMarker(POI* pPoi, HMaps::Marker* pMarker) :
			__pMarker(pMarker), __pPoi(pPoi) {
	}

	HMaps::MapObject* GetMarker(void) {
		return __pMarker;
	}

	POI* GetPoi(void) {
		return __pPoi;
	}
private:
	HMaps::MapObject* __pMarker;
	POI* __pPoi;
};

class MapForm: public Tizen::Ui::Controls::Form,
		public HMaps::IMapEventListener,
		public Tizen::Ui::Controls::IFormBackEventListener,
		public Tizen::Ui::Scenes::ISceneEventListener,
		public HMaps::IMapObjectEventListener,
		public IOnDataChangedListener {
public:
	MapForm();
	virtual ~MapForm();

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

// IOnTrackChangeListener
	virtual result Update(void);

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
	result AddTrack(void);
	result AddPois(void);
	result InitializeInfoWindow(void);
	void HideInfoWindow();
	void ShowInfoWindow();
	Tizen::Base::String CreatePoiInfo(POI* pPoi);
	POI* GetPoiFromClick(HMaps::MapObject& mapObject, Tizen::Locations::Coordinates& coord);

	HMaps::Map* __pMap;
	Tracker* __pTracker;
	TrackerManager* __pTrackerMngr;
	HMaps::Polyline* __pPolyline;
	Tizen::Ui::Controls::Label* __pInfoWindow;
	Tizen::Base::Collection::LinkedListT<PoiMarker*>* __pMarkerList;
	Tizen::Graphics::Bitmap *__pStartMarkerBitmap, *__pEndMarkerBitmap,
			*__pPoiMarkerBitmap;
	HMaps::Marker* __pSelectedMarker;
};

#endif /* MAPFORM_H_ */
