/*
 * MapForm.cpp
 *
 *  Created on: Aug 7, 2014
 *      Author: bohemius
 */

#include "ui/MapForm.h"
#include "SceneRegister.h"

using namespace Tizen::Ui::Controls;
using namespace Tizen::Ui::Scenes;
using namespace Tizen::Base;
using namespace Tizen::Locales;
using namespace HMaps;

MapForm::MapForm() :
		__pMap(null) {
	// TODO Auto-generated constructor stub

}

MapForm::~MapForm() {
}

bool MapForm::Initialize(void) {
	result r = Form::Construct(FORM_STYLE_NORMAL);
	if (IsFailed(r)) {
		return false;
	}
	return true;
}

result MapForm::OnInitializing(void) {
	result r = E_SUCCESS;

	String appId = L"SkH6ws8o9MTThvyKJLJN";
	String appCode = L"14LI-CnPtBQEtVEKofyY9w";

	//TODO should take it from local manager which holds the currently selected language
	if (!MapApplicationContext::GetInstance().IsInitialized())
		MapApplicationContext::GetInstance().Initialize(appCode, appId,
				LANGUAGE_ENG);

	if (__pMap == null) {
		__pMap = new (std::nothrow) Map();
		r = __pMap->Construct(GetClientAreaBounds().width,
				GetClientAreaBounds().height);
		if (r != E_SUCCESS) {
			AppLogException(
					"Error constructing map control: [%s]", GetErrorMessage(r));
			return r;
		}
		AddControl(__pMap);
	}

	Tizen::Locations::Coordinates coord;

	// Coordinates for Los Angeles
	coord.Set(34.0535, -118.245, 0.0);
	__pMap->SetCenter(coord);
	__pMap->SetZoomLevel(12.0);

	SetFormBackEventListener(this);

	return r;
}

result MapForm::OnTerminating(void) {
}

void MapForm::OnFormBackRequested(Tizen::Ui::Controls::Form& source) {
	SceneManager* pSceneMngr = SceneManager::GetInstance();
	pSceneMngr->GoBackward(BackwardSceneTransition(SCENE_MAIN_FORM));
}

void MapForm::OnSceneActivatedN(
		const Tizen::Ui::Scenes::SceneId& previousSceneId,
		const Tizen::Ui::Scenes::SceneId& currentSceneId,
		Tizen::Base::Collection::IList* pArgs) {
	AppLog("Activated MAP_FORM scene");
}

void MapForm::OnMapLongPressed(Map& map,
		const Tizen::Locations::Coordinates& coordinate) {
}

void MapForm::OnMapRegionChanged(Map& map) {
}

void MapForm::OnMapTapped(Map& map,
		const Tizen::Locations::Coordinates& coordinate) {
}

void MapForm::OnSceneDeactivated(
		const Tizen::Ui::Scenes::SceneId& currentSceneId,
		const Tizen::Ui::Scenes::SceneId& nextSceneId) {
	AppLog("Deactivate MAP_FORM scene");
}

