/*
 * TrackerManager.cpp
 *
 *  Created on: Aug 4, 2013
 *      Author: bohemius
 */

#include "geo/TrackerManager.h"
#include "geo/Tracker.h"
#include "dao/StorageManager.h"

using namespace Tizen::Base::Collection;
using namespace Tizen::Base;

TrackerManager* TrackerManager::__pSelf = 0;

TrackerManager::TrackerManager() {
	__pTracks = new LinkedListT<Tracker*>;
}

TrackerManager::~TrackerManager() {
	// TODO Auto-generated destructor stub
}

Tracker* TrackerManager::GetCurrentTracker(void) const {
	return __pCurrentTracker;
}

void TrackerManager::SetCurrentTracker(Tracker* currentTracker) {
	__pCurrentTracker = currentTracker;
}

TrackerManager* TrackerManager::getInstance(void) {
	if (__pSelf == 0)
		__pSelf = new TrackerManager();
	return __pSelf;
}

//Creates new tracker and adds it to collection and database
result TrackerManager::AddTracker(String &Title, String &Description) {
	result r = E_SUCCESS;
	Tracker* pTracker = new Tracker();

	r = pTracker->Construct(Description, Title);
	if (r != E_SUCCESS) {
		AppLogException(
				"Error constructing new tracker [%ls]: [%s]", Title.GetPointer(), GetErrorMessage(r));
		r = E_FAILURE;
		delete pTracker;
		return r;
	}
	AppLog("Adding new tracker [%ls] to collection.", Title.GetPointer());
	r = __pTracks->Add(pTracker);
	if (r != E_SUCCESS) {
		AppLogException(
				"Error adding new tracker [%ls] to collection: [%s]", Title.GetPointer(), GetErrorMessage(r));
		r = E_FAILURE;
		delete pTracker;
		return r;
	}
	AppLog(
			"Successfully added tracker [%ls] to collection and database.", Title.GetPointer());
	return r;
}

//Removes existing tracker from collection and database
result TrackerManager::RemoveTracker(Tracker* pTracker) {
	result r = E_SUCCESS;

	AppLog(
			"Looking up existing tracker [%ls] with ID [%d] in collection.", pTracker->GetTitle()->GetPointer(), pTracker->GetTrackerId());
	r = __pTracks->Remove(pTracker);
	if (r != E_SUCCESS) {
		AppLogException(
				"Error removing tracker [%ls] with ID [%d] from collection: [%s]", pTracker->GetTitle()->GetPointer(), pTracker->GetTrackerId(), GetErrorMessage(r));
		return r;
	}
	AppLog(
			"Removing existing tracker [%ls] with ID [%d] from database.", pTracker->GetTitle()->GetPointer(), pTracker->GetTrackerId());
	StorageManager::getInstance()->CRUDoperation(pTracker, I_CRUDable::DELETE);
	r = GetLastResult();
	if (r != E_SUCCESS) {
		AppLogException(
				"Error removing tracker [%ls] with ID [%d] from database: [%s]", pTracker->GetTitle()->GetPointer(), pTracker->GetTrackerId(), GetErrorMessage(r));
		return r;
	}
	AppLog("Successfully removed tracker [%ls] with ID [%d] from collection and database.", pTracker->GetTitle()->GetPointer(), pTracker->GetTrackerId());
	delete pTracker;
	return r;
}

//Creates a collection of existing trackers from the database
result TrackerManager::Construct(void) {
}

Tizen::Base::Collection::LinkedListT<Tracker*>* TrackerManager::GetTracks() const {
	return __pTracks;
}

