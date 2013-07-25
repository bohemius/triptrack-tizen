/*
 * TTLocation.h
 *
 *  Created on: Jul 24, 2013
 *      Author: hsp
 *
 *  Since Tizen::Locations::Location does not have a public constructor and
 *  is most likely a singleton I have to compose own class to use for typed
 *  collections.
 */

#ifndef TTLOCATION_H_
#define TTLOCATION_H_

#include <FLocations.h>
#include <FBase.h>
#include <FSystem.h>

class TTLocation {

public:
	TTLocation();
	TTLocation(Tizen::Locations::Location location);
	virtual ~TTLocation();

	Tizen::Locations::Coordinates* getCoordinates(void);
	Tizen::Base::DateTime* getTimestamp(void);
	double getSpeed(void);
	double getCourse(void);
	void setSpeed(double speed);
	void setCourse(double course);
	void setCoordinates(Tizen::Locations::Coordinates* coordinates);
	void setTimestamp(Tizen::Base::DateTime* timestamp);
	double getLatitude(void);
	double getLongitude(void);
	double getAltitude(void);

private:
	Tizen::Locations::Coordinates* __pCoordinates;
	Tizen::Base::DateTime* __pTimeStamp;
	double __speed;
	double __course;
};

#endif /* TTLOCATION_H_ */
