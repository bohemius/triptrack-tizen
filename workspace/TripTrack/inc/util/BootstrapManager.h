/*
 * BootstrapManager.h
 *
 *  Created on: Aug 1, 2013
 *      Author: bohemius
 */

#ifndef BOOTSTRAPMANAGER_H_
#define BOOTSTRAPMANAGER_H_

#include <FIo.h>
#include <FBase.h>
#include <FApp.h>
#include <unique_ptr.h>

class BootstrapManager {
public:
	virtual ~BootstrapManager();
	void CreateDatabase(Tizen::Base::String createSql);
	void DropDatabase(Tizen::Base::String dropSql);
	result Construct(void);
	result Bootstrap(void);
	static Tizen::Io::Database* getDatabase(Tizen::Base::String filename);
	static Tizen::Base::String* readFile(Tizen::Base::String filename);
	Tizen::Base::String* getScript(void);
	static BootstrapManager* getInstance(void);

	static const Tizen::Base::String defaultScriptFilename;
	static const Tizen::Base::String defaultDbFilename;

private:
	BootstrapManager();
	static BootstrapManager* __pSelf;
	Tizen::Base::String* __pSql;
	Tizen::Io::Database* __pDb;
};

#endif /* BOOTSTRAPMANAGER_H_ */
