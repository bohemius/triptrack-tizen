/*
 * POI.cpp
 *
 *  Created on: Jul 30, 2013
 *      Author: bohemius
 */

#include "dao/POI.h"
#include "geo/Tracker.h"

POI::POI() {
	// TODO Auto-generated constructor stub

}

POI::~POI() {
	// TODO Auto-generated destructor stub
}

int POI::GetDefImageId() const {
	return __defImageId;
}

void POI::SetDefImageId(int defImageId) {
	__defImageId = defImageId;
}

Tizen::Base::Collection::LinkedListT<TTMedia*>* POI::GetAssociatedMedia() const {
	return __pAssociatedMedia;
}

Tizen::Base::String* POI::GetDescription() const {
	return __pDescription;
}

void POI::SetDescription(Tizen::Base::String* description) {
	__pDescription = description;
}

TTLocation* POI::GetLocation() const {
	return __pLocation;
}

void POI::SetLocation(TTLocation* location) {
	__pLocation = location;
}

Tizen::Base::String* POI::GetTitle() const {
	return __pTitle;
}

void POI::SetTitle(Tizen::Base::String* title) {
	__pTitle = title;
}

void POI::AddMedia(TTMedia* media) {
	__pAssociatedMedia->Add(media);
}

void POI::DeleteMedia(TTMedia* media) {
	__pAssociatedMedia->Remove(media);
}

int POI::GetId() const {
	return __id;
}




