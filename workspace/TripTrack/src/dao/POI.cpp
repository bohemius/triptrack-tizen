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
using namespace Tizen::App;

POI::POI() :
		__pDescription(null), __pTitle(null), __pTimestamp(null), __pCoordinates(
				null), __defImageId(-1) {
}

POI::~POI() {
	// TODO Auto-generated destructor stub
}

long long int POI::GetDefImageId() const {
	return __defImageId;
}

void POI::SetDefImageId(long long int defImageId) {
	__defImageId = defImageId;
}

Tizen::Base::Collection::LinkedListT<TTMedia*>* POI::GetAssociatedMedia() const {
	return __pAssociatedMedia;
}

Tizen::Base::String* POI::GetDescription() const {
	return __pDescription;
}

void POI::SetDescription(Tizen::Base::String* description) {
	if (__pDescription != null)
		delete __pDescription;
	__pDescription = description;
}

Coordinates* POI::GetLocation() const {
	return __pCoordinates;
}

void POI::SetLocation(Coordinates* location) {
	if (__pCoordinates != null)
		delete __pCoordinates;
	__pCoordinates = location;
}

Tizen::Base::String* POI::GetTitle() const {
	return __pTitle;
}

void POI::SetTitle(Tizen::Base::String* title) {
	if (__pTitle != null)
		delete __pTitle;
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
	__id = id;
	AppLog("Reading poi data from database for poi id: [%d]", __id);
	Read();

	AppLog(
			"Constructed poi with values desc=[%S], title=[%S], latitude=[%f], longitude=[%f], altitude=[%f], time=[%S], defImgId=[%lld], id=[%lld]", __pDescription->GetPointer(), __pTitle->GetPointer(), __pCoordinates->GetLatitude(), __pCoordinates->GetLongitude(), __pCoordinates->GetAltitude(), __pTimestamp->ToString().GetPointer(), GetDefImageId(), GetId());

	return E_SUCCESS;
}

result POI::Construct(Tizen::Base::String& Title,
		Tizen::Base::String& Description, Location location) {
	StorageManager* store = StorageManager::getInstance();
	DbEnumerator* pEnum = 0;
	Database* db = BootstrapManager::getInstance()->getDatabase();
	result r = E_SUCCESS;

	AppLog(
			"Creating a new poi [%ls] with location [%f],[%f] and date [%ls].", Title.GetPointer(), location.GetCoordinates().GetLongitude(), location.GetCoordinates().GetLatitude(), location.GetTimestamp().ToString().GetPointer());

	SetTimestamp(new DateTime(location.GetTimestamp()));
	Coordinates coor;
	DateTime poiTime;
	//__pTimestamp->SetValue(location.GetTimestamp());
	// r = __pCoordinates->Set(location.GetCoordinates().GetLatitude(),
	if (location.IsValid()) {
		coor = location.GetCoordinates();
		poiTime = location.GetTimestamp();
	} else {
		Location lastLocation = LocationProvider::GetLastKnownLocation();
		if (!lastLocation.IsValid()) {
			AppLogException(
					"Error getting valid location using home coordinates:[%s] ", GetErrorMessage(r));
			//TODO do the home location in the settings
			coor.Set(0.0, 0.0, 0.0);
			//use current system time
			SystemTime::GetCurrentTime(poiTime);

		} else {
			coor = lastLocation.GetCoordinates();
			poiTime = lastLocation.GetTimestamp();
		}
	}
	SetLocation(new Coordinates(coor));
	SetTitle(new String(Title));
	SetDescription(new String(Description));
	SetTimestamp(new DateTime(poiTime));

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
	StorageManager::getInstance()->CRUDoperation(this, I_CRUDable::UPDATE);

	return r;
}

long long int POI::GetId() const {
	return __id;
}

Tizen::Io::DbStatement* POI::Read(void) {
	String sqlStatement;
	DbStatement* pStmt = null;
	DbEnumerator* pEnum = null;
	Database* db;
	result r = E_SUCCESS;

	sqlStatement.Append(
			L"SELECT Description, Title, Latitude, Longitude, Altitude, TimeSig, DefaultMediaID FROM poi WHERE ID = ?");

	db = BootstrapManager::getInstance()->getDatabase();
	pStmt = db->CreateStatementN(sqlStatement);
	AppLog( "Creating sql statement for SELECT for poi with ID: [%d]", __id);
	r = GetLastResult();
	if (r != E_SUCCESS) {
		AppLogException(
				"Error creating sql statement for SELECT for poi with ID [%d]: [%s]", __id, GetErrorMessage(r));
		return null;
	}
	AppAssert(pStmt);
	AppLog( "Sql SELECT statement created for poi with ID: [%d]", __id);

	r = pStmt->BindInt64(0, __id);
	if (r != E_SUCCESS) {
		AppLogException(
				"Error binding __id for SELECT for poi with ID [%d]: [%s]", __id, GetErrorMessage(r));
		return null;
	}
	double latitude, longitude, altitude;
	long long int defId;
	String dateString, title, desc;
	DateTime timeStamp;

	pEnum = db->ExecuteStatementN(*pStmt);
	//Description, Title, Latitude, Longitude, Altitude, Timestamp, DefaultMediaID
	while (pEnum->MoveNext() == E_SUCCESS) {
		pEnum->GetStringAt(0, desc);
		pEnum->GetStringAt(1, title);
		pEnum->GetDoubleAt(2, latitude);
		pEnum->GetDoubleAt(3, longitude);
		pEnum->GetDoubleAt(4, altitude);
		pEnum->GetStringAt(5, dateString);
		pEnum->GetInt64At(6, defId);
	}

	Coordinates* pCoor = new Coordinates();
	pCoor->Set(latitude, longitude, altitude);

	r = DateTime::Parse(dateString, timeStamp);
	if (r != E_SUCCESS) {
		AppLogException(
				"Error parsing timestamp for poi with id [%d]: [%s]", __id, GetErrorMessage(r));
		return null;
	}
	AppLog(
			"Read values desc=[%S], title=[%S], latitude=[%f], longitutde=[%f], altitude=[%f]. time=[%S], defImgId=[%lld]", desc.GetPointer(), title.GetPointer(), latitude, longitude, altitude, dateString.GetPointer(), defId);

	SetDescription(new String(desc));
	SetTitle(new String(title));
	SetTimestamp(new DateTime(timeStamp));
	SetLocation(pCoor);
	SetDefImageId(defId);

	delete pEnum;
	delete pStmt;

	AppLog("Successfully loaded data for poi [%ls]", __pTitle->GetPointer());
	return null;
	/*String sqlStatement;
	 DbStatement* pStmt = null;
	 Database* db;
	 result r = E_SUCCESS;

	 sqlStatement.Append(
	 L"SELECT Description, Title, Latitude, Longitude, Altitude, TimeSig, DefaultMediaID FROM poi WHERE ID = ?");

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
	 return pStmt;*/
}

Tizen::Io::DbStatement* POI::Write(void) {
	String sqlStatement;
	DbStatement* pStmt = null;
	Database* db;
	result r = E_SUCCESS;

	sqlStatement.Append(
			L"INSERT INTO poi (Description, Title, Latitude, Longitude, Altitude, TimeSig, DefaultMediaID) VALUES (?,?,?,?,?,?,?)");

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
			L"UPDATE poi SET Description = ?, Title = ?, Latitude = ?, Longitude = ?, Altitude = ?, TimeSig = ?, DefaultMediaID = ? WHERE ID = ?");

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
	if (__pTimestamp != null)
		delete __pTimestamp;
	__pTimestamp = timestamp;
}

