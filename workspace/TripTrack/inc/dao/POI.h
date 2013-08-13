/*
 * POI.h
 *
 *  Created on: Jul 30, 2013
 *      Author: bohemius
 */

#ifndef POI_H_
#define POI_H_

#include <FBase.h>
#include <FLocations.h>
#include "dao/TTLocation.h"
#include "dao/TTMedia.h"
#include "dao/ICRUD.h"

class POI: I_CRUDable {
public:
	POI();
	virtual ~POI();

	int GetDefImageId() const;
	void SetDefImageId(int defImageId);
	result Construct(int id);
	result Construct(Tizen::Base::String &Title,
			Tizen::Base::String &Description);
	Tizen::Base::Collection::LinkedListT<TTMedia*>* GetAssociatedMedia() const;
	Tizen::Base::String* GetDescription() const;
	void SetDescription(Tizen::Base::String* description);
	TTLocation* GetLocation() const;
	void SetLocation(TTLocation* location);
	Tizen::Base::String* GetTitle() const;
	void SetTitle(Tizen::Base::String* title);
	void AddMedia(TTMedia* media);
	void DeleteMedia(TTMedia* media);
	int GetId() const;
	virtual Tizen::Io::DbStatement* Read(void);
	virtual Tizen::Io::DbStatement* Write(void);
	virtual Tizen::Io::DbStatement* Delete(void);
	virtual Tizen::Io::DbStatement* Update(void);
	Tizen::Base::DateTime* GetTimestamp() const;
	void SetTimestamp(Tizen::Base::DateTime* timestamp);

private:
	Tizen::Base::String* __pDescription;
	Tizen::Base::String* __pTitle;
	Tizen::Base::DateTime* __pTimestamp;
	TTLocation* __pLocation;
	int __defImageId;
	int __id;
	Tizen::Base::Collection::LinkedListT<TTMedia*>* __pAssociatedMedia;
};

#endif /* POI_H_ */
