#ifndef _APP_RESOURCE_ID_H_
#define _APP_RESOURCE_ID_H_

#include <FBase.h>

extern const wchar_t* IDS_STRING1;
extern const wchar_t* ID_STRING_CREATE_POI_POPUP_TITLE;
extern const wchar_t* ID_STRING_DEFAULT_POI_DESCRIPTION;
extern const wchar_t* ID_STRING_DEFAULT_POI_TITLE;
extern const wchar_t* ID_STRING_EDIT_POI_POPUP_TITLE;
extern const wchar_t* ID_STRING_FIELD_LABEL_POI_DESCRIPTION;
extern const wchar_t* ID_STRING_FIELD_LABEL_POI_TITLE;
extern const wchar_t* ID_STRING_FIELD_POI_DESCRIPTION;
extern const wchar_t* ID_STRING_FIELD_POI_TITLE;
extern const wchar_t* ID_STRING_TIMELINE_VIEW_TITLE;
extern const wchar_t* ID_STRING_TRACK_VIEW_TITLE;
extern const wchar_t* ID_STRING_EDIT_TRACK_POPUP_TITLE;
extern const wchar_t* ID_STRING_CREATE_TRACK_POPUP_TITLE;

class I18N {
public:
	static Tizen::Base::String GetLocalizedString(const wchar_t* key);
};

#endif // _APP_RESOURCE_ID_H_
