#include "FormFactory.h"
#include "TripTrackForm.h"
#include "ui/PoiForm.h"
#include "ui/MapForm.h"
#include "ui/GalleryForm.h"
#include "ui/PoiMapForm.h"
#include "social/FacebookForm.h"

using namespace Tizen::Ui::Scenes;
using namespace Tizen::Ui::Controls;

const wchar_t* FORM_MAIN = L"Form_Main";
const wchar_t* FORM_POI = L"Form_Poi";
const wchar_t* FORM_MAP = L"Form_Map";
const wchar_t* FORM_GALLERY = L"Form_Gallery";
const wchar_t* FORM_FACEBOOK = L"Form_Facebook";
const wchar_t* FORM_POIMAP = L"Form_PoiMap";

FormFactory::FormFactory(void) {
}

FormFactory::~FormFactory(void) {
}

Tizen::Ui::Controls::Form*
FormFactory::CreateFormN(const Tizen::Base::String& formId,
		const Tizen::Ui::Scenes::SceneId& sceneId) {
	SceneManager* pSceneManager = SceneManager::GetInstance();
	AppAssert(pSceneManager);
	Tizen::Ui::Controls::Form* pNewForm = null;

	if (formId == FORM_MAIN) {
		TripTrackForm* pForm = new TripTrackForm();
		pForm->Initialize();
		pSceneManager->AddSceneEventListener(sceneId, *pForm);
		pNewForm = pForm;
	} else if (formId == FORM_POI) {
		PoiForm* pForm = new PoiForm();
		pForm->Initialize();
		pSceneManager->AddSceneEventListener(sceneId, *pForm);
		pNewForm = pForm;
	} else if (formId == FORM_MAP) {
		MapForm* pForm = new MapForm();
		pForm->Construct(FORM_STYLE_NORMAL);
		pSceneManager->AddSceneEventListener(sceneId, *pForm);
		pNewForm = pForm;
	} else if (formId == FORM_GALLERY) {
		GalleryForm* pForm = new GalleryForm();
		pForm->Initialize();
		pSceneManager->AddSceneEventListener(sceneId, *pForm);
		pNewForm = pForm;
	} else if (formId == FORM_FACEBOOK) {
		FacebookForm* pForm = new FacebookForm();
		pForm->Initialize();
		pSceneManager->AddSceneEventListener(sceneId, *pForm);
		pNewForm = pForm;
	} else if (formId == FORM_POIMAP) {
		PoiMapForm* pForm = new PoiMapForm();
		pForm->Construct(FORM_STYLE_NORMAL);
		pSceneManager->AddSceneEventListener(sceneId, *pForm);
		pNewForm = pForm;
	}

	return pNewForm;
}
