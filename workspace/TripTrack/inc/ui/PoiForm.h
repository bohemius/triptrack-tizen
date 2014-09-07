/*
 * PoiForm.h
 *
 *  Created on: Sep 2, 2013
 *      Author: bohemius
 */

#ifndef POIFORM_H_
#define POIFORM_H_

#include <FUiCtrlForm.h>
#include <FUi.h>
#include "dao/POI.h"
#include "ui/CommonComponents.h"

class PoiForm: public Tizen::Ui::Controls::Form,
		public Tizen::Ui::IActionEventListener,
		public Tizen::Ui::Controls::IFormBackEventListener,
		public Tizen::Ui::Scenes::ISceneEventListener,
		public Tizen::Ui::Controls::IIconListViewItemProvider,
		public Tizen::Ui::Controls::IIconListViewItemEventListener,
		public Tizen::App::IAppControlResponseListener,
		public Tizen::Ui::ITouchEventListener,
		public Tizen::Ui::ITouchLongPressGestureEventListener,
		public IOnDataChangedListener {
public:
	PoiForm();
	bool Initialize(void);
	virtual ~PoiForm();

	virtual result OnInitializing(void);
	virtual result OnTerminating(void);
	virtual void OnActionPerformed(const Tizen::Ui::Control& source,
			int actionId);
	virtual void OnFormBackRequested(Tizen::Ui::Controls::Form& source);
	virtual result OnDraw();
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
	virtual void OnIconListViewItemStateChanged(
			Tizen::Ui::Controls::IconListView &view, int index,
			Tizen::Ui::Controls::IconListViewItemStatus status);
	virtual Tizen::Ui::Controls::IconListViewItem* CreateItem(int index);
	virtual void OnIconListViewOverlayBitmapSelected(
			Tizen::Ui::Controls::IconListView& iconListView, int index,
			int overlayBitmapId);
	virtual bool DeleteItem(int index,
			Tizen::Ui::Controls::IconListViewItem* pItem);
	virtual int GetItemCount(void);

	virtual result Update(void);

	// ITouchEventListener
	virtual void OnTouchPressed(const Tizen::Ui::Control &source,
			const Tizen::Graphics::Point &currentPosition,
			const Tizen::Ui::TouchEventInfo &touchInfo);
	virtual void OnTouchFocusIn(const Tizen::Ui::Control &source,
			const Tizen::Graphics::Point &currentPosition,
			const Tizen::Ui::TouchEventInfo &touchInfo) {
	}
	;
	virtual void OnTouchFocusOut(const Tizen::Ui::Control &source,
			const Tizen::Graphics::Point &currentPosition,
			const Tizen::Ui::TouchEventInfo &touchInfo) {
	}
	;
	virtual void OnTouchMoved(const Tizen::Ui::Control &source,
			const Tizen::Graphics::Point &currentPosition,
			const Tizen::Ui::TouchEventInfo &touchInfo) {
	}
	;
	virtual void OnTouchReleased(const Tizen::Ui::Control &source,
			const Tizen::Graphics::Point &currentPosition,
			const Tizen::Ui::TouchEventInfo &touchInfo) {
	}
	;
	virtual void OnTouchDoublePressed(const Tizen::Ui::Control &source,
			const Tizen::Graphics::Point &currentPosition,
			const Tizen::Ui::TouchEventInfo &touchInfo);

	// ITouchLongPressGestureEventListener
	virtual void OnLongPressGestureDetected(
			Tizen::Ui::TouchLongPressGestureDetector& gestureDetector);
	virtual void OnLongPressGestureCanceled(
			Tizen::Ui::TouchLongPressGestureDetector &gestureDetector) {
	}
	;

private:
	result LoadResources(void);
	result LoadImageList(void);
	void ShowEditPopup(void);
	void OpenCamera(void);
	void ProcessCameraResult(Tizen::Base::String* imagePath);
	TTMedia* GetMediaFromClick(Tizen::Graphics::Point point);
	Tizen::Graphics::Bitmap* CreateTitleBitmap(void);

private:
	static const int ID_FOOTER_BUTTON_CAMERA = 200;
	static const int ID_FOOTER_BUTTTON_EDIT = 201;
	static const int ID_FOOTER_BUTTON_MAP = 202;
	static const int ID_FOOTER_BUTTON_FB = 203;

	static const int TITLE_PADDING_X = 30;
	static const int TITLE_PADDING_Y = 40;
	static const int TITLE_TEXT_SIZE = 70;

	static const int DESCRIPTION_PADDING_X = 30;
	static const int DESCRIPTION_PADDING_Y = 30;
	static const int DESCRIPTION_TEXT_SIZE = 30;

	static const float TILES_PER_ROW = 3.0f;
	static const float TILES_SPACING_X = 20.0f;
	static const float TILES_SPACING_Y = 30.0f;
	static const float TILES_OFFSET_X = 0.0f;
	static const float TILES_OFFSET_Y = 30.0f;

	static const int ID_OVERLAY_BITMAP_DELETE = 801;

	Tizen::Graphics::Bitmap* __pEditBitmap;
	Tizen::Graphics::Bitmap* __pDeleteBitmap;
	Tizen::Graphics::Bitmap* __pCameraBitmap;
	Tizen::Graphics::Bitmap* __pLocationBitmap;
	Tizen::Graphics::Bitmap* __pFbBitmap;

	Tizen::Graphics::Bitmap* __pBgBitmap;

	Tizen::Graphics::Rectangle* __pDescRect;
	Tizen::Graphics::Rectangle* __pTitleRect;
	Tizen::Graphics::FloatRectangle* __pListRect;
	Tizen::Graphics::Rectangle* __pClientBounds;
	Tizen::Ui::Controls::Label* __pTitleLabel;
	Tizen::Ui::Controls::Label* __pDescriptionLabel;
	Tizen::Ui::Controls::Panel* __pPoiPanel;
	Tizen::Ui::Controls::IconListView* __pMediaIconListView;
	Tizen::Ui::TouchLongPressGestureDetector* __pLongPressDetector;
	Tizen::Graphics::Point __lastPosition;

	POI* __pPoi;

	float tile_width, tile_height;
};

#endif /* POIFORM_H_ */
