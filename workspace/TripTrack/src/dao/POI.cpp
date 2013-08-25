/*
 * POI.cpp
 *
 *  Created on: Jul 30, 2013
 *      Author: bohemius
 */

#include <FGraphics.h>
#include <FSystem.h>
#include "dao/POI.h"
#include "geo/Tracker.h"
#include "util/BootstrapManager.h"
#include "util/GraphicsUtils.h"
#include "dao/StorageManager.h"

using namespace Tizen::Locations;
using namespace Tizen::Base::Collection;
using namespace Tizen::Base;
using namespace Tizen::Io;
using namespace Tizen::Graphics;
using namespace Tizen::System;

POI::POI() {
	__pCoordinates = new Coordinates();
	__pTimestamp = new DateTime();
}

POI::~POI() {
	// TODO Auto-generated destructor stub
}

long long int POI::GetDefImageId() const {
	return __defImageId;
}

void POI::SetDefImageId(long long int defImageId) {
	__defImageId = defImageId;
	//now that the default media id is set update the POI
	StorageManager::getInstance()->CRUDoperation(this, I_CRUDable::UPDATE);
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

Coordinates* POI::GetLocation() const {
	return __pCoordinates;
}

void POI::SetLocation(Coordinates* location) {
	__pCoordinates = location;
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
	StorageManager* pStore = StorageManager::getInstance();
	pStore->CRUDoperation(media, I_CRUDable::DELETE);
	__pAssociatedMedia->Remove(media);
}

result POI::Construct(long long int id) {
	StorageManager* store = StorageManager::getInstance();
	DbEnumerator* pEnum = 0;
	result r = E_SUCCESS;

	AppLog("Reading poi data from database for poi id: [%d]", id);
	__id = id;
	pEnum = store->CRUDoperation(this, I_CRUDable::READ);

	double latitude, longitude, altitude;
	String dateString;

	//Description, Title, Latitude, Longitude, Altitude, Timestamp, DefaultMediaID
	//while (pEnum->MoveNext() == E_SUCCESS) {
	r = pEnum->MoveNext();
	if (r != E_SUCCESS)
		AppLogException("Error moving next: [%s]", GetErrorMessage(r));
	r = pEnum->GetStringAt(0, *__pDescription);
	if (r != E_SUCCESS)
		AppLogException("Error getting string@0: [%s]", GetErrorMessage(r));
	pEnum->GetStringAt(1, *__pTitle);
	pEnum->GetDoubleAt(2, latitude);
	pEnum->GetDoubleAt(3, longitude);
	pEnum->GetDoubleAt(4, altitude);
	pEnum->GetStringAt(5, dateString);
	pEnum->GetInt64At(6, __defImageId);
	//}

	__pCoordinates->Set(latitude, longitude, altitude);
	r = DateTime::Parse(dateString, *__pTimestamp);
	if (r != E_SUCCESS) {
		AppLogException(
				"Error parsing timestamp for poi with id [%d]: [%s]", __id, GetErrorMessage(r));
		return r;
	}
	__pAssociatedMedia = StorageManager::getInstance()->GetMedia(__id);
	AppLog("Successfully loaded data for poi [%ls]", __pTitle->GetPointer());

	delete pEnum;
	return r;
}

result POI::Construct(Tizen::Base::String& Title,
		Tizen::Base::String& Description, Location location) {
	StorageManager* store = StorageManager::getInstance();
	DbEnumerator* pEnum = 0;
	Database* db = BootstrapManager::getInstance()->getDatabase();
	result r = E_SUCCESS;

	AppLog( "Creating a new poi [%ls].", Title.GetPointer());

	__pTimestamp->SetValue(location.GetTimestamp());
	r = __pCoordinates->Set(location.GetCoordinates().GetLatitude(),
			location.GetCoordinates().GetLongitude(),
			location.GetCoordinates().GetAltitude());
	if (r != E_SUCCESS) {
		AppLogException(
				"Error setting coordinates, looks like cannot get a valid location using home location:[%s] ", GetErrorMessage(r));
		//TODO do the home location in the settings
		__pCoordinates->Set(0.0, 0.0, 0.0);
		//use current system time
		SystemTime::GetCurrentTime(*__pTimestamp);
		r = E_SUCCESS;
	}
	__pTitle = &Title;
	__pDescription = &Description;

	pEnum = store->CRUDoperation(this, I_CRUDable::CREATE);
	//get the inserted ID using last_insert_rowid()
	__id = db->GetLastInsertRowId();
	if (r != E_SUCCESS || __id < 0) {
		AppLogException(
				"Error storing the new poi [%ls] in the database: [%s]", __pTitle->GetPointer(), GetErrorMessage(r));
		return r;
	}

	AppLog(
			"Successfully stored the new poi [%ls] in the database with ID: [%d]", __pTitle->GetPointer(), __id);
	delete pEnum;
	__pAssociatedMedia = new LinkedListT<TTMedia*>();
	return r;
}

result POI::Construct(Tizen::Base::String& Title,
		Tizen::Base::String& Description, Tizen::Locations::Location location,
		Tizen::Base::String& SourceUri) {

	result r = E_SUCCESS;

	//create poi using supplied parameters
	r = Construct(Title, Description, location);
	if (r != E_SUCCESS)
		AppLogException(
				"Error creating poi  [%ls]: [%s]", __pTitle->GetPointer(), GetErrorMessage(r));

	//process the camera image and create a tile
	Bitmap* pCapturedBitmap = GraphicsUtils::CreateBitmap(SourceUri);
	r = pCapturedBitmap->Scale(
			FloatDimension(TILE_IMAGE_WIDTH, TILE_IMAGE_HEIGHT));
	if (r != E_SUCCESS)
		AppLogException(
				"Bitmap [%ls] scaling failed: [%s]", SourceUri.GetPointer(), GetErrorMessage(r));
	ByteBuffer* pImgBuffer = GraphicsUtils::CreateImageBuffer(pCapturedBitmap);

	//create the media using the image
	TTMedia* pDefMedia = new TTMedia();
	r = pDefMedia->Construct(SourceUri, __id, pImgBuffer);
	if (r != E_SUCCESS) {
		AppLogException(
				"Error storing the new media for poiId [%d] in the database: [%s]", __id, GetErrorMessage(r));
		return r;
	}
	AddMedia(pDefMedia);
	SetDefImageId(pDefMedia->GetId());

	return r;
}

long long int POI::GetId() const {
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
	r = GetLastResult();
	if (r != E_SUCCESS) {
		AppLogException(
				"Error creating sql statement for SELECT for poi with ID [%d]: [%s]", __id, GetErrorMessage(r));
		return 0;
	}
	AppAssert(pStmt);
	AppLog( "Sql SELECT statement created for poi with ID: [%d]", __id);

	r = pStmt->BindInt64(0, __id);
	if (r != E_SUCCESS) {
		AppLogException(
				"Error binding __id for SELECT for poi with ID [%d]: [%s]", __id, GetErrorMessage(r));
		return 0;
	}
	return pStmt;
}

Tizen::Io::DbStatement* POI::Write(void) {
	String sqlStatement;
	DbStatement* pStmt = null;
	Database* db;
	result r = E_SUCCESS;

	sqlStatement.Append(
			L"INSERT INTO poi (Description, Title, Latitude, Longitude, Altitude, Timestamp, DefaultMediaID) VALUES (?,?,?,?,?,?,?)");

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
	pStmt->BindDouble(2, __pCoordinates->GetLatitude());
	pStmt->BindDouble(3, __pCoordinates->GetLongitude());
	pStmt->BindDouble(4, __pCoordinates->GetAltitude());

	String dateString = __pTimestamp->ToString();
	pStmt->BindString(5, dateString);
	pStmt->BindInt64(6, __defImageId);
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
	pStmt->BindInt64(0, __id);
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
	pStmt->BindDouble(2, __pCoordinates->GetLatitude());
	pStmt->BindDouble(3, __pCoordinates->GetLongitude());
	pStmt->BindDouble(4, __pCoordinates->GetAltitude());

	String dateString = __pTimestamp->ToString();
	pStmt->BindString(5, dateString);
	pStmt->BindInt64(6, __defImageId);
	pStmt->BindInt64(7, __id);

	return pStmt;
}

Tizen::Base::DateTime* POI::GetTimestamp() const {
	return __pTimestamp;
}

void POI::SetTimestamp(Tizen::Base::DateTime* timestamp) {
	__pTimestamp = timestamp;
}

