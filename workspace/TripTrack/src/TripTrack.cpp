/*
 * TripTrack.cpp
 *
 *  Created on: Aug 15, 2013
 *      Author: hsp
 */
#include "TripTrack.h"
#include "SceneRegister.h"
#include "util/BootstrapManager.h"
#include "geo/Tracker.h"

using namespace Tizen::App;
using namespace Tizen::Base;
using namespace Tizen::Base::Collection;
using namespace Tizen::System;
using namespace Tizen::Ui;
using namespace Tizen::Ui::Controls;
using namespace Tizen::Ui::Scenes;

TripTrackApp::TripTrackApp(void) {
}

TripTrackApp::~TripTrackApp(void) {
}

UiApp*
TripTrackApp::CreateInstance(void) {
	return new TripTrackApp();
}

bool TripTrackApp::OnAppInitializing(AppRegistry& appRegistry) {
	BootstrapManager* bmInstance = BootstrapManager::getInstance();
	result r = E_SUCCESS;

	AppLog("Constructing BootstrapManager.");
	r = bmInstance->Construct();
	if (r != E_SUCCESS) {
		AppLogException(
				"Error constructing BootstrapManager: [%]", GetErrorMessage(r));
		return false;
	}

	/*
	Tracker* track = new Tracker();

	AppLog("Constructing testing tracker");
	String desc(L"Testing tracker used for testing");
	String title(L"Test track");
	r = track->Construct(desc, title);
	if (r != E_SUCCESS) {
		AppLogException(
				"Error constructin tracker [%ls]: [%s]", track->GetTitle()->GetPointer(), GetErrorMessage(r));
		return false;
	}
	AppLog(
			"Successfully constructed tracker [%ls] ", track->GetTitle()->GetPointer());

	*/
	Frame* pAppFrame = new Frame();
	pAppFrame->Construct();
	AddFrame(*pAppFrame);

	// Prepare Scene management.
	SceneRegister::RegisterAllScenes();
	SceneManager* pSceneManager = SceneManager::GetInstance();
	pSceneManager->GoForward(ForwardSceneTransition(SCENE_MAIN_FORM));

	return true;
}

bool TripTrackApp::OnAppInitialized(void) {
	return true;
}

bool TripTrackApp::OnAppWillTerminate(void) {
	return true;
}

bool TripTrackApp::OnAppTerminating(AppRegistry& appRegistry,
		bool forcedTermination) {
	return true;
}

void TripTrackApp::OnForeground(void) {
}

void TripTrackApp::OnBackground(void) {
}

void TripTrackApp::OnLowMemory(void) {
}

void TripTrackApp::OnBatteryLevelChanged(BatteryLevel batteryLevel) {
}

void TripTrackApp::OnScreenOn(void) {
}

void TripTrackApp::OnScreenOff(void) {
}

