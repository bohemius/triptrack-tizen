/*
 * TripTrackForm.h
 *
 *  Created on: Aug 15, 2013
 *      Author: hsp
 */

#ifndef TRIPTRACKFORM_H_
#define TRIPTRACKFORM_H_

#include <FApp.h>
#include <FBase.h>
#include <FLocations.h>
#include <FUi.h>
#include <HMaps.h>
#include "ui/TrackComponents.h"
#include "ui/PoiComponents.h"

class LocationManagerThread;

class TripTrackForm: public Tizen::Ui::Controls::Form,
		public Tizen::Ui::IActionEventListener,
		public Tizen::Ui::Controls::IFormBackEventListener,
		public Tizen::Ui::Scenes::ISceneEventListener,
		public Tizen::App::IAppControlResponseListener {
public:
	TripTrackForm(void);
	virtual ~TripTrackForm(void);
	bool Initialize(void);

	virtual result OnInitializing(void);
	virtual result OnTerminating(void);
	virtual void OnActionPerformed(const Tizen::Ui::Control& source,
			int actionId);
	virtual void OnFormBackRequested(Tizen::Ui::Controls::Form& source);
	virtual void OnSceneActivatedN(
			const Tizen::Ui::Scenes::SceneId& previousSceneId,
			const Tizen::Ui::Scenes::SceneId& currentSceneId,
			Tizen::Base::Collection::IList* pArgs);
	virtual void OnSceneDeactivated(
			const Tizen::Ui::Scenes::SceneId& currentSceneId,
			const Tizen::Ui::Scenes::SceneId& nextSceneId);

	virtual void OnAppControlCompleteResponseReceived(
			const Tizen::App::AppId &appId,
			const Tizen::Base::String &operationId,
			Tizen::App::AppCtrlResult appControlResult,
			const Tizen::Base::Collection::IMap *pExtraData);

	virtual result OnDraw();
	void SetPoiView();
	void SetTrackView();
	void ShowEditPopUp(IFormFieldProvider* pProvider);
	void ShowMessageBox(const Tizen::Base::String& title,
			const Tizen::Base::String& message);

private:
	void LaunchLocationSettings(void);
	void OpenCamera(void);
	void ProcessCameraResult(Tizen::Base::String* imagePath);
	bool CheckLocationSetting(void);
	result LoadResources(void);

private:
	enum ViewType {
		VIEW_TYPE_NONE, VIEW_TYPE_POI_VIEW, VIEW_TYPE_TRACK_VIEW,
	} __viewType;

	static const int ID_HEADER_TAB_POI_VIEW = 201;
	static const int ID_HEADER_TAB_TRACK_VIEW = 202;

	static const int ID_FOOTER_BUTTON_ADD_POI = 203;
	static const int ID_FOOTER_BUTTON_CAMERA_POI = 204;
	static const int ID_FOOTER_BUTTON_ADD_TRACK = 205;
	static const int ID_FOOTER_BUTTON_POI_LOCATIONS = 206;

	Tizen::Graphics::Bitmap* __pDeleteBitmap;
	Tizen::Graphics::Bitmap* __pEditBitmap;
	Tizen::Graphics::Bitmap* __pCameraBitmap;
	Tizen::Graphics::Bitmap* __pLocationBitmap;
	Tizen::Graphics::Bitmap* __pAddBitmap;
	Tizen::Graphics::Bitmap* __pBgBitmap;

	TrackListPanel* __pTrackListPanel;
	PoiIconListPanel* __pPoiListPanel;
	Tizen::Ui::Controls::ProgressPopup* __pProgressPopup;
	TrackerManager* __pTrackerManager;
};

#endif /* TRIPTRACKFORM_H_ */
