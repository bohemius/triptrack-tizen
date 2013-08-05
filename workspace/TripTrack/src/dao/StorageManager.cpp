/*
 * StorageManager.cpp
 *
 *  Created on: Jul 21, 2013
 *      Author: hsp
 */

#include "dao/StorageManager.h"
#include "util/BootstrapManager.h"

using namespace Tizen::Base;
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

Tizen::Io::DbEnumerator* StorageManager::PerformTransaction(DbStatement* statement) {
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

StorageManager::StorageManager() {
}

StorageManager::~StorageManager() {
	delete __pSelf;
}

