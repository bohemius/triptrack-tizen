/*
 * Tracker.h
 *
 *  Created on: Jul 21, 2013
 *      Author: hsp
 */

#ifndef TRACKER_H_
#define TRACKER_H_

#include <FLocILocationProviderListener.h>
#include <FUiIActionEventListener.h>
#include <FBase.h>
#include <FLocations.h>
#include <FGraphics.h>
#include "ui/CommonComponents.h"
#include "dao/TTLocation.h"
#include "dao/ICRUD.h"

class Tracker: public I_CRUDable,
		public IFormFieldProvider,
		public Tizen::Base::Object {
public:
	static const int ACTIVE = 0;
	static const int PAUSED = 1;
	static const int LOCKED = 2;
	Tracker();
	virtual ~Tracker();
	result AddLocation(Tizen::Locations::Location position);
	TTLocation* FindLocationAtTime(Tizen::Base::DateTime timeStamp,
			const Tizen::Base::DateTime timeWindow);
	TTLocation* StartPosition(void);
	TTLocation* EndPosition(void);
	result Construct(void);
	result Construct(long long int id);
	result Construct(Tizen::Base::String& Description,
			Tizen::Base::String& Title);
	Tizen::Base::Collection::LinkedListT<TTLocation*>* GetTrack(void);
	Tizen::Base::String* GetDescription(void) const;
	void SetDescription(Tizen::Base::String* description);
	Tizen::Base::String* GetTitle(void) const;
	void SetTitle(Tizen::Base::String* title);
	Tizen::Base::Collection::LinkedListT<TTLocation*>* GetTrackPoints(void) const;
	Tizen::Base::TimeSpan GetDuration(void);
	long long int GetTrackerId(void) const;
	result RemoveTrackPoints(void);

	// I_CRUDable
	virtual Tizen::Io::DbStatement* Read(void);
	virtual Tizen::Io::DbStatement* Write(void);
	virtual Tizen::Io::DbStatement* Delete(void);
	virtual Tizen::Io::DbStatement* Update(void);

	// IFormFieldProvider
	virtual Tizen::Base::Collection::LinkedListT<FormField*>* GetFields(void);
	virtual result SaveFields(Tizen::Base::Collection::LinkedListT<FormField*>* fieldList);
	virtual int GetFieldCount(void);
	virtual int GetProviderID(void);

	double GetDistance() const;
	int GetStatus() const;
	void SetStatus(int status);
	void SetDistance(double distance);

private:

	long long int __trackerId;
	int __status;
	Tizen::Base::String *__pDescription;
	Tizen::Base::String *__pTitle;
	Tizen::Base::Collection::LinkedListT<TTLocation*>* __pTrackPoints;
	Tizen::Base::TimeSpan* __pDuration;
	double __distance;
};

#endif /* TRACKER_H_ */
