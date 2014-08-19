/*
 * GeoHelper.cpp
 *
 *  Created on: Aug 18, 2014
 *      Author: bohemius
 */

#include "geo/GeoHelper.h"
#include "FBase.h"

using namespace Tizen::Locations;
using namespace HMaps;

GeoHelper::GeoHelper() {
	// TODO Auto-generated constructor stub

}

GeoHelper::~GeoHelper() {
	// TODO Auto-generated destructor stub
}

Coordinates GeoHelper::GetPresentLocation(void) {
	LocationCriteria criteria;

	criteria.SetAccuracy(LOC_ACCURACY_HUNDRED_METERS);
	Coordinates c=LocationProvider::GetLocation(criteria).GetCoordinates();

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

