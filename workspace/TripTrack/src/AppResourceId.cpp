#include "AppResourceId.h"
#include <FApp.h>

using namespace Tizen::App;
using namespace Tizen::Base;

const wchar_t* IDS_STRING1 = L"IDS_STRING1";
const wchar_t* ID_STRING_CREATE_POI_POPUP_TITLE = L"ID_STRING_CREATE_POI_POPUP_TITLE";
const wchar_t* ID_STRING_CREATE_TRACK_POPUP_TITLE = L"ID_STRING_CREATE_TRACK_POPUP_TITLE";
const wchar_t* ID_STRING_DEFAULT_POI_DESCRIPTION = L"ID_STRING_DEFAULT_POI_DESCRIPTION";
const wchar_t* ID_STRING_DEFAULT_POI_TITLE = L"ID_STRING_DEFAULT_POI_TITLE";
const wchar_t* ID_STRING_EDIT_POI_POPUP_TITLE = L"ID_STRING_EDIT_POI_POPUP_TITLE";
const wchar_t* ID_STRING_FIELD_LABEL_POI_DESCRIPTION = L"ID_STRING_FIELD_LABEL_POI_DESCRIPTION";
const wchar_t* ID_STRING_FIELD_LABEL_POI_TITLE = L"ID_STRING_FIELD_LABEL_POI_TITLE";
const wchar_t* ID_STRING_FIELD_POI_DESCRIPTION = L"ID_STRING_FIELD_POI_DESCRIPTION";
const wchar_t* ID_STRING_FIELD_POI_TITLE = L"ID_STRING_FIELD_POI_TITLE";
const wchar_t* ID_STRING_TIMELINE_VIEW_TITLE = L"ID_STRING_TIMELINE_VIEW_TITLE";
const wchar_t* ID_STRING_TRACK_VIEW_TITLE = L"ID_STRING_TRACK_VIEW_TITLE";
const wchar_t* ID_STRING_EDIT_TRACK_POPUP_TITLE = L"ID_STRING_EDIT_TRACK_POPUP_TITLE";

String I18N::GetLocalizedString(const wchar_t* key) {
	result r=E_SUCCESS;
	String retVal;

	r = Application::GetInstance()->GetAppResource()->GetString(key, retVal);
	if (r != E_SUCCESS) {
		retVal = L"NA";
		AppLogException("Error getting localized String for key: [%ls]", key);
	}
	return retVal;
}
