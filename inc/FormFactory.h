#ifndef _FORM_FACTORY_H_
#define _FORM_FACTORY_H_

#include <FUi.h>

extern const wchar_t* FORM_MAIN;
extern const wchar_t* FORM_POI;
extern const wchar_t* FORM_MAP;
extern const wchar_t* FORM_GALLERY;
extern const wchar_t* FORM_FACEBOOK;
extern const wchar_t* FORM_POIMAP;

class FormFactory
	: public Tizen::Ui::Scenes::IFormFactory
{
public:
	FormFactory(void);
	virtual ~FormFactory(void);

	virtual Tizen::Ui::Controls::Form* CreateFormN(const Tizen::Base::String& formId, const Tizen::Ui::Scenes::SceneId& sceneId);
};

#endif // _FORM_FACTORY_H_
