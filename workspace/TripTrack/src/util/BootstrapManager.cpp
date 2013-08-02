/*
 * BootstrapManager.cpp
 *
 *  Created on: Aug 1, 2013
 *      Author: bohemius
 */

#include "util/BootstrapManager.h"

using namespace Tizen::Io;
using namespace Tizen::Base;
using namespace Tizen::App;

BootstrapManager* BootstrapManager::__pSelf = 0;
const String BootstrapManager::defaultDbFilename(L"tt.db");
const String BootstrapManager::defaultScriptFilename(L"tt.sql");

BootstrapManager::BootstrapManager() {
}

BootstrapManager::~BootstrapManager() {
	delete __pDb;
	delete __pSql;
	delete __pSelf;
}

BootstrapManager* BootstrapManager::getInstance(void) {
	if (__pSelf == 0)
		__pSelf = new BootstrapManager();
	return __pSelf;
}

void BootstrapManager::CreateDatabase(Tizen::Base::String createSql) {
	//not used right now
}

void BootstrapManager::DropDatabase(Tizen::Base::String dropSql) {
	//not used right now
}

result BootstrapManager::Construct(void) {
	__pDb = 0;
	__pSql = 0;
	result r = E_SUCCESS;

	AppLog("Constructing database.");
	__pDb = getDatabase(
			App::GetInstance()->GetAppDataPath() + defaultDbFilename);
	if (__pDb == 0) {
		AppLogException(
				"Error constructing db file [%ls]", (App::GetInstance()->GetAppDataPath() + defaultDbFilename).GetPointer());
		r = E_FAILURE;
	} else
		AppLog("Opened db file.");
	AppLog("Reading SQL file into memory.");
	__pSql = readFile(
			App::GetInstance()->GetAppDataPath() + defaultScriptFilename);
	if (__pSql == 0) {
		AppLogException(
				"Error loading SQL: [%ls]", (App::GetInstance()->GetAppDataPath() + defaultScriptFilename).GetPointer());
		r = E_FAILURE;
	} else
		AppLog("Opened and read sql: [%ls]", __pSql->GetPointer());

	return r;
}

result BootstrapManager::Bootstrap(void) {
	result r = E_SUCCESS;

	AppLog("Bootstrapping database.");
	r = __pDb->ExecuteSql(*__pSql,true);
	if (r != E_SUCCESS) {
		AppLogException(
				"Error bootstrapping database: [%s]", GetErrorMessage(r));
		return r;
	}
	AppLog("Bootstrap complete");
	return r;
}

Database* BootstrapManager::getDatabase(String filename) {
	Database* db = new Database();
	result r = E_SUCCESS;

	AppLog("Opening db file: [%ls]", filename.GetPointer());
	r = db->Construct(filename, "a+");

	if (r != E_SUCCESS) {
		AppLogException(
				"Error opening db file [%ls]: [%s]", filename.GetPointer(), GetErrorMessage(r));
		return 0;
	}
	return db;
}

Tizen::Base::String* BootstrapManager::readFile(String filename) {
	FileAttributes attr;
	File sqlFile;
	result r = E_SUCCESS;

	AppLog("Getting attributes for sql file: [%ls]", filename.GetPointer());
	r = File::GetAttributes(filename, attr);
	if (r != E_SUCCESS) {
		AppLogException(
				"Error getting attributes for sql file [%ls]: [%s]", filename.GetPointer(), GetErrorMessage(r));
		return 0;
	}

	long filesize = attr.GetFileSize();
	char* buffer = new char[filesize];

	AppLog("Opening sql file sized %d", filesize);
	r = sqlFile.Construct(filename, "r");
	if (r != E_SUCCESS) {
		AppLogException(
				"Error opening sql file [%ls]: [%s]", filename.GetPointer(), GetErrorMessage(r));
		return 0;
	}

	int readCount = sqlFile.Read(buffer, filesize);
	if (r != E_SUCCESS || readCount < 0 || readCount != filesize) {
		AppLogException(
				"Error reading sql file [%ls]: [%s]", filename.GetPointer(), GetErrorMessage(r));
		return 0;
	}
	AppLog("Read count: [%d] Orig size: [%d]", readCount, filesize);

	String* retVal = new String(buffer);
	return retVal;
}

Tizen::Base::String* BootstrapManager::getScript(void) {
	return __pSql;
}

