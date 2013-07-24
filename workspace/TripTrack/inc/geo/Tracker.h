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

class Tracker: public Tizen::Locations::ILocationProviderListener,
		public Tizen::Ui::IActionEventListener {
public:
	Tracker();
	virtual ~Tracker();

	void AddLocation(Tizen::Locations::Location position);
	Tizen::Locations::Location* FindLocationAtTime(Tizen::Base::DateTime timeStamp, const Tizen::Base::DateTime timeWindow);
	Tizen::Locations::Location* StartPosition(void);
	Tizen::Locations::Location* EndPosition(void);
	int TrackerStatus(void);
	Tizen::Base::Collection::LinkedList* GetTrack(void);

private:

	int __trackerId;
	Tizen::Base::Collection::LinkedList* __pTrackPoints;
};

#endif /* TRACKER_H_ */
