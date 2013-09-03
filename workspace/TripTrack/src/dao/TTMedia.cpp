/*
 * TTMedia.cpp
 *
 *  Created on: Jul 30, 2013
 *      Author: bohemius
 */
#include "dao/TTMedia.h"
#include "util/BootstrapManager.h"
#include "dao/StorageManager.h"

using namespace Tizen::Base;
using namespace Tizen::Io;

TTMedia::TTMedia() :
		__pContent(null), __pSourceUri(null) {
	// TODO Auto-generated constructor stub

}

TTMedia::~TTMedia() {
	// TODO Auto-generated destructor stub
}

long long int TTMedia::GetId() const {
	return __id;
}

Tizen::Base::ByteBuffer* TTMedia::GetContent() const {
	return __pContent;
}

void TTMedia::SetContent(Tizen::Base::ByteBuffer* content) {
	__pContent = content;
}

Tizen::Base::String* TTMedia::GetSourceUri() const {
	return __pSourceUri;
}

result TTMedia::Construct(long long int id) {
	__id = id;
	AppLog("Reading media data from database for media id: [%lld]", __id);
	Read();

	AppLog(
			"Constructed media with values source=[%S], content size limit=[%d], poi Id=[%lld], media Id=[%lld]",
			__pSourceUri->GetPointer(),
			__pContent->GetLimit(),
			__poiId,
			__id);

	return E_SUCCESS;
}

result TTMedia::Construct(Tizen::Base::String& SourceURI, long long int PoiId,
		ByteBuffer* buffer) {
	StorageManager* store = StorageManager::getInstance();
	DbEnumerator* pEnum = 0;
	Database* db = BootstrapManager::getInstance()->getDatabase();
	result r = E_SUCCESS;

	__pSourceUri = &SourceURI;
	AppLog(
			"Creating a new media with source URI [%ls].", __pSourceUri->GetPointer());

	__poiId = PoiId;
	__pContent = buffer;
	//__pContent->CopyFrom(*buffer);
	pEnum = store->CRUDoperation(this, I_CRUDable::CREATE);
	r = GetLastResult();
	if (r != E_SUCCESS) {
		AppLogException(
				"Error storing the new media with source URI [%ls] in the database: [%s]", __pSourceUri->GetPointer(), GetErrorMessage(r));
		return r;
	}
	//get the inserted ID using last_insert_rowid()
	__id = db->GetLastInsertRowId();

	AppLog(
			"Successfully stored the new media with source URI [%ls] in the database with ID: [%d]", __pSourceUri->GetPointer(), __id);
	delete pEnum;
	return r;
}

Tizen::Io::DbStatement* TTMedia::Read(void) {
	String sqlStatement;
	DbStatement* pStmt = null;
	DbEnumerator* pEnum = null;
	Database* db;
	result r = E_SUCCESS;

	sqlStatement.Append(
			L"SELECT source,thumbnail, POI_ID FROM media WHERE ID = ?");

	db = BootstrapManager::getInstance()->getDatabase();
	pStmt = db->CreateStatementN(sqlStatement);
	AppLog( "Creating sql statement for SELECT for media with ID: [%d]", __id);
	r = GetLastResult();
	if (r != E_SUCCESS) {
		AppLogException(
				"Error creating sql statement for SELECT for media with ID [%d]: [%s]", __id, GetErrorMessage(r));
		return null;
	}
	AppAssert(pStmt);
	AppLog( "Sql SELECT statement created for media with ID: [%d]", __id);

	r = pStmt->BindInt64(0, __id);
	if (r != E_SUCCESS) {
		AppLogException(
				"Error binding __id for SELECT for media with ID [%d]: [%s]", __id, GetErrorMessage(r));
		return null;
	}

	String sourcePath;
	long long int poiId;
	ByteBuffer content;

	pEnum = db->ExecuteStatementN(*pStmt);
	//source,thumbnail, POI_ID
	while (pEnum->MoveNext() == E_SUCCESS) {
		pEnum->GetStringAt(0, sourcePath);
		pEnum->GetBlobAt(1, content);
		pEnum->GetInt64At(2, poiId);
	}

	AppLog(
			"Read values sourcePath=[%S], content size=[%d], poiId=[%lld]",
			sourcePath.GetPointer(), content.GetLimit(), __poiId);

	SetSourceUri(&sourcePath);
	SetContent(&content);
	SetPoiId(poiId);

	delete pEnum;
	delete pStmt;

	AppLog("Successfully loaded data for media with id:[%lld]", __id);
	return null;
	/*String sqlStatement;
	 DbStatement* pStmt = null;
	 Database* db;
	 result r = E_SUCCESS;

	 sqlStatement.Append(
	 L"SELECT source,thumbnail, POI_ID FROM media WHERE ID = ?");

	 db = BootstrapManager::getInstance()->getDatabase();
	 pStmt = db->CreateStatementN(sqlStatement);
	 AppLog( "Creating sql statement for SELECT for media with ID: [%d]", __id);
	 if (pStmt == 0 || r != E_SUCCESS) {
	 AppLogException(
	 "Error creating sql statement for SELECT for media with ID [%d]: [%s]", __id, GetErrorMessage(r));
	 return 0;
	 }
	 AppLog( "Sql SELECT statement created for media with ID: [%d]", __id);
	 pStmt->BindInt(0, __id);
	 return pStmt;*/
}

Tizen::Io::DbStatement* TTMedia::Write(void) {
	String sqlStatement;
	DbStatement* pStmt = null;
	Database* db;
	result r = E_SUCCESS;

	sqlStatement.Append(
			L"INSERT INTO media (source, thumbnail, POI_ID) VALUES (?,?,?)");

	db = BootstrapManager::getInstance()->getDatabase();
	pStmt = db->CreateStatementN(sqlStatement);
	AppLog(
			"Creating sql statement for INSERT for media with source URI [%ls]", __pSourceUri->GetPointer());
	if (pStmt == 0 || r != E_SUCCESS) {
		AppLogException(
				"Error creating sql statement for INSERT for media with URI [%ls]: [%s]", __pSourceUri->GetPointer(), GetErrorMessage(r));
		return 0;
	}
	AppLog(
			"Sql INSERT statement created for media with source URI [%ls]", __pSourceUri->GetPointer());
	pStmt->BindString(0, *__pSourceUri);
	pStmt->BindBlob(1, *__pContent);
	pStmt->BindInt64(2, __poiId);
	return pStmt;
}

Tizen::Io::DbStatement* TTMedia::Delete(void) {
	String sqlStatement;
	DbStatement* pStmt = null;
	Database* db;
	result r = E_SUCCESS;

	sqlStatement.Append(L"DELETE FROM media WHERE ID = ?");

	db = BootstrapManager::getInstance()->getDatabase();
	pStmt = db->CreateStatementN(sqlStatement);
	AppLog(
			"Creating DELETE statement for media with source URI: [%ls]", __pSourceUri->GetPointer());
	if (pStmt == 0 || r != E_SUCCESS) {
		AppLogException(
				"Error creating sql statement for DELETE for media with source URI [%ls]: [%s]", __pSourceUri->GetPointer(), GetErrorMessage(r));
		return 0;
	}
	AppLog(
			"Sql DELETE statement created for media with source URI [%ls]", __pSourceUri->GetPointer());
	pStmt->BindInt64(0, __id);
	return pStmt;
}

Tizen::Io::DbStatement* TTMedia::Update(void) {
	String sqlStatement;
	DbStatement* pStmt = null;
	Database* db;
	result r = E_SUCCESS;

	sqlStatement.Append(
			L"UPDATE media SET  source = ?, thumbnail = ?, POI_ID = ? WHERE ID = ?");

	db = BootstrapManager::getInstance()->getDatabase();
	pStmt = db->CreateStatementN(sqlStatement);
	AppLog(
			"Creating sql statement for UPDATE for media with source URI [%ls]", __pSourceUri->GetPointer());
	if (pStmt == 0 || r != E_SUCCESS) {
		AppLogException(
				"Error creating sql statement for UPDATE for media with source URI [%ls]: [%s]", __pSourceUri->GetPointer(), GetErrorMessage(r));
		return 0;
	}
	AppLog(
			"Sql UPDATE statement created for media with source URI [%ls]", __pSourceUri->GetPointer());

	pStmt->BindString(0, *__pSourceUri);
	pStmt->BindBlob(1, *__pContent);
	pStmt->BindInt64(2, __poiId);
	pStmt->BindInt64(3, __id);
	return pStmt;
}

void TTMedia::SetSourceUri(Tizen::Base::String* sourceUri) {
	__pSourceUri = sourceUri;
}

long long int TTMedia::GetPoiId() const {
	return __poiId;
}

void TTMedia::SetPoiId(long long int poiId) {
	__poiId = poiId;
}

