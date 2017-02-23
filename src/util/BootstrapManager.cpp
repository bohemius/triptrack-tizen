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
const String BootstrapManager::defaultDbFilename("tt.db");
const String BootstrapManager::defaultScriptFilename("tt.sql");

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
	Database* db = new Database();
	__pDb = 0;
	__pSql = 0;
	result r = E_SUCCESS;

	String dbPath(App::GetInstance()->GetAppDataPath() + defaultDbFilename);
	String sqlPath(
			App::GetInstance()->GetAppDataPath() + defaultScriptFilename);

	AppLog("Constructing database.");

	//database exists, use it
	if (Database::Exists(dbPath)) {
		AppLog(
				"Db file [%ls] exists, using it to construct database.", dbPath.GetPointer());
		r = db->Construct(dbPath, "r+");
		if (r != E_SUCCESS) {
			AppLogException(
					"Error constructing db using file [%ls]", dbPath.GetPointer());
			r = E_FAILURE;
			return r;
		} else {
			AppLog("Opened db file [%ls].", dbPath.GetPointer());
			__pDb = db;
		}
	}
	//database does not exist create it
	else {
		AppLog(
				"Db file [%ls] does not exist, creating new db file: ", dbPath.GetPointer());
		r = db->Construct(dbPath, "a+");
		if (r != E_SUCCESS) {
			AppLogException(
					"Error constructing db using new file [%ls]", dbPath.GetPointer());
			r = E_FAILURE;
			return r;
		} else {
			AppLog("Opened db using new file [%ls].", dbPath.GetPointer());
			__pDb = db;
		}
		AppLog("Reading SQL file into memory.");
		__pSql = readFile(sqlPath);
		if (__pSql == 0) {
			AppLogException( "Error loading SQL: [%ls]", sqlPath.GetPointer());
			r = E_FAILURE;
			return r;
		} else {
			AppLog("Opened and read sql: [%ls]", __pSql->GetPointer());
			r = Bootstrap();
			if (r != E_SUCCESS) {
				AppLogException(
						"Error running bootstrap script [%ls]: ", sqlPath.GetPointer());
				return r;
			}
		}
	}
	AppLog("Construction complete");
	return r;
}

result BootstrapManager::Bootstrap(void) {
	result r = E_SUCCESS;

	AppLog("Bootstrapping database.");
	r = __pDb->ExecuteSql(*__pSql, true);
	if (r != E_SUCCESS) {
		AppLogException(
				"Error bootstrapping database: [%s]", GetErrorMessage(r));
		return r;
	}
	AppLog("Bootstrap complete");
	return r;
}

Database* BootstrapManager::getDatabase(void) {
	return __pDb;
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

