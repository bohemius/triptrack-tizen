/*
 * StorageManager.cpp
 *
 *  Created on: Jul 21, 2013
 *      Author: hsp
 */

#include "dao/StorageManager.h"
#include "util/BootstrapManager.h"

using namespace Tizen::Base;
using namespace Tizen::Base::Collection;
using namespace Tizen::Io;

StorageManager* StorageManager::__pSelf = 0;

result StorageManager::Construct(Tizen::Io::Database* database) {
	return E_SUCCESS;
}

StorageManager* StorageManager::getInstance(void) {
	if (__pSelf == 0)
		__pSelf = new StorageManager();
	return __pSelf;
}

Tizen::Io::DbEnumerator* StorageManager::CRUDoperation(I_CRUDable* entity,
		enum I_CRUDable::CRUD_OP operation) {
	DbStatement* pStmt;

	switch (operation) {
	case I_CRUDable::CREATE:
		return PerformTransaction(entity->Write());
	case I_CRUDable::READ:
		return PerformTransaction(entity->Read());
	case I_CRUDable::UPDATE:
		return PerformTransaction(entity->Update());
	case I_CRUDable::DELETE:
		return PerformTransaction(entity->Delete());
	}
}

Tizen::Io::DbEnumerator* StorageManager::PerformTransaction(
		DbStatement* statement) {
	result r = E_SUCCESS;
	DbEnumerator* retVal = 0;
	Database* db = BootstrapManager::getInstance()->getDatabase();

	AppLog("Performing transaction with statement: [%d]", statement);
	db->BeginTransaction();
	retVal = db->ExecuteStatementN(*statement);
	db->CommitTransaction();
	if (retVal == 0 || r != E_SUCCESS) {
		AppLogException(
				"Error performing transaction with statement [%d]: [%s]", statement, GetErrorMessage(r));
		return 0;
	}
	AppLog("Transaction complete, received dbEnum: [%d]", retVal);
	delete statement;
	return retVal;
}

LinkedListT<POI*>* StorageManager::GetPois(void) {
}

LinkedListT<TTMedia*>* StorageManager::GetMedia(int poiId) {
}

LinkedListT<Tracker*>* StorageManager::GetTracks(void) {
	result r = E_SUCCESS;
	DbEnumerator* pEnum = 0;
	LinkedListT<Tracker*>* retVal = new LinkedListT<Tracker*>();
	String sql;
	int id;

	Database* db = BootstrapManager::getInstance()->getDatabase();

	sql.Append(L"SELECT ID FROM track");
	AppLog("Getting all track IDs from the database.");
	pEnum = db->QueryN(sql);

	r = GetLastResult();
	if (r != E_SUCCESS) {
		AppLogException(
				"Error getting track IDs from database: [%s]", GetErrorMessage(r));
		return 0;
	}
	AppLog("Creating collection of tracks.");

	while (pEnum->MoveNext() == E_SUCCESS) {
		r = pEnum->GetIntAt(0, id);
		if (r != E_SUCCESS) {
			AppLogException("Error getting track id: [%s]", GetErrorMessage(r));
			return 0;
		}
		Tracker* pTracker = new Tracker();
		r = pTracker->Construct(id);
		if (r != E_SUCCESS) {
			AppLogException(
					"Error constructing tracker with ID [%d]: [%s]", id, GetErrorMessage(r));
			return 0;
		}
		r = retVal->Add(pTracker);
		if (r != E_SUCCESS) {
			AppLogException(
					"Error adding tracker [%ls] to collection: [%s]", pTracker->GetTitle()->GetPointer(), GetErrorMessage(r));
			return 0;
		}
		AppLog(
				"Successfully added tracker [%ls] to collection.", pTracker->GetTitle()->GetPointer());
	}
	AppLog(
			"Successfully read and added [%d] tracks to collection.", retVal->GetCount());
	delete pEnum;
	return retVal;
}

LinkedListT<TTLocation*>* StorageManager::GetLocations(int trackerId) {
	result r = E_SUCCESS;
	DbEnumerator* pEnum = 0;
	DbStatement* pStmt = null;
	LinkedListT<TTLocation*>* retVal = new LinkedListT<TTLocation*>();
	String sql;
	int id;

	Database* db = BootstrapManager::getInstance()->getDatabase();

	sql.Append(L"SELECT ID FROM location WHERE tracker_ID = ?");
	AppLog(
			"Getting all location IDs for tracker_ID [%d] from the database.", trackerId);
	AppLog(
			"Creating SELECT statement for all location IDs for tracker_ID [%d]", trackerId);

	pStmt = db->CreateStatementN(sql);
	if (pStmt == 0 || r != E_SUCCESS) {
		AppLogException(
				"Error creating sql statement for SELECT for all location IDs for tracker_ID [%d]: [%s]", trackerId, GetErrorMessage(r));
		return 0;
	}
	AppLog(
			"Sql SELECT statement created for all location IDs for tracker_ID [%d]", trackerId);
	pStmt->BindInt(0, trackerId);
	pEnum = db->ExecuteStatementN(*pStmt);

	r = GetLastResult();
	if (pEnum ==0 || r != E_SUCCESS) {
		AppLogException(
				"Error getting all location IDs for tracker ID [%d] from database: [%s]", trackerId, GetErrorMessage(r));
		return 0;
	}
	AppLog("Creating collection of locations for track ID [%d].",trackerId);

	while (pEnum->MoveNext() == E_SUCCESS) {
		r = pEnum->GetIntAt(0, id);
		if (r != E_SUCCESS) {
			AppLogException("Error getting location id: [%s]", GetErrorMessage(r));
			return 0;
		}
		TTLocation* pLoc = new TTLocation();
		r = pLoc->Construct(id);
		if (r != E_SUCCESS) {
			AppLogException(
					"Error constructing location with ID [%d]: [%s]", id, GetErrorMessage(r));
			return 0;
		}
		r = retVal->Add(pLoc);
		if (r != E_SUCCESS) {
			AppLogException(
					"Error adding location with id [%d] to collection: [%s]", id, GetErrorMessage(r));
			return 0;
		}
		AppLog(
				"Successfully added location with id [%d] to collection.", id);
	}
	AppLog(
			"Successfully read and added [%d] locations to collection.", retVal->GetCount());
	delete pEnum;
	delete pStmt;
	return retVal;
}

StorageManager::StorageManager() {
}

StorageManager::~StorageManager() {
	delete __pSelf;
}

