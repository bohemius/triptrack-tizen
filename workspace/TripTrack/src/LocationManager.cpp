#include "LocationManager.h"
#include "LocationManagerMainForm.h"
#include "SceneRegister.h"

using namespace Tizen::App;
using namespace Tizen::Base;
using namespace Tizen::Base::Collection;
using namespace Tizen::System;
using namespace Tizen::Ui;
using namespace Tizen::Ui::Controls;
using namespace Tizen::Ui::Scenes;

LocationManagerApp::LocationManagerApp(void)
{
}

LocationManagerApp::~LocationManagerApp(void)
{
}

UiApp*
LocationManagerApp::CreateInstance(void)
{
	return new LocationManagerApp();
}

bool
LocationManagerApp::OnAppInitializing(AppRegistry& appRegistry)
{
	Frame* pAppFrame = new Frame();
	pAppFrame->Construct();
	AddFrame(*pAppFrame);

	// Prepare Scene management.
	SceneRegister::RegisterAllScenes();
	SceneManager* pSceneManager = SceneManager::GetInstance();
	pSceneManager->GoForward(ForwardSceneTransition(SCENE_MAIN_FORM));

	return true;
}

bool
LocationManagerApp::OnAppWillTerminate(void)
{
	return true;
}

bool
LocationManagerApp::OnAppTerminating(AppRegistry& appRegistry, bool forcedTermination)
{
	return true;
}

void
LocationManagerApp::OnForeground(void)
{
}

void
LocationManagerApp::OnBackground(void)
{
}

void
LocationManagerApp::OnLowMemory(void)
{
}

void
LocationManagerApp::OnBatteryLevelChanged(BatteryLevel batteryLevel)
{
}

void
LocationManagerApp::OnScreenOn(void)
{
}

void
LocationManagerApp::OnScreenOff(void)
{
}
