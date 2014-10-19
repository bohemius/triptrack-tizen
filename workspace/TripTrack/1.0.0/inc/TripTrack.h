/*
 * TripTrack.h
 *
 *  Created on: Aug 15, 2013
 *      Author: hsp
 */

#ifndef TRIPTRACK_H_
#define TRIPTRACK_H_

#include <FApp.h>
#include <FBase.h>
#include <FSystem.h>
#include <FUi.h>

class TripTrackApp
	: public Tizen::App::UiApp
	, public Tizen::System::IScreenEventListener
{
public:
	static Tizen::App::UiApp* CreateInstance(void);

public:
	TripTrackApp(void);
	virtual ~TripTrackApp(void);

	bool OnAppInitializing(Tizen::App::AppRegistry& appRegistry);
	bool OnAppInitialized(void);
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

#endif /* TRIPTRACK_H_ */
