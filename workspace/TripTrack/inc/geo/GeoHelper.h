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

class GeoHelper {
public:
	GeoHelper();
	virtual ~GeoHelper();

	static Tizen::Locations::Coordinates GetPresentLocation(void);
	static result GetPresentAddress(HMaps::IGeocodeQueryExecuteResponseListener* pListener);
};

#endif /* GEOHELPER_H_ */
