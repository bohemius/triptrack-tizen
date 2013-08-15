#ifndef _APP_RESOURCE_ID_H_
#define _APP_RESOURCE_ID_H_

#include <FBase.h>

extern const wchar_t* IDS_HELLO;
extern const wchar_t* IDS_STRING1;
extern const wchar_t* ID_STRING_TIMELINE_VIEW_TITLE;
extern const wchar_t* ID_STRING_TRACK_VIEW_TITLE;

class I18N {
public:
	static Tizen::Base::String GetLocalizedString(const wchar_t* key);
};

#endif // _APP_RESOURCE_ID_H_
