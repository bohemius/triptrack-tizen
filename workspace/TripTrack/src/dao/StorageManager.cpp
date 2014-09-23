/*
 * StorageManager.cpp
 *
 *  Created on: Jul 21, 2013
 *      Author: hsp
 */

#include "dao/StorageManager.h"
#include "dao/TTMedia.h"
#include "util/BootstrapManager.h"

using namespace Tizen::Base;
using namespace Tizen::Base::Collection;
using namespace Tizen::Io;

StorageManager* StorageManager::__pSelf = 0;

result StorageManager::Construct(Tizen::Io::Database* database) {
	return E_SUCCESS;
}

StorageManager* StorageManager::getInstance(void) {
	if (__pSelf == 0) {
		__pSelf = new StorageManager();
	}
	return __pSelf;
}

Tizen::Io::DbEnumerator* StorageManager::CRUDoperation(I_CRUDable* entity,
		enum I_CRUDable::CRUD_OP operation) {

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
	DbEnumerator* retVal = null;
	Database* db = BootstrapManager::getInstance()->getDatabase();

	AppLog("Performing transaction with statement: [%d]", statement);
	db->BeginTransaction();
	retVal = db->ExecuteStatementN(*statement);
	r = db->CommitTransaction();
	if (r != E_SUCCESS) {
		AppLogException(
				"Error performing transaction with statement [%d]: [%s]", statement, GetErrorMessage(r));
		return 0;
	}
	AppLog("Transaction complete, received dbEnum: [%d]", retVal);
	delete statement;
	//r = retVal->MoveNext();
	//AppLog("move next result: [%s]",GetErrorMessage(r));
	return retVal;
}

LinkedListT<POI*>* StorageManager::GetPois(void) {
	result r = E_SUCCESS;
	DbEnumerator* pEnum = null;
	LinkedListT<POI*>* retVal = new LinkedListT<POI*>();
	String sql;

	Database* db = BootstrapManager::getInstance()->getDatabase();

	sql.Append(L"SELECT ID FROM poi");
	AppLog("Getting all poi IDs from the database.");
	pEnum = db->QueryN(sql);

	r = GetLastResult();
	if (r != E_SUCCESS) {
		AppLogException(
				"Error getting poi IDs from database: [%s]", GetErrorMessage(r));
		return null;
	}

	if (pEnum != null) {

		LongLongBuffer poiBuffer;
		poiBuffer.Construct(1024);
		long long int count = 0;
		long long int id;

		while (pEnum->MoveNext() == E_SUCCESS) {
			r = pEnum->GetInt64At(0, id);
			if (r != E_SUCCESS) {
				AppLogException(
						"Error getting poi id: [%s]", GetErrorMessage(r));
				return 0;
			}
			poiBuffer.Set(id);
			count++;
		}

		delete pEnum;
		poiBuffer.Rewind();

		AppLog("Creating collection of pois.");
		while (poiBuffer.GetPosition() < count) {
			POI* pPoi = new POI();
			r = poiBuffer.Get(id);
			r = pPoi->Construct(id);
			if (r != E_SUCCESS) {
				AppLogException(
						"Error constructing poi with ID [%d]: [%s]", id, GetErrorMessage(r));
				return 0;
			}
			r = retVal->Add(pPoi);
			if (r != E_SUCCESS) {
				AppLogException(
						"Error adding poi [%ls] to collection: [%s]", pPoi->GetTitle()->GetPointer(), GetErrorMessage(r));
				return 0;
			}
			AppLog(
					"Successfully added poi [%ls] to collection.", pPoi->GetTitle()->GetPointer());
		}
	} else
		AppLog("No POIs in the database, returning empty collection.");

	AppLog(
			"Successfully read and added [%d] pois to collection.", retVal->GetCount());

	return retVal;
}

Tizen::Base::Collection::HashMapT<long long int, LinkedListT<POI*>*>* StorageManager::GetPoiHash(
		void) {
	HashMapT<long long int, LinkedListT<POI*>*>* retVal = new HashMapT<
			long long int, LinkedListT<POI*>*>();
	retVal->Construct(10, 10);
	LinkedListT<POI*>* pCollection = GetPois();

	IEnumeratorT<POI*>* pEnum = pCollection->GetEnumeratorN();

	int count = 0;
	while (pEnum->MoveNext() == E_SUCCESS) {
		POI* pPoi = null;
		result r = pEnum->GetCurrent(pPoi);
		if (r != E_SUCCESS) {
			AppLogException(
					"Error getting poi from collection.", GetErrorMessage(r));
		}
		DateTime* pDate = new DateTime();
		pDate->SetValue(pPoi->GetTimestamp()->GetYear(),
				pPoi->GetTimestamp()->GetMonth(),
				pPoi->GetTimestamp()->GetDay(), 0, 0, 0);
		bool flag;
		r = retVal->ContainsKey(pDate->GetTicks(), flag);
		if (r != E_SUCCESS)
			AppLogException(
					"Error looking up key in poi hash.", GetErrorMessage(r));
		else {
			if (flag == false) {
				LinkedListT<POI*>* pPoiGroupCollection =
						new LinkedListT<POI*>();
				r = pPoiGroupCollection->Add(pPoi);
				if (r != E_SUCCESS)
					AppLogException(
							"Error adding poi to collection.", GetErrorMessage(r));
				r = retVal->Add(pDate->GetTicks(), pPoiGroupCollection);
				if (r != E_SUCCESS)
					AppLogException(
							"Error adding collection to hash.", GetErrorMessage(r));
				else
					count++;
			} else {
				LinkedListT<POI*>* pPoiGroupCollection = null;
				r = retVal->GetValue(pDate->GetTicks(), pPoiGroupCollection);
				if (r != E_SUCCESS || pPoiGroupCollection == null)
					AppLogException(
							"Error getting collection from hash.", GetErrorMessage(r));
				else
					r = pPoiGroupCollection->Add(pPoi);
				if (r != E_SUCCESS)
					AppLogException(
							"Error adding po to collection.", GetErrorMessage(r));
				else
					count++;
			}
		}
	}
	AppLog(
			"Processed and added [%d] pois from collection to hashtable. Orig count was [%d]", count, pCollection->GetCount());
	return retVal;
}

LinkedListT<TTMedia*>* StorageManager::GetMedia(long long int poiId) {
	result r = E_SUCCESS;
	DbEnumerator* pEnum = null;
	DbStatement* pStmt = null;
	LinkedListT<TTMedia*>* retVal = new LinkedListT<TTMedia*>();
	String sql;

	Database* db = BootstrapManager::getInstance()->getDatabase();

	sql.Append(L"SELECT ID FROM media WHERE POI_ID = ?");
	pStmt = db->CreateStatementN(sql);
	pStmt->BindInt64(0, poiId);

	AppLog(
			"Getting all media IDs for POI_ID [%d] from the database. SQL=[%ls]", poiId, sql.GetPointer());
	pEnum = db->ExecuteStatementN(*pStmt);

	r = GetLastResult();
	if (r != E_SUCCESS) {
		AppLogException(
				"Error getting media IDs for poi with ID [%ld] from database: [%s]", poiId, GetErrorMessage(r));
		return null;
	}

	if (pEnum != null) {

		LongLongBuffer poiBuffer;
		poiBuffer.Construct(1024);
		long long int count = 0;
		long long int id;

		while (pEnum->MoveNext() == E_SUCCESS) {
			r = pEnum->GetInt64At(0, id);
			if (r != E_SUCCESS) {
				AppLogException(
						"Error getting media id: [%s]", GetErrorMessage(r));
				return 0;
			}
			poiBuffer.Set(id);
			count++;
		}

		delete pEnum;
		delete pStmt;
		poiBuffer.Rewind();

		AppLog("Creating collection of media.");
		while (poiBuffer.GetPosition() < count) {
			TTMedia* pMedia = new TTMedia();
			r = poiBuffer.Get(id);
			r = pMedia->Construct(id);
			if (r != E_SUCCESS) {
				AppLogException(
						"Error constructing media with ID [%d]: [%s]", id, GetErrorMessage(r));
				return 0;
			}
			r = retVal->Add(pMedia);
			if (r != E_SUCCESS) {
				AppLogException(
						"Error adding media with uri [%ls] to collection: [%s]", pMedia->GetSourceUri()->GetPointer(), GetErrorMessage(r));
				return 0;
			}
			AppLog(
					"Successfully added media [%ls] to collection.", pMedia->GetSourceUri()->GetPointer());
		}
	} else
		AppLog("No media in the database, returning empty collection.");

	AppLog(
			"Successfully read and added [%d] media items to collection.", retVal->GetCount());

	return retVal;
}

LinkedListT<Tracker*>* StorageManager::GetTracks(void) {
	result r = E_SUCCESS;
	DbEnumerator* pEnum = 0;
	LinkedListT<Tracker*>* retVal = new LinkedListT<Tracker*>();
	String sql;
	long long int id;

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

	if (pEnum != null && r == E_SUCCESS)
		while (pEnum->MoveNext() == E_SUCCESS) {
			r = pEnum->GetInt64At(0, id);
			if (r != E_SUCCESS) {
				AppLogException(
						"Error getting track id: [%s]", GetErrorMessage(r));
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
	else
		AppLog("No tracks in the database, returning empty collection.");
	AppLog(
			"Successfully read and added [%d] tracks to collection.", retVal->GetCount());
	delete pEnum;
	return retVal;
}

LinkedListT<TTLocation*>* StorageManager::GetLocations(
		long long int trackerId) {
	result r = E_SUCCESS;
	DbEnumerator* pEnum = 0;
	DbStatement* pStmt = null;
	LinkedListT<TTLocation*>* retVal = new LinkedListT<TTLocation*>();
	String sql;
	long long int id;

	Database* db = BootstrapManager::getInstance()->getDatabase();

	sql.Append(L"SELECT ID FROM location WHERE track_ID = ?");
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
	pStmt->BindInt64(0, trackerId);
	pEnum = db->ExecuteStatementN(*pStmt);

	r = GetLastResult();
	if (pEnum == 0 || r != E_SUCCESS) {
		AppLogException(
				"Error getting all location IDs for tracker ID [%d] from database: [%s]", trackerId, GetErrorMessage(r));
		return 0;
	}
	AppLog( "Creating collection of locations for track ID [%d].", trackerId);

	if (pEnum != null && r == E_SUCCESS)
		while (pEnum->MoveNext() == E_SUCCESS) {
			r = pEnum->GetInt64At(0, id);
			if (r != E_SUCCESS) {
				AppLogException(
						"Error getting location id: [%s]", GetErrorMessage(r));
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
	else
		AppLog("No locations in the database, returning empty collection.");
	AppLog(
			"Successfully read and added [%d] locations to collection.", retVal->GetCount());
	delete pEnum;
	delete pStmt;
	return retVal;
}

Tizen::Base::Collection::LinkedListT<POI*>* StorageManager::GetPois(
		Tracker* pTracker) {
	result r = E_SUCCESS;

	DateTime startTime = *(pTracker->StartPosition()->getTimestamp());
	DateTime endTime = *(pTracker->EndPosition()->getTimestamp());
	DbEnumerator* pEnum = null;
	DbStatement* pStmt = null;
	LinkedListT<POI*>* retVal = new LinkedListT<POI*>();
	String sql;

	Database* db = BootstrapManager::getInstance()->getDatabase();

	sql.Append(L"SELECT ID FROM poi WHERE TimeSig >= ? AND TimeSig <= ?");
	pStmt = db->CreateStatementN(sql);
	pStmt->BindDateTime(0, startTime);
	pStmt->BindDateTime(1, endTime);

	r = GetLastResult();
	if (pStmt == 0 || r != E_SUCCESS) {
		AppLogException(
				"Error creating sql statement for SELECT for POIs between %ls and %ls:", startTime.ToString().GetPointer(), endTime.ToString().GetPointer(), GetErrorMessage(r));
		return 0;
	}

	AppLog(
			"Sql SELECT statement created for for SELECT for POIs between %ls and %ls:", startTime.ToString().GetPointer(), endTime.ToString().GetPointer());
	AppLog(
			"Getting list of POIs between %ls and %ls from the database.", startTime.ToString().GetPointer(), endTime.ToString().GetPointer());

	pEnum = db->ExecuteStatementN(*pStmt);

	r = GetLastResult();
	if (r != E_SUCCESS) {
		AppLogException(
				"Getting list of POIs between %ls and %ls from the database.", startTime.ToString().GetPointer(), endTime.ToString().GetPointer(), GetErrorMessage(r));
		return null;
	}

	if (pEnum != null) {

		LongLongBuffer poiBuffer;
		poiBuffer.Construct(1024);
		long long int count = 0;
		long long int id;

		while (pEnum->MoveNext() == E_SUCCESS) {
			r = pEnum->GetInt64At(0, id);
			if (r != E_SUCCESS) {
				AppLogException(
						"Error getting poi id: [%s]", GetErrorMessage(r));
				return 0;
			}
			poiBuffer.Set(id);
			count++;
		}

		delete pEnum;
		poiBuffer.Rewind();

		AppLog("Creating collection of pois.");
		while (poiBuffer.GetPosition() < count) {
			POI* pPoi = new POI();
			r = poiBuffer.Get(id);
			r = pPoi->Construct(id);
			if (r != E_SUCCESS) {
				AppLogException(
						"Error constructing poi with ID [%d]: [%s]", id, GetErrorMessage(r));
				return 0;
			}
			r = retVal->Add(pPoi);
			if (r != E_SUCCESS) {
				AppLogException(
						"Error adding poi [%ls] to collection: [%s]", pPoi->GetTitle()->GetPointer(), GetErrorMessage(r));
				return 0;
			}
			AppLog(
					"Successfully added poi [%ls] to collection.", pPoi->GetTitle()->GetPointer());
		}
	} else
		AppLog("No POIs in the database, returning empty collection.");

	AppLog(
			"Successfully read and added [%d] pois to collection.", retVal->GetCount());

	return retVal;
}

FacebookAccessToken StorageManager::GetFacebookCredentials(void) const {
	return __facebookCredentials;
}

void StorageManager::UpdateFacebookCredentials(
		FacebookAccessToken& credentials) {
	__facebookCredentials = credentials;
}

Tizen::Base::Collection::LinkedListT<Tracker*>* StorageManager::GetTracks(
		POI* pPoi) {
	result r = E_SUCCESS;

	LinkedListT<Tracker*>* retVal = new LinkedListT<Tracker*>();
	IEnumeratorT<Tracker*>* pEnum=GetTracks()->GetEnumeratorN();

	while (pEnum->MoveNext() == E_SUCCESS) {
		Tracker* pTrack = null;

		pEnum->GetCurrent(pTrack);
		if (*(pTrack->StartPosition()->getTimestamp()) <= *(pPoi->GetTimestamp()) && *(pTrack->EndPosition()->getTimestamp()) >= *(pPoi->GetTimestamp()))
			retVal->Add(pTrack);
	}

	return retVal;
}

StorageManager::StorageManager() {
	__facebookCredentials.AppId = String(L"316072995237651");
	__facebookCredentials.AppSecret = L"c6226e59c07e451c0b038c30ba9312de";
	__facebookCredentials.AccessToken = L"";
	__facebookCredentials.ExpiryTime = -1;
}

StorageManager::~StorageManager() {
	delete __pSelf;
}

