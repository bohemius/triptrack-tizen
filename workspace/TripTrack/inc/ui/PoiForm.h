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

class PoiForm: public Tizen::Ui::Controls::Form,
		public Tizen::Ui::IActionEventListener,
		public Tizen::Ui::Controls::IFormBackEventListener,
		public Tizen::Ui::Scenes::ISceneEventListener {
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

private:
	result LoadResources(void);
	result LoadImageList(void);

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

	Tizen::Graphics::Bitmap* __pEditBitmap;
	Tizen::Graphics::Bitmap* __pCameraBitmap;
	Tizen::Graphics::Bitmap* __pLocationBitmap;
	Tizen::Graphics::Bitmap* __pFbBitmap;

	Tizen::Graphics::Bitmap* __pBgBitmap;

	Tizen::Graphics::Rectangle* __pDescRect;
	Tizen::Graphics::Rectangle* __pTitleRect;
	Tizen::Ui::Controls::Label* __pTitleLabel;
	Tizen::Ui::Controls::Label* __pDescriptionLabel;
	Tizen::Ui::Controls::IconListView* __pMediaIconListView;

	POI* __pPoi;
};

#endif /* POIFORM_H_ */
