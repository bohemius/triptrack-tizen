#ifndef _SCENE_REGISTER_H_
#define _SCENE_REGISTER_H_

extern const wchar_t* SCENE_MAIN_FORM;
extern const wchar_t* SCENE_POI_FORM;
extern const wchar_t* SCENE_MAP_FORM;
extern const wchar_t* SCENE_GALLERY_FORM;
extern const wchar_t* SCENE_FACEBOOK_FORM;

class SceneRegister
{
public:
	static void RegisterAllScenes(void);

private:
	SceneRegister(void);
	~SceneRegister(void);
};

#endif // _SCENE_REGISTER_H_
