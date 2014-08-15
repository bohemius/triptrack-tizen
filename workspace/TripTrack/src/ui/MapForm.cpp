/*
 * MapForm.cpp
 *
 *  Created on: Aug 7, 2014
 *      Author: bohemius
 */

#include "ui/MapForm.h"
#include "SceneRegister.h"

using namespace Tizen::Ui::Controls;
using namespace Tizen::Ui::Scenes;
using namespace HMaps;

MapForm::MapForm() {
	// TODO Auto-generated constructor stub

}

MapForm::~MapForm() {
	delete __pMap;
	__pMap = null;
}

bool MapForm::Initialize(void) {
	result r = Form::Construct(FORM_STYLE_NORMAL);
	if (IsFailed(r)) {
		return false;
	}
	return true;
}

result MapForm::OnInitializing(void) {
	result r = E_SUCCESS;

	Map*__pMap = new (std::nothrow) Map();
	r = __pMap->Construct(GetClientAreaBounds().width,
			GetClientAreaBounds().height);
	if (r != E_SUCCESS) {
		AppLogException(
				"Error constructing map control: [%s]", GetErrorMessage(r));
		return r;
	}
	AddControl(__pMap);

	SetFormBackEventListener(this);

	return r;
}

result MapForm::OnTerminating(void) {
}

void MapForm::OnFormBackRequested(Tizen::Ui::Controls::Form& source) {
	SceneManager* pSceneMngr = SceneManager::GetInstance();
	pSceneMngr->GoBackward(BackwardSceneTransition(SCENE_MAIN_FORM));
}

void MapForm::OnSceneActivatedN(
		const Tizen::Ui::Scenes::SceneId& previousSceneId,
		const Tizen::Ui::Scenes::SceneId& currentSceneId,
		Tizen::Base::Collection::IList* pArgs) {
}

void MapForm::OnSceneDeactivated(
		const Tizen::Ui::Scenes::SceneId& currentSceneId,
		const Tizen::Ui::Scenes::SceneId& nextSceneId) {
}

