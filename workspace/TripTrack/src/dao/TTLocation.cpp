/*
 * TTLocation.cpp
 *
 *  Created on: Jul 24, 2013
 *      Author: hsp
 */

#include "dao/TTLocation.h"
#include "dao/StorageManager.h"
#include "geo/TrackerManager.h"
#include "util/BootstrapManager.h"

using namespace Tizen::Locations;
using namespace Tizen::Base;
using namespace Tizen::System;
using namespace Tizen::Io;

TTLocation::TTLocation() :
		__pCoordinates(null), __pTimeStamp(null) {
}

TTLocation::~TTLocation() {
	delete (__pCoordinates);
	delete (__pTimeStamp);
}

//Constructs existing location from database using locationId
result TTLocation::Construct(long long int id) {
	__locationId = id;

	AppLog(
			"Reading location data from database for location id: [%lld]", __locationId);
	Read();

	AppLog(
				"Constructed location with values latitude=[%f], longitude=[%f], altitude=[%f], time=[%S], speed=[%f], course=[%f], id=[%lld]",
				__pCoordinates->GetLatitude(),
				__pCoordinates->GetLongitude(),
				__pCoordinates->GetAltitude(),
				__pTimeStamp->ToString().GetPointer(),
				__speed,
				__course,
				__locationId);

	return E_SUCCESS;
}

//Constructs a new location using Location usually supplied by the device GPS
result TTLocation::Construct(Location location) {
	StorageManager* store = StorageManager::getInstance();
	DbEnumerator* pEnum = 0;
	Database* db = BootstrapManager::getInstance()->getDatabase();
	result r = E_SUCCESS;

	__pTimeStamp->SetValue(location.GetTimestamp());
	AppLog(
			"Creating a new location with timestamp [%ls].", __pTimeStamp->ToString().GetPointer());

	__speed = location.GetSpeed();
	__course = location.GetCourse();
	__pCoordinates->Set(location.GetCoordinates().GetLatitude(),
			location.GetCoordinates().GetLongitude(),
			location.GetCoordinates().GetAltitude());

	pEnum = store->CRUDoperation(this, I_CRUDable::CREATE);
	if (r != E_SUCCESS) {
		AppLogException(
				"Error storing the new location with timestamp [%ls] in the database: [%s]", __pTimeStamp->ToString().GetPointer(), GetErrorMessage(r));
		return r;
	}
	//get the inserted ID using last_insert_rowid()
	__locationId = db->GetLastInsertRowId();

	AppLog(
			"Successfully stored the new location with timestamp [%ls] in the database with ID: [%d]", __pTimeStamp->ToString().GetPointer(), __locationId);
	delete pEnum;
	return r;
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
	String sqlStatement;
	DbStatement* pStmt = null;
	DbEnumerator* pEnum = null;
	Database* db;
	result r = E_SUCCESS;

	sqlStatement.Append(
			L"SELECT Latitude, Longitude, Altitude, TimeSig, Speed, Course FROM location WHERE ID = ?");

	db = BootstrapManager::getInstance()->getDatabase();
	AppLog(
			"Creating sql statement for SELECT for location with ID: [%d]", __locationId);
	pStmt = db->CreateStatementN(sqlStatement);
	r = GetLastResult();
	if (r != E_SUCCESS) {
		AppLogException(
				"Error creating sql statement for SELECT for location with ID [%d]: [%s]", __locationId, GetErrorMessage(r));
		return null;
	}
	AppAssert(pStmt);
	AppLog(
			"Sql SELECT statement created for location with ID: [%d]", __locationId);

	r = pStmt->BindInt64(0, __locationId);
	if (r != E_SUCCESS) {
		AppLogException(
				"Error binding __locationId for SELECT for location with ID [%d]: [%s]", __locationId, GetErrorMessage(r));
		return null;
	}

	double latitude, longitude, altitude, speed, course;
	String dateString;
	DateTime timeStamp;

	pEnum = db->ExecuteStatementN(*pStmt);
	//Latitude, Longitude, Altitude, TimeSig, Speed, Course
	while (pEnum->MoveNext() == E_SUCCESS) {
		pEnum->GetDoubleAt(0, latitude);
		pEnum->GetDoubleAt(1, longitude);
		pEnum->GetDoubleAt(2, altitude);
		pEnum->GetStringAt(3, dateString);
		pEnum->GetDoubleAt(4, speed);
		pEnum->GetDoubleAt(5, course);
	}

	Coordinates* pCoor = new Coordinates();
	pCoor->Set(latitude, longitude, altitude);

	r = DateTime::Parse(dateString, timeStamp);
	if (r != E_SUCCESS) {
		AppLogException(
				"Error parsing timestamp for location with id [%d]: [%s]", __locationId, GetErrorMessage(r));
		return null;
	}
	AppLog(
			"Read values latitude=[%f], longitude=[%f], altitude=[%f], time=[%S], speed=[%f], course=[%f]", latitude, longitude, altitude, timeStamp.ToString().GetPointer(), speed, course);

	setCoordinates(pCoor);
	setSpeed(speed);
	setCourse(course);
	setTimestamp(&timeStamp);

	delete pEnum;
	delete pStmt;

	AppLog(
			"Successfully loaded data for location with id [%lld]", __locationId);
	return null;

	/*String sqlStatement;
	 DbStatement* pStmt = null;
	 Database* db;
	 result r = E_SUCCESS;

	 sqlStatement.Append(
	 L"SELECT Latitude, Longitude, Altitude, TimeSig, Speed, Course FROM location WHERE ID = ?");

	 db = BootstrapManager::getInstance()->getDatabase();
	 pStmt = db->CreateStatementN(sqlStatement);
	 AppLog(
	 "Creating sql statement for SELECT for location with ID: [%d]", __locationId);
	 if (pStmt == 0 || r != E_SUCCESS) {
	 AppLogException(
	 "Error creating sql statement for SELECT for location with ID [%d]: [%s]", __locationId, GetErrorMessage(r));
	 return 0;
	 }
	 AppLog(
	 "Sql SELECT statement created for location with ID: [%d]", __locationId);
	 pStmt->BindInt64(0, __locationId);
	 return pStmt;*/
}

Tizen::Io::DbStatement* TTLocation::Write(void) {
	String sqlStatement;
	DbStatement* pStmt = null;
	Database* db;
	result r = E_SUCCESS;

	sqlStatement.Append(
			L"INSERT INTO location (Latitude, Longitude, Altitude, TimeSig, Speed, Course, Track_ID) VALUES (?,?,?,?,?,?,?)");

	db = BootstrapManager::getInstance()->getDatabase();
	pStmt = db->CreateStatementN(sqlStatement);
	AppLog(
			"Creating sql statement for INSERT for location with timestamp [%ls]", __pTimeStamp->ToString().GetPointer());
	if (pStmt == 0 || r != E_SUCCESS) {
		AppLogException(
				"Error creating sql statement for INSERT for location with timestamp [%ls]: [%s]", __pTimeStamp->ToString().GetPointer(), GetErrorMessage(r));
		return 0;
	}
	AppLog(
			"Sql INSERT statement created for location with timestamp [%ls]", __pTimeStamp->ToString().GetPointer());
	pStmt->BindDouble(0, __pCoordinates->GetLatitude());
	pStmt->BindDouble(1, __pCoordinates->GetLongitude());
	pStmt->BindDouble(2, __pCoordinates->GetAltitude());

	String dateString = __pTimeStamp->ToString();
	pStmt->BindString(3, dateString);
	pStmt->BindDouble(4, __speed);
	pStmt->BindDouble(5, __course);
	pStmt->BindInt64(6,
			TrackerManager::getInstance()->GetCurrentTracker()->GetTrackerId());
	return pStmt;
}

Tizen::Io::DbStatement* TTLocation::Delete(void) {
	String sqlStatement;
	DbStatement* pStmt = null;
	Database* db;
	result r = E_SUCCESS;

	sqlStatement.Append(L"DELETE FROM location WHERE ID = ?");

	db = BootstrapManager::getInstance()->getDatabase();
	pStmt = db->CreateStatementN(sqlStatement);
	AppLog(
			"Creating DELETE statement for location with timestamp: [%ls]", __pTimeStamp->ToString().GetPointer());
	if (pStmt == 0 || r != E_SUCCESS) {
		AppLogException(
				"Error creating sql statement for DELETE for location with timestamp [%ls]: [%s]", __pTimeStamp->ToString().GetPointer(), GetErrorMessage(r));
		return 0;
	}
	AppLog(
			"Sql DELETE statement created for location with timestamp [%ls]", __pTimeStamp->ToString().GetPointer());
	pStmt->BindInt64(0, __locationId);
	return pStmt;
}

Tizen::Io::DbStatement* TTLocation::Update(void) {
	String sqlStatement;
	DbStatement* pStmt = null;
	Database* db;
	result r = E_SUCCESS;

	sqlStatement.Append(
			L"UPDATE location SET  Latitude = ?, Longitude = ?, Altitude = ?, TimeSig = ?, Speed = ?, Course = ? WHERE ID = ?");

	db = BootstrapManager::getInstance()->getDatabase();
	pStmt = db->CreateStatementN(sqlStatement);
	AppLog(
			"Creating sql statement for UPDATE for location with timestamp [%ls]", __pTimeStamp->ToString().GetPointer());
	if (pStmt == 0 || r != E_SUCCESS) {
		AppLogException(
				"Error creating sql statement for UPDATE for location with timestamp [%ls]: [%s]", __pTimeStamp->ToString().GetPointer(), GetErrorMessage(r));
		return 0;
	}
	AppLog(
			"Sql UPDATE statement created for location with timestamp [%ls]", __pTimeStamp->ToString().GetPointer());

	pStmt->BindDouble(0, __pCoordinates->GetLatitude());
	pStmt->BindDouble(1, __pCoordinates->GetLongitude());
	pStmt->BindDouble(2, __pCoordinates->GetAltitude());

	String dateString = __pTimeStamp->ToString();
	pStmt->BindString(3, dateString);
	pStmt->BindDouble(4, __speed);
	pStmt->BindDouble(5, __course);
	pStmt->BindInt64(6, __locationId);
	return pStmt;
}

long long int TTLocation::GetLocationId() const {
	return __locationId;
}

void TTLocation::SetLocationId(long long int locationId) {
	__locationId = locationId;
}

