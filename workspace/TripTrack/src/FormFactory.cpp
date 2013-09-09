#include "FormFactory.h"
#include "TripTrackForm.h"
#include "ui/PoiForm.h"

using namespace Tizen::Ui::Scenes;

const wchar_t* FORM_MAIN = L"Form_Main";
const wchar_t* FORM_POI = L"Form_Poi";

FormFactory::FormFactory(void)
{
}

FormFactory::~FormFactory(void)
{
}

Tizen::Ui::Controls::Form*
FormFactory::CreateFormN(const Tizen::Base::String& formId, const Tizen::Ui::Scenes::SceneId& sceneId)
{
	SceneManager* pSceneManager = SceneManager::GetInstance();
	AppAssert(pSceneManager);
	Tizen::Ui::Controls::Form* pNewForm = null;

	if (formId == FORM_MAIN)
	{
		TripTrackForm* pForm = new TripTrackForm();
		pForm->Initialize();
		pSceneManager->AddSceneEventListener(sceneId, *pForm);
		pNewForm = pForm;
	}

	else if (formId == FORM_POI) {
		PoiForm* pForm = new PoiForm();
		pForm->Initialize();
		pSceneManager->AddSceneEventListener(sceneId, *pForm);
		pNewForm = pForm;
	}

	return pNewForm;
}
