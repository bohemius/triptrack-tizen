/*
 * GeoHelper.cpp
 *
 *  Created on: Aug 18, 2014
 *      Author: bohemius
 */

#include "geo/GeoHelper.h"
#include "FBase.h"
#include "dao/TTLocation.h"

using namespace Tizen::Locations;
using namespace Tizen::Base::Collection;
using namespace HMaps;

GeoHelper::GeoHelper() {
	// TODO Auto-generated constructor stub

}

GeoHelper::~GeoHelper() {
	// TODO Auto-generated destructor stub
}

Coordinates GeoHelper::GetPresentLocation(void) {
	LocationCriteria criteria;

	criteria.SetAccuracy(LOC_ACCURACY_FINEST);
	Coordinates c = LocationProvider::GetLocation(criteria).GetCoordinates();

	//Coordinates c;

	//c.SetLatitude(50.6581693);
	//c.SetLongitude(14.0402449);

	return c;
	//return LocationProvider::GetLastKnownLocation().GetCoordinates();
}

result GeoHelper::GetPresentAddress(
		IGeocodeQueryExecuteResponseListener* pListener) {
	result r = E_SUCCESS;

	Coordinates coord = GetPresentLocation();
	ReverseGeocodeQuery* pQuery = new (std::nothrow) ReverseGeocodeQuery();
	r = pQuery->Construct(coord,
			ReverseGeocodeQuery::REVERSE_GEOCODE_MODE_RETRIEVE_ADDRESSES);
	if (r != E_SUCCESS)
		return r;
	r = pQuery->Execute(pListener);
	return r;
}

GeoBoundingBox GeoHelper::GetGeoBoundingBox(Tracker* tracker) {
	result r = E_SUCCESS;
	GeoBoundingBox retVal;

	LinkedListT<TTLocation*>* pTrackPoints = tracker->GetTrackPoints();
	IEnumeratorT<TTLocation*>* pEnum =
			tracker->GetTrackPoints()->GetEnumeratorN();
	r = GetLastResult();
	if (r != E_SUCCESS) {
		AppLogException(
				"Error getting locations enumerator for bounding box calculation: ", GetErrorMessage(r));
		return retVal;
	}

	TTLocation* location;

	r = pTrackPoints->GetAt(0, location);
	if (r != E_SUCCESS) {
		AppLogException(
				"Error getting initial location for bounding box calculation: ", GetErrorMessage(r));
		return retVal;
	}

	Coordinates topLeft(*(location->getCoordinates()));
	Coordinates bottomRight(*(location->getCoordinates()));

	double tempLong;
	double tempLat;

	while (pEnum->MoveNext() == E_SUCCESS) {
		r = pEnum->GetCurrent(location);
		tempLong=location->getLongitude();
		tempLat=location->getLatitude();

		if (r == E_SUCCESS) {
			if (topLeft.GetLongitude() > tempLong)
				topLeft.SetLongitude(tempLong);
			if (topLeft.GetLatitude() < tempLat)
				topLeft.SetLatitude(tempLat);
			if (bottomRight.GetLongitude() < tempLong)
				bottomRight.SetLongitude(tempLong);
			if (bottomRight.GetLatitude() > tempLat)
				bottomRight.SetLatitude(tempLat);
		} else
			AppLogException(
					"Error getting location for bounding box calculation: ", GetErrorMessage(r));
	}

	// Throw an error if boundaries contains poles
	if (topLeft.GetLatitude() >= 80.0	|| bottomRight.GetLatitude() <= -66) {
		AppLogException("Bounding box near poles");
		return retVal;
	}

	retVal.SetTopLeft(topLeft);
	retVal.SetBottomRight(bottomRight);

	return retVal;
}

