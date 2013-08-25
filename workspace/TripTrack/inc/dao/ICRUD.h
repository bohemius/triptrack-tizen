/*
 * ICRUD.h
 *
 *  Created on: Aug 4, 2013
 *      Author: bohemius
 */
#ifndef ICRUD_H_
#define ICRUD_H_

#include <FIo.h>
#include <FBase.h>

class I_CRUDable {
public:
	enum CRUD_OP {CREATE, READ, UPDATE, DELETE};

	virtual Tizen::Io::DbStatement* Read(void)=0;
	virtual Tizen::Io::DbStatement* Write(void)=0;
	virtual Tizen::Io::DbStatement* Delete(void)=0;
	virtual Tizen::Io::DbStatement* Update(void)=0;
};

#endif /* ICRUD_H_ */
