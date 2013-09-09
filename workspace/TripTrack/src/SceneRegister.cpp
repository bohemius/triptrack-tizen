#include "SceneRegister.h"
#include "FormFactory.h"
#include <FUi.h>

using namespace Tizen::Ui::Scenes;

const wchar_t* SCENE_MAIN_FORM = L"Scene_MainForm";
const wchar_t* SCENE_POI_FORM = L"Scene_PoiForm";

SceneRegister::SceneRegister(void) {
}

SceneRegister::~SceneRegister(void) {
}

void SceneRegister::RegisterAllScenes(void) {
	static const wchar_t* PANEL_BLANK = L"";
	static FormFactory formFactory;

	SceneManager* pSceneManager = SceneManager::GetInstance();
	AppAssert(pSceneManager);
	pSceneManager->RegisterFormFactory(formFactory);

	pSceneManager->RegisterScene(SCENE_MAIN_FORM, FORM_MAIN, PANEL_BLANK);
	pSceneManager->RegisterScene(SCENE_POI_FORM, FORM_POI, PANEL_BLANK);
}
