/*
 * GeoHelper.h
 *
 *  Created on: Aug 18, 2014
 *      Author: bohemius
 */

#ifndef GEOHELPER_H_
#define GEOHELPER_H_

#include <FLocations.h>
#include <HMaps.h>
#include "geo/Tracker.h"

class GeoHelper {
public:
	GeoHelper();
	virtual ~GeoHelper();

	static Tizen::Locations::Coordinates GetPresentLocation(void);
	static result GetPresentAddress(HMaps::IGeocodeQueryExecuteResponseListener* pListener);
	static HMaps::GeoBoundingBox GetGeoBoundingBox(Tracker* tracker);
};

#endif /* GEOHELPER_H_ */
