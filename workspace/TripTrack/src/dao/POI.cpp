/*
 * POI.cpp
 *
 *  Created on: Jul 30, 2013
 *      Author: bohemius
 */

#include "dao/POI.h"
#include "geo/Tracker.h"
#include "util/BootstrapManager.h"
#include "dao/StorageManager.h"

using namespace Tizen::Locations;
using namespace Tizen::Base::Collection;
using namespace Tizen::Base;
using namespace Tizen::Io;

POI::POI() {
	// TODO Auto-generated constructor stub

}

POI::~POI() {
	// TODO Auto-generated destructor stub
}

int POI::GetDefImageId() const {
	return __defImageId;
}

void POI::SetDefImageId(int defImageId) {
	__defImageId = defImageId;
}

Tizen::Base::Collection::LinkedListT<TTMedia*>* POI::GetAssociatedMedia() const {
	return __pAssociatedMedia;
}

Tizen::Base::String* POI::GetDescription() const {
	return __pDescription;
}

void POI::SetDescription(Tizen::Base::String* description) {
	__pDescription = description;
}

TTLocation* POI::GetLocation() const {
	return __pLocation;
}

void POI::SetLocation(TTLocation* location) {
	__pLocation = location;
}

Tizen::Base::String* POI::GetTitle() const {
	return __pTitle;
}

void POI::SetTitle(Tizen::Base::String* title) {
	__pTitle = title;
}

void POI::AddMedia(TTMedia* media) {
	__pAssociatedMedia->Add(media);
}

void POI::DeleteMedia(TTMedia* media) {
	__pAssociatedMedia->Remove(media);
}

result POI::Construct(int id) {
}

result POI::Construct(Tizen::Base::String& Title,
		Tizen::Base::String& Description) {
}

int POI::GetId() const {
	return __id;
}

Tizen::Io::DbStatement* POI::Read(void) {
	String sqlStatement;
	DbStatement* pStmt = null;
	Database* db;
	result r = E_SUCCESS;

	sqlStatement.Append(
			L"SELECT Description, Title, Latitude, Longitude, Altitude, Timestamp, DefaultMediaID FROM poi WHERE ID = ?");

	db = BootstrapManager::getInstance()->getDatabase();
	pStmt = db->CreateStatementN(sqlStatement);
	AppLog( "Creating sql statement for SELECT for poi with ID: [%d]", __id);
	if (pStmt == 0 || r != E_SUCCESS) {
		AppLogException(
				"Error creating sql statement for SELECT for poi with ID [%d]: [%s]", __id, GetErrorMessage(r));
		return 0;
	}
	AppLog( "Sql SELECT statement created for poi with ID: [%d]", __id);
	pStmt->BindInt(0, __id);
	return pStmt;
}

Tizen::Io::DbStatement* POI::Write(void) {
	String sqlStatement;
	DbStatement* pStmt = null;
	Database* db;
	result r = E_SUCCESS;

	sqlStatement.Append(
			L"INSERT INTO poi (Description, Title, Latitude, Longitude, Altitude, Timestamp, DefaultMediaID) VALUES (?,?,?,?,?,?,?); SELECT last_insert_rowid();");

	db = BootstrapManager::getInstance()->getDatabase();
	pStmt = db->CreateStatementN(sqlStatement);
	AppLog(
			"Creating sql statement for INSERT for poi: [%ls]", __pTitle->GetPointer());
	if (pStmt == 0 || r != E_SUCCESS) {
		AppLogException(
				"Error creating sql statement for INSERT for poi [%ls]: [%s]", __pTitle->GetPointer(), GetErrorMessage(r));
		return 0;
	}
	AppLog(
			"Sql INSERT statement created for poi: [%ls]", __pTitle->GetPointer());
	pStmt->BindString(0, *__pDescription);
	pStmt->BindString(1, *__pTitle);
	pStmt->BindDouble(2, __pLocation->getLatitude());
	pStmt->BindDouble(3, __pLocation->getLongitude());
	pStmt->BindDouble(4, __pLocation->getAltitude());

	String dateString = __pTimestamp->ToString();
	pStmt->BindString(5, dateString);
	pStmt->BindInt(6, __defImageId);
	return pStmt;
}

Tizen::Io::DbStatement* POI::Delete(void) {
	String sqlStatement;
	DbStatement* pStmt = null;
	Database* db;
	result r = E_SUCCESS;

	sqlStatement.Append(L"DELETE FROM poi WHERE ID = ?");

	db = BootstrapManager::getInstance()->getDatabase();
	pStmt = db->CreateStatementN(sqlStatement);
	AppLog( "Creating DELETE statement for poi: [%ls]", __pTitle->GetPointer());
	if (pStmt == 0 || r != E_SUCCESS) {
		AppLogException(
				"Error creating sql statement for DELETE for poi [%ls]: [%s]", __pTitle->GetPointer(), GetErrorMessage(r));
		return 0;
	}
	AppLog(
			"Sql DELETE statement created for poi: [%ls]", __pTitle->GetPointer());
	pStmt->BindInt(0, __id);
	return pStmt;
}

Tizen::Io::DbStatement* POI::Update(void) {
	String sqlStatement;
	DbStatement* pStmt = null;
	Database* db;
	result r = E_SUCCESS;

	sqlStatement.Append(
			L"UPDATE poi SET Description = ?, Title = ?, Latitude = ?, Longitude = ?, Altitude = ?, Timestamp = ?, DefaultMediaID = ? WHERE ID = ?");

	db = BootstrapManager::getInstance()->getDatabase();
	pStmt = db->CreateStatementN(sqlStatement);
	AppLog(
			"Creating sql statement for UPDATE for poi: [%ls]", __pTitle->GetPointer());
	if (pStmt == 0 || r != E_SUCCESS) {
		AppLogException(
				"Error creating sql statement for UPDATE for poi [%ls]: [%s]", __pTitle->GetPointer(), GetErrorMessage(r));
		return 0;
	}
	AppLog(
			"Sql UPDATE statement created for poi: [%ls]", __pTitle->GetPointer());
	pStmt->BindString(0, *__pDescription);
	pStmt->BindString(1, *__pTitle);
	pStmt->BindDouble(2, __pLocation->getLatitude());
	pStmt->BindDouble(3, __pLocation->getLongitude());
	pStmt->BindDouble(4, __pLocation->getAltitude());

	String dateString=__pTimestamp->ToString();
	pStmt->BindString(5, dateString);
	pStmt->BindInt(6, __defImageId);
	pStmt->BindInt(7,__id);

	return pStmt;
}

Tizen::Base::DateTime* POI::GetTimestamp() const {
	return __pTimestamp;
}

void POI::SetTimestamp(Tizen::Base::DateTime* timestamp) {
	__pTimestamp = timestamp;
}

