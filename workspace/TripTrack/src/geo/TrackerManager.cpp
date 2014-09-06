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
using namespace Tizen::Locations;

TrackerManager* TrackerManager::__pSelf = 0;

Tizen::Locations::LocationProvider* TrackerManager::GetLocationProvider(void) {
	return __pLocProvider;
}

void TrackerManager::SetLocationProvider(
		Tizen::Locations::LocationProvider* provider) {
	__pLocProvider = provider;
}

void TrackerManager::OnLocationUpdated(
		const Tizen::Locations::Location& location) {
	result r = E_SUCCESS;

	//Add track to database and update track data
	if (location.IsValid()) {
		Tracker* pTracker = GetCurrentTracker();

		TTLocation* pLastLocation = pTracker->EndPosition();
		r = GetLastResult();
		if (r != E_SUCCESS || pLastLocation == null)
			AppLogException(
					"Error getting last recorded location from collection, distance not updated", GetErrorMessage(r));
		else {
			double dist = location.GetCoordinates().GetDistanceTo(
					*(pLastLocation->getCoordinates()));
			pTracker->SetDistance(pTracker->GetDistance() + dist);

			StorageManager::getInstance()->CRUDoperation(pTracker,
					I_CRUDable::UPDATE);
		}

		r = pTracker->AddLocation(location);
		if (r != E_SUCCESS)
			AppLogException(
					"Error adding new location for track: [%ld]", GetCurrentTracker()->GetTrackerId(), GetErrorMessage(r));
	} else
		AppLogException("Location is not valid");

	//Notify all listeners of the update
	IEnumeratorT<IOnDataChangedListener*>* pEnum =
			__pListeners->GetEnumeratorN();

	while (pEnum->MoveNext() == E_SUCCESS) {
		IOnDataChangedListener* pListener;
		r = pEnum->GetCurrent(pListener);
		if (r != E_SUCCESS)
			AppLogException(
					"Error getting track change listener from collection", GetErrorMessage(r));
		else {
			r = pListener->Update();
			if (r != E_SUCCESS)
				AppLogException(
						"Error executing track change listener", GetErrorMessage(r));
		}
	}
}

void TrackerManager::OnLocationUpdateStatusChanged(
		Tizen::Locations::LocationServiceStatus status) {
}

void TrackerManager::OnRegionEntered(Tizen::Locations::RegionId regionId) {
}

void TrackerManager::OnRegionLeft(Tizen::Locations::RegionId regionId) {
}

void TrackerManager::OnRegionMonitoringStatusChanged(
		Tizen::Locations::LocationServiceStatus status) {
}

void TrackerManager::OnAccuracyChanged(
		Tizen::Locations::LocationAccuracy accuracy) {
}

result TrackerManager::AddOnTrackChangeListener(
		IOnDataChangedListener* listener) {
	result r = E_SUCCESS;

	r = __pListeners->Add(listener);

	if (r != E_SUCCESS)
		AppLogException(
				"Error adding track change listener to collection", GetErrorMessage(r));

	return r;
}

result TrackerManager::RemoveOnTrackChangeListener(
		IOnDataChangedListener* listener) {
	result r = E_SUCCESS;

	r = __pListeners->Remove(listener);
	if (r != E_SUCCESS)
		AppLogException(
				"Error removing listener from collection.", GetErrorMessage(r));

	return r;
}

TrackerManager::TrackerManager() :
		__pLocProvider(null), __pCurrentTracker(null), __pTracks(null), __pListeners(
				null) {
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
	__pCurrentTracker = pTracker;
	__pCurrentTracker->SetStatus(Tracker::ACTIVE);
	return r;
}

//Removes existing tracker from collection and database
result TrackerManager::RemoveTracker(Tracker* pTracker) {
	result r = E_SUCCESS;

	r = pTracker->RemoveTrackPoints();
	if (r != E_SUCCESS) {
		AppLogExceptionInternal(
				"Error removing track points for tracker with id [%ld]: ",
				pTracker->GetTrackerId(), GetErrorMessage(r));
		return r;
	}

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
	AppLog(
			"Successfully removed tracker [%ls] with ID [%d] from collection and database.", pTracker->GetTitle()->GetPointer(), pTracker->GetTrackerId());
	delete pTracker;
	return r;
}

//Creates a collection of existing trackers from the database
result TrackerManager::Construct(void) {
	result r = E_SUCCESS;

	if (__pTracks == null && __pLocProvider == null && __pListeners == null) {
		//Create the IOnTrackChangeListener collection
		__pListeners = new LinkedListT<IOnDataChangedListener*>();

		//Fetch the track into collection from the database
		__pTracks = new LinkedListT<Tracker*>();
		__pTracks = StorageManager::getInstance()->GetTracks();

		//Construct location provider
		LocationCriteria criteria;

		criteria.SetAccuracy(LOC_ACCURACY_FINEST);
		__pLocProvider = new (std::nothrow) LocationProvider();
		r = __pLocProvider->Construct(criteria, *this);
		if (r != E_SUCCESS) {
			AppLogException(
					"Error constructing location provider: [%s]", GetErrorMessage(r));
			return r;
		}

		//Check for active tracker and resume tracking if neccessary
		IEnumeratorT<Tracker*>* pEnum = __pTracks->GetEnumeratorN();

		while (pEnum->MoveNext() == E_SUCCESS) {
			Tracker* pTracker;
			r = pEnum->GetCurrent(pTracker);
			if (r != E_SUCCESS)
				AppLogException(
						"Error accessing tracker from collection", GetErrorMessage(r));
			if (pTracker->GetStatus() == Tracker::ACTIVE) {
				if (__pLocProvider->GetLocationUpdateStatus()
						== LOC_SVC_STATUS_IDLE)
					__pLocProvider->StartLocationUpdatesByInterval(5);
				SetCurrentTracker(pTracker);
				return E_SUCCESS;
			}
		}
		return r;
	} else
		return E_ALREADY_SET;
}

Tizen::Base::Collection::LinkedListT<Tracker*>* TrackerManager::GetTracks() const {
	return __pTracks;
}

