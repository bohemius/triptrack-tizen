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
#include "geo/TrackerManager.h"

using namespace Tizen::Locations;
using namespace Tizen::Graphics;
using namespace Tizen::Base::Collection;
using namespace Tizen::Base;
using namespace Tizen::Io;

const int Tracker::ACTIVE;
const int Tracker::PAUSED;
const int Tracker::LOCKED;

Tracker::Tracker() :
		__pDescription(null), __pTitle(null), __trackerId(-1), __pTrackPoints(
				null) {
}

Tracker::~Tracker() {
	// TODO deallocate tracker related items
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
	if (__pTrackPoints == null)
		__pTrackPoints = new LinkedListT<TTLocation*>();
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
result Tracker::Construct(long long int id) {
	__trackerId = id;

	AppLog(
			"Reading tracker data from database for tracker id: [%lld]", __trackerId);
	Read();

	AppLog(
			"Constructed track with values desc=[%S], title=[%S], distance=[%f], status=[%d], id=[%lld]", __pDescription->GetPointer(), __pTitle->GetPointer(), __distance, __status, __trackerId);

	return E_SUCCESS;
}

//Constructs a new tracker using the supplied parameters, sets the status to paused
//and saves it to the database
result Tracker::Construct(Tizen::Base::String &Description,
		Tizen::Base::String &Title) {
	StorageManager* store = StorageManager::getInstance();
	DbEnumerator* pEnum = 0;
	Database* db = BootstrapManager::getInstance()->getDatabase();
	result r = E_SUCCESS;

	AppLog(
			"Creating a new tracker with title [%ls] and description [%ls].", Title.GetPointer(), Description.GetPointer());
	__pDescription = new String(Description);
	__pTitle = new String(Title);
	__status = PAUSED;
	pEnum = store->CRUDoperation(this, I_CRUDable::CREATE);
	if (r != E_SUCCESS) {
		AppLogException(
				"Error storing the new tracker [%ls] in the database: [%ls]", __pTitle->GetPointer(), GetErrorMessage(r));
		return r;
	}
	//get the inserted ID using last_insert_rowid()
	__trackerId = db->GetLastInsertRowId();
	AppLog(
			"Successfully stored the new tracker [%ls] in the database with ID: [%d]", __pTitle->GetPointer(), __trackerId);
	delete pEnum;
	__pTrackPoints = new LinkedListT<TTLocation*>();
	return r;
}

//Constructs a new tracker using current field values, sets the status to paused and saves it to the database
result Tracker::Construct(void) {
	StorageManager* store = StorageManager::getInstance();
	DbEnumerator* pEnum = 0;
	Database* db = BootstrapManager::getInstance()->getDatabase();
	result r = E_SUCCESS;

	AppLog("Creating a new tracker from object.");

	__status = PAUSED;

	pEnum = store->CRUDoperation(this, I_CRUDable::CREATE);
	if (r != E_SUCCESS) {
		AppLogException(
				"Error storing the new tracker from object in the database: [%s]", GetErrorMessage(r));
		return r;
	}
	//get the inserted ID using last_insert_rowid()
	__trackerId = db->GetLastInsertRowId();
	AppLog(
			"Successfully stored the new tracker from object in the database with ID: [%d]", __trackerId);
	delete pEnum;
	__pTrackPoints = new LinkedListT<TTLocation*>();
	return r;
}

LinkedListT<TTLocation*>* Tracker::GetTrack(void) {
	return __pTrackPoints;
}

String* Tracker::GetDescription() const {
	return __pDescription;
}

void Tracker::SetDescription(String* description) {
	if (__pDescription != null)
		delete __pDescription;
	__pDescription = description;
}

String* Tracker::GetTitle() const {
	return __pTitle;
}

void Tracker::SetTitle(String* title) {
	if (__pTitle != null)
		delete __pTitle;
	__pTitle = title;
}

Tizen::Base::Collection::LinkedListT<TTLocation*>* Tracker::GetTrackPoints() const {
	return __pTrackPoints;
}

long long int Tracker::GetTrackerId() const {
	return __trackerId;
}

Tizen::Io::DbStatement* Tracker::Read(void) {
	String sqlStatement;
	DbStatement* pStmt = null;
	DbEnumerator* pEnum = null;
	Database* db;
	result r = E_SUCCESS;

	sqlStatement.Append(
			L"SELECT Description, Title, Distance, Status FROM Track WHERE ID = ?");

	db = BootstrapManager::getInstance()->getDatabase();
	pStmt = db->CreateStatementN(sqlStatement);
	AppLog(
			"Creating sql statement for SELECT for track with ID: [%d]", __trackerId);
	r = GetLastResult();
	if (r != E_SUCCESS) {
		AppLogException(
				"Error creating sql statement for SELECT for track with ID [%d]: [%s]", __trackerId, GetErrorMessage(r));
		return null;
	}
	AppAssert(pStmt);
	AppLog(
			"Sql SELECT statement created for track with ID: [%d]", __trackerId);

	r = pStmt->BindInt64(0, __trackerId);
	if (r != E_SUCCESS) {
		AppLogException(
				"Error binding __trackerId for SELECT for track with ID [%d]: [%s]", __trackerId, GetErrorMessage(r));
		return null;
	}

	double distance;
	String title, desc;
	int status;

	pEnum = db->ExecuteStatementN(*pStmt);
	//Description, Title, Distance, Status
	while (pEnum->MoveNext() == E_SUCCESS) {
		pEnum->GetStringAt(0, desc);
		pEnum->GetStringAt(1, title);
		pEnum->GetDoubleAt(2, distance);
		pEnum->GetIntAt(3, status);
	}

	AppLog(
			"Read values desc=[%S], title=[%S], distance=[%f], status=[%d]", desc.GetPointer(), title.GetPointer(), distance, status);

	SetDescription(new String(desc));
	SetTitle(new String(title));
	SetStatus(status);
	SetDistance(distance);

	//Now create the track point list from the database
	__pTrackPoints = StorageManager::getInstance()->GetLocations(__trackerId);

	delete pEnum;
	delete pStmt;

	AppLog("Successfully loaded data for track [%ls]", __pTitle->GetPointer());
	return null;

	/*String sqlStatement;
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
	 pStmt->BindInt64(0, __trackerId);
	 return pStmt;*/
}

Tizen::Io::DbStatement* Tracker::Write(void) {
	String sqlStatement;
	DbStatement* pStmt = null;
	Database* db;
	result r = E_SUCCESS;

	sqlStatement.Append(
			L"INSERT INTO Track (Description, Title, Distance, Status) VALUES (?,?,?,?)");

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
	pStmt->BindInt64(0, __trackerId);
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
	pStmt->BindInt64(4, __trackerId);
	return pStmt;
}

double Tracker::GetDistance() const {
	return __distance;
}

int Tracker::GetStatus() const {
	return __status;
}

LinkedListT<IFormFieldProvider::FormField*>* Tracker::GetFields(void) {
	LinkedListT<IFormFieldProvider::FormField*>* result = new LinkedListT<
			IFormFieldProvider::FormField*>();

	//TODO localize this
	IFormFieldProvider::FormField* pTitleField =
			new IFormFieldProvider::FormField();
	pTitleField->fieldName = String(L"Title");
	pTitleField->fieldData = String(GetTitle()->GetPointer());
	pTitleField->id = 1;
	pTitleField->limit = 255;
	pTitleField->fieldDim = Dimension(600, 80);

	result->Add(pTitleField);

	IFormFieldProvider::FormField* pDescField =
			new IFormFieldProvider::FormField();
	pDescField->fieldName = String(L"Description");
	pDescField->fieldData = String(GetDescription()->GetPointer());
	pDescField->id = 2;
	pDescField->limit = 1000;
	pDescField->fieldDim = Dimension(600, 400);

	result->Add(pDescField);

	return result;
}

result Tracker::SaveFields(LinkedListT<FormField*>* fieldList) {
	result r = E_SUCCESS;

	if (fieldList->GetCount() > 2)
		return E_INVALID_DATA;

	FormField *titleField, *descField;

	r = fieldList->GetAt(0, titleField);
	if (r != E_SUCCESS) {
		AppLogException(
				"Error getting title field from dialog: [%s]", GetErrorMessage(r));
		return r;
	}

	r = fieldList->GetAt(1, descField);
	if (r != E_SUCCESS) {
		AppLogException(
				"Error getting description field from dialog: [%s]", GetErrorMessage(r));
		return r;
	}

	if (__pTitle)
		delete __pTitle;
	__pTitle = new String(titleField->fieldData);

	if (__pDescription)
		delete __pDescription;
	__pDescription = new String(descField->fieldData);

	if (__trackerId < 0) {
		r = Construct();
		if (r != E_SUCCESS)
			AppLogException(
					"Error saving tracker from object: [%s]", GetErrorMessage(r));
		else {
			TrackerManager::getInstance()->GetTracks()->Add(this);
			AppLog("Successfully saved new tracker from object.");
		}
		SetStatus(ACTIVE);
		TrackerManager::getInstance()->SetCurrentTracker(this);
		return r;
	} else {
		StorageManager::getInstance()->CRUDoperation(this, I_CRUDable::UPDATE);
	}
}

int Tracker::GetFieldCount(void) {
	return 2;
}

void Tracker::SetDistance(double distance) {
	__distance = distance;
}

void Tracker::SetStatus(int status) {
	__status = status;
}

