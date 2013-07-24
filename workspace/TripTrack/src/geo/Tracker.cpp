/*
 * Tracker.cpp
 *
 *  Created on: Jul 21, 2013
 *      Author: hsp
 */

#include "geo/Tracker.h"

using namespace Tizen::Locations;
using namespace Tizen::Base::Collection;

Tracker::Tracker() {
	__pTrackPoints=new LinkedList(SingleObjectDeleter);

}

Tracker::~Tracker() {
	// TODO Auto-generated destructor stub
}

void Tracker::AddLocation(Location position) {
	__pTrackPoints->Add(position);
}

Tizen::Locations::Location* Tracker::FindLocationAtTime(
		Tizen::Base::DateTime timeStamp,
		const Tizen::Base::DateTime timeWindow) {
	return NULL;
}

Tizen::Locations::Location* Tracker::StartPosition(void) {
	return NULL;
}

Tizen::Locations::Location* Tracker::EndPosition(void) {
	return NULL;
}

int Tracker::TrackerStatus(void) {
	return NULL;
}

LinkedList* Tracker::GetTrack(
		void) {
	return __pTrackPoints;
}


