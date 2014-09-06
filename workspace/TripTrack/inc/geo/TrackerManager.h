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

class TrackerManager: public Tizen::Locations::ILocationProviderListener {
public:
	virtual ~TrackerManager();
	static TrackerManager* getInstance(void);
	result AddTracker(Tizen::Base::String &Title,
			Tizen::Base::String &Description);
	result RemoveTracker(Tracker* pTracker);
	result Construct(void);
	Tracker* GetCurrentTracker(void) const;
	void SetCurrentTracker(Tracker* currentTracker);
	Tizen::Base::Collection::LinkedListT<Tracker*>* GetTracks() const;
	Tizen::Locations::LocationProvider* GetLocationProvider(void);
	void SetLocationProvider(Tizen::Locations::LocationProvider* provider);
	result AddOnTrackChangeListener(IOnDataChangedListener* listener);
	result RemoveOnTrackChangeListener(IOnDataChangedListener* listener);

	//ILocationProviderListener
	virtual void OnLocationUpdated(const Tizen::Locations::Location& location);
	virtual void OnLocationUpdateStatusChanged(
			Tizen::Locations::LocationServiceStatus status);
	virtual void OnRegionEntered(Tizen::Locations::RegionId regionId);
	virtual void OnRegionLeft(Tizen::Locations::RegionId regionId);
	virtual void OnRegionMonitoringStatusChanged(
			Tizen::Locations::LocationServiceStatus status);
	virtual void OnAccuracyChanged(Tizen::Locations::LocationAccuracy accuracy);

private:
	TrackerManager();

	Tracker* __pCurrentTracker;
	Tizen::Base::Collection::LinkedListT<Tracker*>* __pTracks;
	Tizen::Locations::LocationProvider* __pLocProvider;
	Tizen::Base::Collection::LinkedListT<IOnDataChangedListener*>* __pListeners;
	static TrackerManager* __pSelf;
};

#endif /* TRACKERMANAGER_H_ */
