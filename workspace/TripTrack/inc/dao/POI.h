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

class POI: public I_CRUDable {
public:
	POI();
	virtual ~POI();

	long long int GetDefImageId() const;
	void SetDefImageId(long long int defImageId);
	result Construct(long long int id);
	result Construct(Tizen::Base::String &Title,
			Tizen::Base::String &Description,
			Tizen::Locations::Location location);
	result Construct(Tizen::Base::String &Title,
			Tizen::Base::String &Description,
			Tizen::Locations::Location location,
			Tizen::Base::String& SourceUri);
	Tizen::Base::Collection::LinkedListT<TTMedia*>* GetAssociatedMedia() const;
	Tizen::Base::String* GetDescription() const;
	void SetDescription(Tizen::Base::String* description);
	Tizen::Locations::Coordinates* GetLocation() const;
	void SetLocation(Tizen::Locations::Coordinates* location);
	Tizen::Base::String* GetTitle() const;
	void SetTitle(Tizen::Base::String* title);
	void AddMedia(TTMedia* media);
	void DeleteMedia(TTMedia* media);
	long long int GetId() const;
	virtual Tizen::Io::DbStatement* Read(void);
	virtual Tizen::Io::DbStatement* Write(void);
	virtual Tizen::Io::DbStatement* Delete(void);
	virtual Tizen::Io::DbStatement* Update(void);
	result POI::ReadFromDb(void);
	Tizen::Base::DateTime* GetTimestamp() const;
	void SetTimestamp(Tizen::Base::DateTime* timestamp);

	static const float TILE_IMAGE_WIDTH = 100.0f;
	static const float TILE_IMAGE_HEIGHT = 176.0f;

private:
	Tizen::Base::String* __pDescription;
	Tizen::Base::String* __pTitle;
	Tizen::Base::DateTime* __pTimestamp;
	Tizen::Locations::Coordinates* __pCoordinates;
	long long int __defImageId;
	long long int __id;
	Tizen::Base::Collection::LinkedListT<TTMedia*>* __pAssociatedMedia;
};

#endif /* POI_H_ */
