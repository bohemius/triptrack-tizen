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
#include "dao/TTLocation.h"
#include "dao/ICRUD.h"

class Tracker: public Tizen::Locations::ILocationProviderListener,
		public Tizen::Ui::IActionEventListener, public I_CRUDable {
public:
	static const int ACTIVE;
	static const int PAUSED;
	static const int LOCKED;

	Tracker();
	virtual ~Tracker();

	result AddLocation(Tizen::Locations::Location position);
	TTLocation* FindLocationAtTime(Tizen::Base::DateTime timeStamp,
			const Tizen::Base::DateTime timeWindow);
	TTLocation* StartPosition(void);
	TTLocation* EndPosition(void);
	result Construct(int id);
	Tizen::Base::Collection::LinkedListT<TTLocation*>* GetTrack(void);
	Tizen::Base::String* GetDescription() const;
	void SetDescription(Tizen::Base::String* description);
	Tizen::Base::String* GetTitle() const;
	void SetTitle(Tizen::Base::String* title);
	Tizen::Base::Collection::LinkedListT<TTLocation*>* GetTrackPoints() const;
	int GetTrackerId() const;
	virtual Tizen::Io::DbStatement* Read(void);
	virtual Tizen::Io::DbStatement* Write(void);
	virtual Tizen::Io::DbStatement* Delete(void);
	virtual Tizen::Io::DbStatement* Update(void);
	double GetDistance() const;
	int GetStatus() const;
	void SetStatus(int status);

private:

	int __trackerId;
	int __status;
	Tizen::Base::String *__pDescription;
	Tizen::Base::String *__pTitle;
	Tizen::Base::Collection::LinkedListT<TTLocation*>* __pTrackPoints;
	double __distance;
};

#endif /* TRACKER_H_ */
