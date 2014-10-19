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
#include "ICRUD.h"

class TTLocation: public I_CRUDable {

public:
	TTLocation();
	virtual ~TTLocation();

	result Construct(long long int id);
	result Construct(Tizen::Locations::Location location);
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
	virtual Tizen::Io::DbStatement* Read(void);
	virtual Tizen::Io::DbStatement* Write(void);
	virtual Tizen::Io::DbStatement* Delete(void);
	virtual Tizen::Io::DbStatement* Update(void);
	long long int GetLocationId() const;
	void SetLocationId(long long int locationId);

private:
	Tizen::Locations::Coordinates* __pCoordinates;
	Tizen::Base::DateTime* __pTimeStamp;
	double __speed;
	double __course;
	long long int __locationId;
};

#endif /* TTLOCATION_H_ */
