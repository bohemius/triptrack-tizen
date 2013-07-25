/*
 * TTLocation.cpp
 *
 *  Created on: Jul 24, 2013
 *      Author: hsp
 */

#include "dao/TTLocation.h"

using namespace Tizen::Locations;
using namespace Tizen::Base;
using namespace Tizen::System;

TTLocation::TTLocation() {
	// TODO Auto-generated constructor stub

}

TTLocation::TTLocation(Tizen::Locations::Location location) {
	__pCoordinates=new Coordinates(location.GetCoordinates());
	__pTimeStamp=new DateTime(location.GetTimestamp());
	__speed=location.GetSpeed();
	__course=location.GetCourse();
}

TTLocation::~TTLocation() {
	__pCoordinates=new Coordinates();
	SystemTime::GetCurrentTime(*__pTimeStamp);
	__speed=0.0;
	__speed=0.0;
}

Tizen::Locations::Coordinates* TTLocation::getCoordinates(void) {
	return __pCoordinates;
}

Tizen::Base::DateTime* TTLocation::getTimestamp(void) {
	return __pTimeStamp;
}

double TTLocation::getSpeed(void) {
	return __speed;
}

double TTLocation::getCourse(void) {
	return __course;
}

void TTLocation::setSpeed(double speed) {
	__speed=speed;
}

void TTLocation::setCourse(double course) {
	__course=course;
}

void TTLocation::setCoordinates(Tizen::Locations::Coordinates* coordinates) {
	__pCoordinates=coordinates;
}

void TTLocation::setTimestamp(Tizen::Base::DateTime* timestamp) {
	__pTimeStamp=timestamp;
}

double TTLocation::getLatitude(void) {
	return __pCoordinates->GetLatitude();
}

double TTLocation::getLongitude(void) {
	return __pCoordinates->GetLongitude();
}

double TTLocation::getAltitude(void) {
	return __pCoordinates->GetAltitude();
}


