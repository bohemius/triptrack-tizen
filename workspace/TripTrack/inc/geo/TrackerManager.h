/*
 * TrackerManager.h
 *
 *  Created on: Aug 4, 2013
 *      Author: bohemius
 */

#ifndef TRACKERMANAGER_H_
#define TRACKERMANAGER_H_

#include <geo/Tracker.h>

class TrackerManager {
public:
	TrackerManager();
	virtual ~TrackerManager();
	static TrackerManager* getInstance(void);
	result AddTracker(Tracker* tracker);
	result RemoveTracker(Tracker* tracker);
	result Construct(void);
	Tracker* GetCurrentTracker() const;
	void SetCurrentTracker(Tracker* currentTracker);
	Tizen::Base::Collection::LinkedListT<Tracker*>* GetTracks() const;

private:
	Tracker* __pCurrentTracker;
	Tizen::Base::Collection::LinkedListT<Tracker*>* __pTracks;
	static TrackerManager* __pSelf;
};

#endif /* TRACKERMANAGER_H_ */
