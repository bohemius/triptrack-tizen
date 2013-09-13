/*
 * StorageManager.h
 *
 *  Created on: Jul 21, 2013
 *      Author: hsp
 */

#ifndef STORAGEMANAGER_H_
#define STORAGEMANAGER_H_

#include <FIo.h>
#include <FBase.h>
#include "ICRUD.h"
#include "dao/POI.h"
#include "dao/TTMedia.h"
#include "dao/TTLocation.h"
#include "geo/Tracker.h"

class StorageManager {
public:
	virtual ~StorageManager();
	result Construct(Tizen::Io::Database* database);
	static StorageManager* getInstance(void);
	Tizen::Io::DbEnumerator* CRUDoperation(I_CRUDable* entity, enum I_CRUDable::CRUD_OP);
	Tizen::Io::DbEnumerator* PerformTransaction(Tizen::Io::DbStatement* statement);
	Tizen::Base::Collection::LinkedListT<POI*>* GetPois(void);
	Tizen::Base::Collection::HashMapT<Tizen::Base::DateTime, Tizen::Base::Collection::LinkedListT<POI*>*>* GetPoiHash(void);
	Tizen::Base::Collection::LinkedListT<TTMedia*>* GetMedia(long long int poiId);
	Tizen::Base::Collection::LinkedListT<Tracker*>* GetTracks(void);
	Tizen::Base::Collection::LinkedListT<TTLocation*>* GetLocations(long long int trackerId);

private:
	StorageManager();
	static StorageManager* __pSelf;
	Tizen::Io::Database* __pDb;
};

#endif /* STORAGEMANAGER_H_ */
