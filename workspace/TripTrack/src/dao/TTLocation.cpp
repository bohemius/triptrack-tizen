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
	__pCoordinates = null;
	__pTimeStamp=null;
	__speed = 0.0;
	__course = 0.0;
}

TTLocation::TTLocation(Tizen::Locations::Location location) {
	__pCoordinates = new Coordinates(location.GetCoordinates());
	__pTimeStamp = new DateTime(location.GetTimestamp());
	__speed = location.GetSpeed();
	__course = location.GetCourse();
}

TTLocation::~TTLocation() {
	delete (__pCoordinates);
	delete (__pTimeStamp);
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
	__speed = speed;
}

void TTLocation::setCourse(double course) {
	__course = course;
}

void TTLocation::setCoordinates(Tizen::Locations::Coordinates* coordinates) {
	__pCoordinates = coordinates;
}

void TTLocation::setTimestamp(Tizen::Base::DateTime* timestamp) {
	__pTimeStamp = timestamp;
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

Tizen::Io::DbStatement* TTLocation::Read(void) {
	return 0;
}


Tizen::Io::DbStatement* TTLocation::Write(void) {
	return 0;
}


Tizen::Io::DbStatement* TTLocation::Delete(void) {
	return 0;
}


Tizen::Io::DbStatement* TTLocation::Update(void) {
	return 0;
}
