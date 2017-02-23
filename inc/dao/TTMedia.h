/*
 * TTMedia.h
 *
 *  Created on: Jul 30, 2013
 *      Author: bohemius
 */
#include <FBase.h>
#include "dao/ICRUD.h"

#ifndef TTMEDIA_H_
#define TTMEDIA_H_

class TTMedia: public I_CRUDable, public Tizen::Base::Object {
public:
	TTMedia();
	virtual ~TTMedia();
	result Construct(long long int id);
	result Construct(Tizen::Base::String &SourceURI, long long PoiId, Tizen::Base::ByteBuffer* buffer);
	virtual Tizen::Io::DbStatement* Read(void);
	virtual Tizen::Io::DbStatement* Write(void);
	virtual Tizen::Io::DbStatement* Delete(void);
	virtual Tizen::Io::DbStatement* Update(void);
	long long int GetId() const;
	Tizen::Base::ByteBuffer* GetContent() const;
	void SetContent(Tizen::Base::ByteBuffer* content);
	Tizen::Base::String* GetSourceUri() const;
	void SetSourceUri(Tizen::Base::String* sourceUri);
	long long int GetPoiId() const;
	void SetPoiId(long long int poiId);

private:
	Tizen::Base::ByteBuffer* __pContent;
	Tizen::Base::String* __pSourceUri;
	long long int __id;
	long long int __poiId;

	static const int MAX_TILE_BASE_SIZE= 30720; //30KB
};

#endif /* TTMEDIA_H_ */
