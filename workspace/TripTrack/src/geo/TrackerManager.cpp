/*
 * TrackerManager.cpp
 *
 *  Created on: Aug 4, 2013
 *      Author: bohemius
 */

#include "geo/TrackerManager.h"
#include "geo/Tracker.h"

using namespace Tizen::Base::Collection;

TrackerManager* TrackerManager::__pSelf=0;

TrackerManager::TrackerManager() {
	__pTracks=new LinkedListT<Tracker*>;
}

TrackerManager::~TrackerManager() {
	// TODO Auto-generated destructor stub
}

Tracker* TrackerManager::GetCurrentTracker() const {
	return __pCurrentTracker;
}

void TrackerManager::SetCurrentTracker(Tracker* currentTracker) {
	__pCurrentTracker = currentTracker;
}

TrackerManager* TrackerManager::getInstance(void) {
	if (__pSelf == 0)
		__pSelf=new TrackerManager();
	return __pSelf;
}

result TrackerManager::AddTracker(Tracker* tracker) {

}

result TrackerManager::RemoveTracker(Tracker* tracker) {
}

result TrackerManager::Construct(void) {
}

Tizen::Base::Collection::LinkedListT<Tracker*>* TrackerManager::GetTracks() const {
	return __pTracks;
}


