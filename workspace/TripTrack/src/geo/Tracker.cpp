/*
 * Tracker.cpp
 *
 *  Created on: Jul 21, 2013
 *      Author: hsp
 */

#include "geo/Tracker.h"
#include "dao/TTLocation.h"
#include "dao/StorageManager.h"
#include "util/BootstrapManager.h"

using namespace Tizen::Locations;
using namespace Tizen::Base::Collection;
using namespace Tizen::Base;
using namespace Tizen::Io;

const int Tracker::ACTIVE;
const int Tracker::PAUSED;
const int Tracker::LOCKED;

Tracker::Tracker() {
}

Tracker::~Tracker() {
	// TODO Auto-generated destructor stub
}

result Tracker::AddLocation(Location location) {
	TTLocation* position = new TTLocation();
	result r = E_SUCCESS;

	//TODO check if this tracker is current tracker, otherwise return error
	r = position->Construct(location);
	if (r != E_SUCCESS) {
		AppLogException(
				"Error constructing location with timestamp [%ls]", location.GetTimestamp().ToString().GetPointer());
		r = E_FAILURE;
		return r;
	}
	AppLog(
			"Successfully added location with timestamp [%ls] and id [%d]", position->getTimestamp()->ToString().GetPointer(), position->GetLocationId());
	__pTrackPoints->Add(position);
	return r;
}

TTLocation* Tracker::FindLocationAtTime(Tizen::Base::DateTime timeStamp,
		const Tizen::Base::DateTime timeWindow) {
	return 0;
}

TTLocation* Tracker::StartPosition(void) {
	result r = E_SUCCESS;
	AppLog("Getting starting position for tracker id: [%d]", __trackerId);

	TTLocation* retVal = 0;
	r = __pTrackPoints->GetAt(0, retVal);
	if (r != E_SUCCESS || retVal == 0) {
		AppLogException(
				"Error getting starting position for tracker [%d]: [%s]", __trackerId, GetErrorMessage(r));
		return 0;
	}
	return retVal;
}

TTLocation* Tracker::EndPosition(void) {
	result r = E_SUCCESS;
	AppLog("Getting ending position for tracker id: [%d]", __trackerId);

	TTLocation* retVal = 0;
	r = __pTrackPoints->GetAt(__pTrackPoints->GetCount() - 1, retVal);
	if (r != E_SUCCESS || retVal == 0) {
		AppLogException(
				"Error getting ending position for tracker [%d]: [%s]", __trackerId, GetErrorMessage(r));
		return 0;
	}
	return retVal;
}

//Constructs existing tracker from database using trackerId
result Tracker::Construct(int id) {
	StorageManager* store = StorageManager::getInstance();
	DbEnumerator* pEnum = 0;
	result r = E_SUCCESS;

	AppLog("Reading tracker data from database for tracker id: [%d]", id);
	__trackerId = id;
	pEnum = store->CRUDoperation(this, I_CRUDable::READ);
	if (pEnum == 0) {
		AppLogException("Error reading tracker with id: [%d]", id);
		r = E_FAILURE;
		return r;
	}

	while (pEnum->MoveNext() == E_SUCCESS) {
		pEnum->GetStringAt(0, *__pDescription);
		pEnum->GetStringAt(1, *__pTitle);
		pEnum->GetDoubleAt(2, __distance);
		pEnum->GetIntAt(3, __status);
	}
	__pTrackPoints=StorageManager::getInstance()->GetLocations(__trackerId);
	AppLog(
			"Successfully loaded data for tracker: [%ls]", __pTitle->GetPointer());

	delete pEnum;
	return r;
}

//Constructs a new tracker using the supplied parameters, sets the status to paused
//and saves it to the database
result Tracker::Construct(Tizen::Base::String &Description,
		Tizen::Base::String &Title) {
	StorageManager* store = StorageManager::getInstance();
	DbEnumerator* pEnum = 0;
	result r = E_SUCCESS;

	AppLog(
			"Creating a new tracker with title [%ls] and description [%ls].", Title.GetPointer(), Description.GetPointer());
	__pDescription = &Description;
	__pTitle = &Title;
	__status = PAUSED;
	pEnum = store->CRUDoperation(this, I_CRUDable::CREATE);
	if (r != E_SUCCESS) {
		AppLogException(
				"Error storing the new tracker [%ls] in the database: [%ls]", __pTitle->GetPointer(), GetErrorMessage(r));
		return r;
	}
	//get the inserted ID using last_insert_rowid()
	r = pEnum->MoveNext();
	if (r != E_SUCCESS) {
		AppLogException(
				"Error getting tracker ID using last_insert_rowid(): [%s]", GetErrorMessage(r));
		return r;
	} else {
		r = pEnum->GetIntAt(0, __trackerId);
		if (r != E_SUCCESS) {
			AppLogException(
					"Error getting tracker ID using last_insert_rowid(): [%s]", GetErrorMessage(r));
			return r;
		}
	}

	AppLog(
			"Successfully stored the new tracker [%ls] in the database with ID: [%d]", __pTitle->GetPointer(), __trackerId);
	delete pEnum;
	__pTrackPoints=new LinkedListT<TTLocation*>();
	return r;
}

LinkedListT<TTLocation*>* Tracker::GetTrack(void) {
	return __pTrackPoints;
}

String* Tracker::GetDescription() const {
	return __pDescription;
}

void Tracker::SetDescription(String* description) {
	__pDescription = description;
}

String* Tracker::GetTitle() const {
	return __pTitle;
}

void Tracker::SetTitle(String* title) {
	__pTitle = title;
}

Tizen::Base::Collection::LinkedListT<TTLocation*>* Tracker::GetTrackPoints() const {
	return __pTrackPoints;
}

int Tracker::GetTrackerId() const {
	return __trackerId;
}

Tizen::Io::DbStatement* Tracker::Read(void) {
	String sqlStatement;
	DbStatement* pStmt = null;
	Database* db;
	result r = E_SUCCESS;

	sqlStatement.Append(
			L"SELECT Description, Title, Distance, Status FROM Track WHERE ID = ?");

	db = BootstrapManager::getInstance()->getDatabase();
	pStmt = db->CreateStatementN(sqlStatement);
	AppLog(
			"Creating sql statement for SELECT for tracker with ID: [%d]", __trackerId);
	if (pStmt == 0 || r != E_SUCCESS) {
		AppLogException(
				"Error creating sql statement for SELECT for tracker with ID [%d]: [%s]", __trackerId, GetErrorMessage(r));
		return 0;
	}
	AppLog(
			"Sql SELECT statement created for tracker with ID: [%d]", __trackerId);
	pStmt->BindInt(0, __trackerId);
	return pStmt;
}

Tizen::Io::DbStatement* Tracker::Write(void) {
	String sqlStatement;
	DbStatement* pStmt = null;
	Database* db;
	result r = E_SUCCESS;

	sqlStatement.Append(
			L"INSERT INTO Track (Description, Title, Distance, Status) VALUES (?,?,?,?); SELECT last_insert_rowid();");

	db = BootstrapManager::getInstance()->getDatabase();
	pStmt = db->CreateStatementN(sqlStatement);
	AppLog(
			"Creating sql statement for INSERT for tracker: [%ls]", __pTitle->GetPointer());
	if (pStmt == 0 || r != E_SUCCESS) {
		AppLogException(
				"Error creating sql statement for INSERT for tracker [%ls]: [%s]", __pTitle->GetPointer(), GetErrorMessage(r));
		return 0;
	}
	AppLog(
			"Sql INSERT statement created for tracker: [%ls]", __pTitle->GetPointer());
	pStmt->BindString(0, *__pDescription);
	pStmt->BindString(1, *__pTitle);
	pStmt->BindDouble(2, __distance);
	pStmt->BindInt(3, __status);
	return pStmt;
}

Tizen::Io::DbStatement* Tracker::Delete(void) {
	String sqlStatement;
	DbStatement* pStmt = null;
	Database* db;
	result r = E_SUCCESS;

	sqlStatement.Append(L"DELETE FROM Track WHERE ID = ?");

	db = BootstrapManager::getInstance()->getDatabase();
	pStmt = db->CreateStatementN(sqlStatement);
	AppLog(
			"Creating DELETE statement for tracker: [%ls]", __pTitle->GetPointer());
	if (pStmt == 0 || r != E_SUCCESS) {
		AppLogException(
				"Error creating sql statement for DELETE for tracker [%ls]: [%s]", __pTitle->GetPointer(), GetErrorMessage(r));
		return 0;
	}
	AppLog(
			"Sql DELETE statement created for tracker: [%ls]", __pTitle->GetPointer());
	pStmt->BindInt(0, __trackerId);
	return pStmt;
}

Tizen::Io::DbStatement* Tracker::Update(void) {
	String sqlStatement;
	DbStatement* pStmt = null;
	Database* db;
	result r = E_SUCCESS;

	sqlStatement.Append(
			L"UPDATE Track SET Description = ?, Title = ?, Distance = ?, Status = ? WHERE ID = ?");

	db = BootstrapManager::getInstance()->getDatabase();
	pStmt = db->CreateStatementN(sqlStatement);
	AppLog(
			"Creating sql statement for UPDATE for tracker: [%ls]", __pTitle->GetPointer());
	if (pStmt == 0 || r != E_SUCCESS) {
		AppLogException(
				"Error creating sql statement for UPDATE for tracker [%ls]: [%s]", __pTitle->GetPointer(), GetErrorMessage(r));
		return 0;
	}
	AppLog(
			"Sql UPDATE statement created for tracker: [%ls]", __pTitle->GetPointer());
	pStmt->BindString(0, *__pDescription);
	pStmt->BindString(1, *__pTitle);
	pStmt->BindDouble(2, __distance);
	pStmt->BindInt(3, __status);
	pStmt->BindInt(4, __trackerId);
	return pStmt;
}

double Tracker::GetDistance() const {
	return __distance;
}

int Tracker::GetStatus() const {
	return __status;
}

void Tracker::SetStatus(int status) {
	__status = status;
}

