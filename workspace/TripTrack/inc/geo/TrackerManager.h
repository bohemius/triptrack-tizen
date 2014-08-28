/*
 * TrackerManager.h
 *
 *  Created on: Aug 4, 2013
 *      Author: bohemius
 */

#ifndef TRACKERMANAGER_H_
#define TRACKERMANAGER_H_

#include <geo/Tracker.h>
#include <FLocations.h>
#include <FBase.h>

class TrackerManager {
public:
	virtual ~TrackerManager();
	static TrackerManager* getInstance(void);
	result AddTracker(Tizen::Base::String &Title, Tizen::Base::String &Description);
	result RemoveTracker(Tracker* pTracker);
	result Construct(void);
	Tracker* GetCurrentTracker(void) const;
	void SetCurrentTracker(Tracker* currentTracker);
	Tizen::Base::Collection::LinkedListT<Tracker*>* GetTracks() const;
	Tizen::Locations::LocationProvider* GetLocationProvider(void);
	void SetLocationProvider(Tizen::Locations::LocationProvider* provider);

private:
	TrackerManager();

	Tracker* __pCurrentTracker;
	Tizen::Base::Collection::LinkedListT<Tracker*>* __pTracks;
	Tizen::Locations::LocationProvider* __pLocProvider;
	static TrackerManager* __pSelf;
};

#endif /* TRACKERMANAGER_H_ */
