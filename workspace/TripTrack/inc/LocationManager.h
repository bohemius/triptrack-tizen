#ifndef _LOCATION_MANAGER_H_
#define _LOCATION_MANAGER_H_

#include <FApp.h>
#include <FBase.h>
#include <FSystem.h>
#include <FUi.h>

class LocationManagerApp
	: public Tizen::App::UiApp
	, public Tizen::System::IScreenEventListener
{
public:
	static Tizen::App::UiApp* CreateInstance(void);

public:
	LocationManagerApp(void);
	virtual~LocationManagerApp(void);

	bool OnAppInitializing(Tizen::App::AppRegistry& appRegistry);
	bool OnAppWillTerminate(void); 
	bool OnAppTerminating(Tizen::App::AppRegistry& appRegistry, bool forcedTermination = false);
	void OnForeground(void);
	void OnBackground(void);
	void OnLowMemory(void);
	void OnBatteryLevelChanged(Tizen::System::BatteryLevel batteryLevel);
	void OnScreenOn(void);
	void OnScreenOff(void);

private:
	int __actionId;
};

#endif //  _LOCATION_MANAGER_H_
