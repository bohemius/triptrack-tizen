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

class Tracker: public Tizen::Locations::ILocationProviderListener,
		public Tizen::Ui::IActionEventListener {
public:
	Tracker();
	virtual ~Tracker();

	void AddLocation(TTLocation* position);
	TTLocation* FindLocationAtTime(Tizen::Base::DateTime timeStamp, const Tizen::Base::DateTime timeWindow);
	TTLocation* StartPosition(void);
	TTLocation* EndPosition(void);
	int TrackerStatus(void);
	Tizen::Base::Collection::LinkedListT<TTLocation*>* GetTrack(void);

private:

	int __trackerId;
	Tizen::Base::Collection::LinkedListT<TTLocation*>* __pTrackPoints;
};

#endif /* TRACKER_H_ */
