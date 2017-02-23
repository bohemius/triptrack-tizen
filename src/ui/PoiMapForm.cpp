/*
 * PoiMapForm.cpp
 *
 *  Created on: Sep 30, 2014
 *      Author: bohemius
 */

#include "ui/PoiMapForm.h"
#include "dao/StorageManager.h"
#include "dao/TTMedia.h"
#include "util/GraphicsUtils.h"
#include "SceneRegister.h"
#include <FLocations.h>
#include <FMedia.h>

using namespace Tizen::Ui;
using namespace Tizen::Ui::Scenes;
using namespace Tizen::Graphics;
using namespace Tizen::App;
using namespace Tizen::Base;
using namespace Tizen::Base::Collection;
using namespace Tizen::Locations;
using namespace Tizen::Media;

using namespace HMaps;

static const int W_THUMBNAIL = 400;
static const int H_THUMBNAIL = 200;
static const int X_THUMBNAIL = 200;
static const int Y_THUMBNAIL = 210;

PoiMapForm::PoiMapForm() :
		__pMap(null), __pPolyline(null), __pMarkerList(null), __pPoiThumbnail(
				null), __pSelectedMarker(null) {
}

PoiMapForm::~PoiMapForm() {
}

void PoiMapForm::OnMapLongPressed(HMaps::Map& map,
		const Tizen::Locations::Coordinates& coordinate) {
}

void PoiMapForm::OnMapRegionChanged(HMaps::Map& map) {
	if (__pSelectedMarker != null) {
		Point placeMarkerPoint;
		result r = __pMap->CoordinateToScreenPosition(
				__pSelectedMarker->GetCoordinates(), placeMarkerPoint);
		if (r == E_SUCCESS)
			__pPoiThumbnail->SetPosition(placeMarkerPoint.x - X_THUMBNAIL,
					placeMarkerPoint.y - Y_THUMBNAIL);
	}
}

void PoiMapForm::OnMapTapped(HMaps::Map& map,
		const Tizen::Locations::Coordinates& coordinate) {
}

result PoiMapForm::OnInitializing(void) {
	result r = E_SUCCESS;

	AppResource* pAppResource = Application::GetInstance()->GetAppResource();
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

	if (__pPoiThumbnail == null)
		InitializeThumbnail();

	__pMap->AddMapEventListener(*this);
	SetFormBackEventListener(this);

	return r;
}

result PoiMapForm::OnTerminating(void) {
}

void PoiMapForm::OnFormBackRequested(Tizen::Ui::Controls::Form& source) {
	SceneManager* pSceneMngr = SceneManager::GetInstance();
	pSceneMngr->GoBackward(BackwardSceneTransition(SCENE_MAIN_FORM));
}

void PoiMapForm::OnSceneActivatedN(
		const Tizen::Ui::Scenes::SceneId& previousSceneId,
		const Tizen::Ui::Scenes::SceneId& currentSceneId,
		Tizen::Base::Collection::IList* pArgs) {
	result r = E_SUCCESS;
	AppLog("Activated POIMAP_FORM scene");

	r = AddPois();
	if (r != E_SUCCESS)
		AppLogException( "Error adding poi markers to map", GetErrorMessage(r));
	__pMap->Draw();

}

void PoiMapForm::OnSceneDeactivated(
		const Tizen::Ui::Scenes::SceneId& currentSceneId,
		const Tizen::Ui::Scenes::SceneId& nextSceneId) {
	AppLog("Deactivated POIMAP_FORM scene");
}

result PoiMapForm::AddPois(void) {
	result r = E_SUCCESS;

	LinkedListT<POI*>* pPoiList = StorageManager::getInstance()->GetPois();

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

	if (__pPolyline != null) {
		__pMap->RemoveMapObject(__pPolyline);
		delete __pPolyline;
		__pPolyline = null;
	}

	__pPolyline = new (std::nothrow) Polyline();

	IEnumeratorT<POI*>* pPoiEnum = pPoiList->GetEnumeratorN();
	while (pPoiEnum->MoveNext() == E_SUCCESS) {
		POI* pPoi;
		pPoiEnum->GetCurrent(pPoi);
		Coordinates coor = *(pPoi->GetLocation());
		if (!(coor.GetAltitude() == 0.0 && coor.GetLatitude() == 0.0
				&& coor.GetLongitude() == 0.0)) {
			__pPolyline->AppendToPath(coor);
		}
	}
	AppLog(
			"Created polyline with [%d] pois.", __pPolyline->GetPathN()->GetCount());
	__pPolyline->SetVisible(false);

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
		else
			AppLog(
					"topleft %3.2f, %3.2f botright %3.2f, %3.2f", box.GetTopLeft().GetLatitude(), box.GetTopLeft().GetLongitude(), box.GetBottomRight().GetLatitude(), box.GetBottomRight().GetLongitude());
	}

	pPoiEnum->Reset();
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

	return r;
}

result PoiMapForm::InitializeThumbnail(void) {
	__pPoiThumbnail = new (std::nothrow) PoiThumbnail();

	result r = __pPoiThumbnail->Construct(
			Rectangle(0, 0, W_THUMBNAIL, H_THUMBNAIL));
	__pPoiThumbnail->SetShowState(false);

	__pMap->AddControl(*__pPoiThumbnail);

	return r;
}

void PoiMapForm::HideThumbnail() {
	if (__pPoiThumbnail != null)
		__pPoiThumbnail->SetShowState(false);
	__pSelectedMarker = null;
}

void PoiMapForm::ShowThumbnail() {
	if (__pPoiThumbnail != null)
		__pPoiThumbnail->SetShowState(true);
}

Tizen::Base::String PoiMapForm::CreatePoiInfo(POI* pPoi) {
	return String(L"");
}

void PoiMapForm::OnMapObjectTapped(HMaps::Map& map, HMaps::MapObject& mapObject,
		const Tizen::Locations::Coordinates& coord) {
	result r = E_SUCCESS;

	IEnumeratorT<PoiMarker*>* pEnum = __pMarkerList->GetEnumeratorN();
	PoiMarker* currentPoi;
	Point placeMarKerPoint;
	Coordinates c = coord;

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
		HideThumbnail();
	else {
		if (__pPoiThumbnail->GetShowState() == true)
			HideThumbnail();

		r = __pMap->CoordinateToScreenPosition(pMarker->GetCoordinates(),
				placeMarKerPoint);
		if (r == E_SUCCESS) {
			TTMedia* pMedia = new TTMedia();
			POI* pPoi = GetPoiFromClick(mapObject, c);
			Bitmap* pBitmap = null;
			long long int defId = pPoi->GetDefImageId();

			AppLog("Clicked poi with id %lld", defId);

			if (defId < 0) {
				AppResource* pAppRes =
						Application::GetInstance()->GetAppResource();
				pBitmap = pAppRes->GetBitmapN(L"BlankPoi.png");
			} else {
				r = pMedia->Construct(defId);

				pBitmap = GraphicsUtils::CreateBitmap(*(pMedia->GetSourceUri()));
			}

			__pPoiThumbnail->SetBitmap(pBitmap);
			__pPoiThumbnail->SetPosition(placeMarKerPoint.x - X_THUMBNAIL,
					placeMarKerPoint.y - Y_THUMBNAIL);
			__pPoiThumbnail->SetShowState(true);
			__pPoiThumbnail->Invalidate(true);
			__pSelectedMarker = pMarker;

			delete pMedia;
		}
	}
}

void PoiMapForm::OnMapObjectDoubleTapped(HMaps::Map& map,
		HMaps::MapObject& mapObject,
		const Tizen::Locations::Coordinates& coord) {
}

void PoiMapForm::OnMapObjectLongPressed(HMaps::Map& map,
		HMaps::MapObject& mapObject,
		const Tizen::Locations::Coordinates& coord) {
}

POI* PoiMapForm::GetPoiFromClick(HMaps::MapObject& mapObject,
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

PoiThumbnail::PoiThumbnail() :
		__pThumbnail(null) {
}

PoiThumbnail::~PoiThumbnail() {
}

result PoiThumbnail::Construct(const Rectangle& rect) {
	result r = Container::Construct(rect, true, true);

	SetMinimumSize(Dimension(0, 0));
	SetMaximumSize(Dimension(W_THUMBNAIL, H_THUMBNAIL));

	return r;
}

result PoiThumbnail::OnDraw(void) {
	result r = E_SUCCESS;
	Canvas* pCanvas = GetCanvasN();

	//pCanvas->SetForegroundColor(Color(46, 151, 199));
	//ImageBuffer* pBuf=new ImageBuffer();
	//pCanvas->SetBackgroundColor(Color(46, 151, 45,128));
	//pBuf->Construct(__pThumbnail);
	r = pCanvas->DrawBitmap(Rectangle(0, 0, W_THUMBNAIL, H_THUMBNAIL),
			*__pThumbnail);
	//pCanvas->SetCompositeMode(COMPOSITE_MODE_OVERLAY);

	r = pCanvas->FillRectangle(Color(46, 255, 255, 85),
			Rectangle(0, 0, W_THUMBNAIL, H_THUMBNAIL));

	pCanvas->SetLineWidth(5);
	pCanvas->SetForegroundColor(Color(46, 151, 199));
	r = pCanvas->DrawRoundRectangle(Rectangle(0, 0, W_THUMBNAIL, H_THUMBNAIL),
			Dimension(5, 5));
	//pCanvas->FillEllipse(Color(255, 255, 255,75),Rectangle(W_THUMBNAIL/4, H_THUMBNAIL/4, W_THUMBNAIL/2, H_THUMBNAIL/2));
	delete pCanvas;

	return r;
}

void PoiThumbnail::SetBitmap(Tizen::Graphics::Bitmap* pBitmap) {
	if (__pThumbnail != null)
		delete __pThumbnail;

	__pThumbnail = pBitmap;
}

