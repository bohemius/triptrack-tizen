/*
 * MapForm.cpp
 *
 *  Created on: Aug 7, 2014
 *      Author: bohemius
 */

#include "dao/POI.h"
#include "dao/StorageManager.h"
#include "ui/MapForm.h"
#include "SceneRegister.h"
#include "geo/GeoHelper.h"
#include <FApp.h>

using namespace Tizen::Ui::Controls;
using namespace Tizen::Ui::Scenes;
using namespace Tizen::Graphics;
using namespace Tizen::Base;
using namespace Tizen::Base::Collection;
using namespace Tizen::Locations;
using namespace Tizen::App;
using namespace HMaps;

MapForm::MapForm() :
		__pMap(null), __pTracker(null), __pTrackerMngr(null), __pPolyline(null), __pMarkerList(
				null) {
}

MapForm::~MapForm() {
}

result MapForm::OnInitializing(void) {
	result r = E_SUCCESS;

	AppResource* pAppResource = Application::GetInstance()->GetAppResource();
	__pStartMarkerBitmap = pAppResource->GetBitmapN(L"home.png");
	__pEndMarkerBitmap = pAppResource->GetBitmapN(L"stop.png");
	__pPoiMarkerBitmap = pAppResource->GetBitmapN(L"photo.png");

	if (__pMap == null) {
		__pMap = new (std::nothrow) Map();
		r = __pMap->Construct(GetClientAreaBounds().width,
				GetClientAreaBounds().height);
		if (r != E_SUCCESS) {
			AppLogException(
					"Error constructing map control: [%s]", GetErrorMessage(r));
			return r;
		}
		AddControl(__pMap);
	}

	SetFormBackEventListener(this);

	return r;
}

result MapForm::OnTerminating(void) {
}

void MapForm::OnFormBackRequested(Tizen::Ui::Controls::Form& source) {
	SceneManager* pSceneMngr = SceneManager::GetInstance();
	pSceneMngr->GoBackward(BackwardSceneTransition(SCENE_MAIN_FORM));
}

void MapForm::OnSceneActivatedN(
		const Tizen::Ui::Scenes::SceneId& previousSceneId,
		const Tizen::Ui::Scenes::SceneId& currentSceneId,
		Tizen::Base::Collection::IList* pArgs) {
	result r = E_SUCCESS;
	AppLog("Activated MAP_FORM scene");

	Object* param = pArgs->GetAt(0);
	__pTracker = static_cast<Tracker*>(param);
	AppLog(
			"MapForm got tracker [%ls] with id [%ld]", __pTracker->GetTitle()->GetPointer(), __pTracker->GetTrackerId());

	r = AddTrack();
	if (r != E_SUCCESS)
		AppLogException("Error adding track to map", GetErrorMessage(r));

	r = AddPois();
	if (r != E_SUCCESS)
		AppLogException("Error adding poi markers to map", GetErrorMessage(r));
}

void MapForm::OnMapLongPressed(Map& map,
		const Tizen::Locations::Coordinates& coordinate) {
}

void MapForm::OnMapRegionChanged(Map& map) {
}

void MapForm::OnMapTapped(Map& map,
		const Tizen::Locations::Coordinates& coordinate) {
}

void MapForm::OnSceneDeactivated(
		const Tizen::Ui::Scenes::SceneId& currentSceneId,
		const Tizen::Ui::Scenes::SceneId& nextSceneId) {
	AppLog("Deactivate MAP_FORM scene");
}

result MapForm::Update(void) {
}

result MapForm::AddTrack(void) {
	result r = E_SUCCESS;

	if (__pPolyline != null) {
		__pMap->RemoveMapObject(__pPolyline);
		delete __pPolyline;
		__pPolyline = null;
	}

	__pPolyline = new (std::nothrow) Polyline();

	IEnumeratorT<TTLocation*>* pEnum =
			__pTracker->GetTrackPoints()->GetEnumeratorN();

	TTLocation* pLocation;

	while (pEnum->MoveNext() == E_SUCCESS) {
		r = pEnum->GetCurrent(pLocation);
		if (r != E_SUCCESS)
			AppLogException(
					"Error getting TTLocation from collection for tracker_ID [%d]: ", __pTracker->GetTrackerId(), GetErrorMessage(r));
		else {
			r = __pPolyline->AppendToPath(
					Coordinates(*(pLocation->getCoordinates())));

			if (r != E_SUCCESS)
				AppLogException(
						"Error adding coordinates to polyline", GetErrorMessage(r));
		}
	}
	AppLog(
			"Created polyline with [%d] way points.", __pPolyline->GetPathN()->GetCount());

	__pPolyline->SetStrokeColor(Color(46, 151, 199));
	__pPolyline->SetStrokeThickness(10);
	__pPolyline->SetVisible(true);

	r = __pMap->AddMapObject(__pPolyline);
	if (r != E_SUCCESS)
		AppLogException(
				"Error adding path polyline to map", GetErrorMessage(r));

	GeoBoundingBox box = __pPolyline->GetBoundingBox();
	if (box.IsValid()) {
		r = __pMap->SetDisplayRegion(box);
		if (r != E_SUCCESS)
			AppLogException(
					"Error setting display region", GetErrorMessage(r));
	}
}

result MapForm::AddPois(void) {
	result r = E_SUCCESS;

	LinkedListT<POI*>* pPoiList = StorageManager::getInstance()->GetPois(
			__pTracker);

	//create a new list of markers from the POI collection
	if (__pMarkerList != null && __pMarkerList->GetCount() > 0) {
		IEnumeratorT<Marker*>* pEnum = __pMarkerList->GetEnumeratorN();

		while (pEnum->MoveNext() == E_SUCCESS) {
			Marker* pMarker;
			pEnum->GetCurrent(pMarker);
			__pMap->RemoveMapObject(pMarker);
		}
		__pMarkerList->RemoveAll();
	}

	__pMarkerList = new LinkedListT<Marker*>();

	Marker* pStartMarker = new (std::nothrow) Marker();

	pStartMarker->SetCoordinates(
			*(__pTracker->StartPosition()->getCoordinates()));
	pStartMarker->SetBitmap(*__pStartMarkerBitmap);
	__pMarkerList->Add(pStartMarker);
	__pMap->AddMapObject(pStartMarker);

	IEnumeratorT<POI*>* pPoiEnum = pPoiList->GetEnumeratorN();
	while (pPoiEnum->MoveNext() == E_SUCCESS) {
		POI* pPoi;
		pPoiEnum->GetCurrent(pPoi);
		Coordinates coor = *(pPoi->GetLocation());
		if (!(coor.GetAltitude() == 0.0 && coor.GetLatitude() == 0.0
				&& coor.GetLongitude() == 0.0)) {
			Marker* pMarker = new (std::nothrow) Marker();
			pMarker->SetCoordinates(coor);
			pMarker->SetBitmap(*__pPoiMarkerBitmap);
			__pMarkerList->Add(pMarker);
			__pMap->AddMapObject(pMarker);
		}

	}

	Marker* pEndMarker = new (std::nothrow) Marker();

	pEndMarker->SetCoordinates(*(__pTracker->EndPosition()->getCoordinates()));
	pEndMarker->SetBitmap(*__pEndMarkerBitmap);
	__pMarkerList->Add(pEndMarker);
	__pMap->AddMapObject(pEndMarker);

	return r;
}

