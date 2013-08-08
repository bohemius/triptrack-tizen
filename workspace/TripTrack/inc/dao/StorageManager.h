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

class StorageManager {
public:
	virtual ~StorageManager();
	result Construct(Tizen::Io::Database* database);
	static StorageManager* getInstance(void);
	Tizen::Io::DbEnumerator* CRUDoperation(I_CRUDable* entity, enum I_CRUDable::CRUD_OP);
	Tizen::Io::DbEnumerator* PerformTransaction(Tizen::Io::DbStatement* statement);

private:
	StorageManager();
	static StorageManager* __pSelf;
	Tizen::Io::Database* __pDb;
};

#endif /* STORAGEMANAGER_H_ */
