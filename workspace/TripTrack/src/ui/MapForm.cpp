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

static const int W_WINDOW_INFO_LABEL = 400;
static const int H_WINDOW_INFO_LABEL = 200;
static const int X_WINDOW_INFO_LABEL = 200;
static const int Y_WINDOW_INFO_LABEL = 210;

MapForm::MapForm() :
		__pMap(null), __pTracker(null), __pTrackerMngr(null), __pPolyline(null), __pMarkerList(
				null), __pInfoWindow(null), __pSelectedMarker(null) {
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

	if (__pInfoWindow == null)
		InitializeInfoWindow();

	__pMap->AddMapEventListener(*this);
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

	if (previousSceneId == SCENE_GALLERY_FORM
			|| previousSceneId == SCENE_POI_FORM) {
		__pMap->Draw();
		return;
	}

	Object* param = pArgs->GetAt(0);
	__pTracker = static_cast<Tracker*>(param);
	AppLog(
			"MapForm got tracker [%ls] with id [%ld]", __pTracker->GetTitle()->GetPointer(), __pTracker->GetTrackerId());

	r = AddTrack();
	if (r != E_SUCCESS)
		AppLogException("Error adding track to map", GetErrorMessage(r));

	r = AddPois();
	if (r != E_SUCCESS)
		AppLogException( "Error adding poi markers to map", GetErrorMessage(r));

}

void MapForm::OnMapLongPressed(Map& map,
		const Tizen::Locations::Coordinates& coordinate) {
}

void MapForm::OnMapRegionChanged(Map& map) {

	if (__pSelectedMarker != null) {
		Point placeMarkerPoint;
		result r = __pMap->CoordinateToScreenPosition(
				__pSelectedMarker->GetCoordinates(), placeMarkerPoint);
		if (r == E_SUCCESS)
			__pInfoWindow->SetPosition(placeMarkerPoint.x - X_WINDOW_INFO_LABEL,
					placeMarkerPoint.y - Y_WINDOW_INFO_LABEL);
	}
}

void MapForm::OnMapTapped(Map& map,
		const Tizen::Locations::Coordinates& coordinate) {
	if (__pInfoWindow->GetShowState() == true)
		HideInfoWindow();
}

void MapForm::OnSceneDeactivated(
		const Tizen::Ui::Scenes::SceneId& currentSceneId,
		const Tizen::Ui::Scenes::SceneId& nextSceneId) {
	AppLog("Deactivated MAP_FORM scene");
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
	__pPolyline->SetStrokeThickness(5);
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

	return r;
}

result MapForm::AddPois(void) {
	result r = E_SUCCESS;

	LinkedListT<POI*>* pPoiList = StorageManager::getInstance()->GetPois(
			__pTracker);

	//create a new list of markers from the POI collection
	if (__pMarkerList != null && __pMarkerList->GetCount() > 0) {
		IEnumeratorT<PoiMarker*>* pEnum = __pMarkerList->GetEnumeratorN();

		while (pEnum->MoveNext() == E_SUCCESS) {
			PoiMarker* poiMarker;
			pEnum->GetCurrent(poiMarker);
			__pMap->RemoveMapObject(poiMarker->GetMarker());
		}
		__pMarkerList->RemoveAll();
	}

	__pMarkerList = new LinkedListT<PoiMarker*>();

	Marker* pStartMarker = new (std::nothrow) Marker();

	pStartMarker->SetCoordinates(
			*(__pTracker->StartPosition()->getCoordinates()));
	pStartMarker->SetBitmap(*__pStartMarkerBitmap);
	__pMarkerList->Add(new PoiMarker(null, pStartMarker));
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
			pMarker->AddMapObjectEventListener(*this);
			__pMarkerList->Add(new PoiMarker(pPoi, pMarker));
			__pMap->AddMapObject(pMarker);
		}
	}

	Marker* pEndMarker = new (std::nothrow) Marker();

	pEndMarker->SetCoordinates(*(__pTracker->EndPosition()->getCoordinates()));
	pEndMarker->SetBitmap(*__pEndMarkerBitmap);
	__pMarkerList->Add(new PoiMarker(null, pEndMarker));
	__pMap->AddMapObject(pEndMarker);

	return r;
}

result MapForm::InitializeInfoWindow(void) {
	__pInfoWindow = new (std::nothrow) Label();
	__pInfoWindow->Construct(
			Rectangle(0, 0, W_WINDOW_INFO_LABEL, H_WINDOW_INFO_LABEL), L"");
	__pInfoWindow->SetTextConfig(30, LABEL_TEXT_STYLE_NORMAL);
	__pInfoWindow->SetTextHorizontalAlignment(ALIGNMENT_LEFT);
	__pInfoWindow->SetTextVerticalAlignment(ALIGNMENT_TOP);
	__pInfoWindow->SetBackgroundColor(Color(46, 151, 199));
	__pInfoWindow->SetTextColor(Color::GetColor(COLOR_ID_WHITE));
	__pInfoWindow->SetShowState(false);
	__pInfoWindow->SetMargin(10, 10);

	__pMap->AddControl(*__pInfoWindow);
}

void MapForm::HideInfoWindow() {
	if (__pInfoWindow != null)
		__pInfoWindow->SetShowState(false);
	__pSelectedMarker = null;
}

void MapForm::OnMapObjectTapped(Map& map, MapObject& mapObject,
		const Tizen::Locations::Coordinates& coord) {
	result r = E_SUCCESS;

	IEnumeratorT<PoiMarker*>* pEnum = __pMarkerList->GetEnumeratorN();
	PoiMarker* currentPoi;
	Point placeMarKerPoint;

	String infoText;

	while (pEnum->MoveNext() == E_SUCCESS) {
		r = pEnum->GetCurrent(currentPoi);
		if (currentPoi->GetMarker() == &mapObject) {
			infoText = CreatePoiInfo(currentPoi->GetPoi());
			break;
		}
	}

	Marker* pMarker = static_cast<Marker*>(currentPoi->GetMarker());

	if (__pSelectedMarker == pMarker)
		HideInfoWindow();
	else {
		if (__pInfoWindow->GetShowState() == true)
			HideInfoWindow();

		r = __pMap->CoordinateToScreenPosition(pMarker->GetCoordinates(),
				placeMarKerPoint);
		if (r == E_SUCCESS) {
			__pInfoWindow->SetText(infoText);
			__pInfoWindow->SetPosition(placeMarKerPoint.x - X_WINDOW_INFO_LABEL,
					placeMarKerPoint.y - Y_WINDOW_INFO_LABEL);
			__pInfoWindow->SetShowState(true);
			__pInfoWindow->Invalidate(true);
			__pSelectedMarker = pMarker;
		}
	}
}

void MapForm::OnMapObjectDoubleTapped(Map& map, MapObject& mapObject,
		const Tizen::Locations::Coordinates& coord) {
	LinkedList* parameterList = new LinkedList();

	Coordinates c = coord;
	POI* pPoi = GetPoiFromClick(mapObject, c);

	HideInfoWindow();
	parameterList->Add(pPoi);
	parameterList->Add(Integer(1));
	SceneManager* pSceneMngr = SceneManager::GetInstance();
	pSceneMngr->GoForward(ForwardSceneTransition(SCENE_POI_FORM),
			parameterList);
}

void MapForm::OnMapObjectLongPressed(Map& map, MapObject& mapObject,
		const Tizen::Locations::Coordinates& coord) {
}

void MapForm::ShowInfoWindow() {
	if (__pInfoWindow != null)
		__pInfoWindow->SetShowState(true);
}

Tizen::Base::String MapForm::CreatePoiInfo(POI* pPoi) {
	String retVal;

	retVal.Append(pPoi->GetTimestamp()->ToString());
	retVal.Append("\n");
	retVal.Append(*(pPoi->GetTitle()));
	retVal.Append("\n\n");
	retVal.Append(*(pPoi->GetDescription()));

	return retVal;
}

POI* MapForm::GetPoiFromClick(HMaps::MapObject& mapObject,
		Tizen::Locations::Coordinates& coord) {
	result r = E_SUCCESS;
	POI* retVal = null;

	IEnumeratorT<PoiMarker*>* pEnum = __pMarkerList->GetEnumeratorN();
	PoiMarker* currentPoi;

	while (pEnum->MoveNext() == E_SUCCESS) {
		r = pEnum->GetCurrent(currentPoi);
		if (currentPoi->GetMarker() == &mapObject) {
			retVal = currentPoi->GetPoi();
			break;
		}
	}
	return retVal;
}

