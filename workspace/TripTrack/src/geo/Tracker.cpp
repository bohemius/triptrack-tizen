/*
 * Tracker.cpp
 *
 *  Created on: Jul 21, 2013
 *      Author: hsp
 */

#include "geo/Tracker.h"
#include "dao/TTLocation.h"

using namespace Tizen::Locations;
using namespace Tizen::Base::Collection;

Tracker::Tracker() {
	__pTrackPoints=new LinkedListT<TTLocation*>;

}

Tracker::~Tracker() {
	// TODO Auto-generated destructor stub
}

void Tracker::AddLocation(TTLocation* position) {
	//const TTLocation* loc=position;
	__pTrackPoints->Add(position);
}

TTLocation* Tracker::FindLocationAtTime(
		Tizen::Base::DateTime timeStamp,
		const Tizen::Base::DateTime timeWindow) {
	return NULL;
}

TTLocation* Tracker::StartPosition(void) {
	return NULL;
}

TTLocation* Tracker::EndPosition(void) {
	return NULL;
}

int Tracker::TrackerStatus(void) {
	return NULL;
}

LinkedListT<TTLocation*>* Tracker::GetTrack(
		void) {
	return __pTrackPoints;
}


