/*
 * CommonComponents.cpp
 *
 *  Created on: Oct 3, 2013
 *      Author: bohemius
 */
#include "ui/CommonComponents.h"
#include <FGraphics.h>

using namespace Tizen::Ui::Controls;
using namespace Tizen::Base::Collection;
using namespace Tizen::Graphics;
using namespace Tizen::Base;
using namespace Tizen::Ui;

EditFormPopup::EditFormPopup(void) :
		__pFieldMap(null), __pExTxtAreasMap(null), __pFieldProvider(null), __pPopupEventListener(
				null) {
}

EditFormPopup::~EditFormPopup(void) {
	__pExTxtAreasMap->RemoveAll();
	delete (__pExTxtAreasMap);
}

result EditFormPopup::Construct(IFormFieldProvider* fieldProvider,
		Dimension dimension, String title) {
	result r = E_SUCCESS;

	r = Popup::Construct(true, dimension);
	if (r != E_SUCCESS) {
		AppLogException("Error constructing popup: [%s]", GetErrorMessage(r));
		return r;
	}

	const int xMargin = 20;
	const int yMargin = (int) dimension.height / 6 * 0.20;
	const int fieldHeight = 120;
	const int fieldWidth = (int) dimension.width - 2 * xMargin;
	const int buttonWidth = (int) dimension.width / 2 - 2 * xMargin;
	int y = 0;

	__pFieldMap = fieldProvider->GetFields();
	IEnumeratorT<int>* pEnum = __pFieldMap->GetKeysN()->GetEnumeratorN();

	__pExTxtAreasMap = new HashMapT<int, ExpandableEditArea*>();
	__pExTxtAreasMap->Construct(5, 10);

	y = yMargin;
	while (pEnum->MoveNext() == E_SUCCESS) {
		int key;

		r = pEnum->GetCurrent(key);
		if (r != E_SUCCESS) {
			AppLogException(
					"Error getting key from collection:[%s]", GetErrorMessage(r));
			return r;
		}

		IFormFieldProvider::FormField* pFormField;

		r = __pFieldMap->GetValue(key, pFormField);
		if (r != E_SUCCESS) {
			AppLogException(
					"Error getting form field for key [%d] from collection:[%s]", key, GetErrorMessage(r));
			return r;
		}

		ExpandableEditArea* pEditArea = new ExpandableEditArea();

		pEditArea->Construct(Rectangle(xMargin, y, fieldWidth, fieldHeight),
				EXPANDABLE_EDIT_AREA_STYLE_NORMAL,
				EXPANDABLE_EDIT_AREA_TITLE_STYLE_TOP, 5);
		pEditArea->SetTitleText(*(pFormField->fieldName));
		pEditArea->SetText(*(pFormField->fieldData));
		AddControl(pEditArea);
		r = __pExTxtAreasMap->Add(key, pEditArea);
		if (r != E_SUCCESS) {
			AppLogException(
					"Error adding edit area with key [%d] to map:[%s]", key, GetErrorMessage(r));
			return r;
		}

		y += fieldHeight + yMargin;
	}

	__pPopupEventListener = new PopupEventListener();
	SetPropagatedKeyEventListener(__pPopupEventListener);
	SetTitleText(title);
	return r;
}

result EditFormPopup::Save(void) {
	result r = E_SUCCESS;
	IEnumeratorT<int>* pEnum = __pExTxtAreasMap->GetKeysN()->GetEnumeratorN();

	while (pEnum->MoveNext() == E_SUCCESS) {
		int key;
		r = pEnum->GetCurrent(key);
		if (r != E_SUCCESS) {
			AppLogException(
					"Error getting key from collection:[%s]", GetErrorMessage(r));
			return r;
		}

		IFormFieldProvider::FormField* pFormField = null;

		r = __pFieldMap->GetValue(key, pFormField);
		if (r != E_SUCCESS) {
			AppLogException(
					"Error getting form field for key [%d] from collection:[%s]", key, GetErrorMessage(r));
			return r;
		}

		ExpandableEditArea* pEditArea = null;

		r = __pExTxtAreasMap->GetValue(key, pEditArea);
		if (r != E_SUCCESS) {
			AppLogException(
					"Error getting edit area for key [%d] from collection:[%s]", key, GetErrorMessage(r));
			return r;
		}

		pFormField->fieldData->Clear();
		pFormField->fieldData->Append(pEditArea->GetText());
		r = __pFieldProvider->SetFieldAt(key, pFormField);
		if (r != E_SUCCESS) {
			AppLogException(
					"Error setting field data for key [%d] in field provider:[%s]", key, GetErrorMessage(r));
			return r;
		}
	}
	return r;
}

void EditFormPopup::OnFormBackRequested(Tizen::Ui::Controls::Form& source) {
	SetShowState(false);
	Invalidate(true);
}

const int SampleFieldProvider::FORM_FIELD_ID_DESCRIPTION = 2;
const int SampleFieldProvider::FORM_FIELD_ID_TITLE = 1;

SampleFieldProvider::SampleFieldProvider(void) :
		__pFormFields(null) {
}

SampleFieldProvider::~SampleFieldProvider(void) {
	if (__pFormFields != null) {
		__pFormFields->RemoveAll();
		delete __pFormFields;
		__pFormFields = null;
	}
}

IFormFieldProvider::FormField* SampleFieldProvider::GetFieldAt(int id) {
	result r = E_SUCCESS;

	FormField* retVal = null;
	r = __pFormFields->GetValue(id, retVal);
	if (r != E_SUCCESS) {
		AppLogException(
				"Error getting field with id [%d] from field map: [%s]", id, GetErrorMessage(r));
		return null;
	}
	return retVal;
}

HashMapT<int, IFormFieldProvider::FormField*>* SampleFieldProvider::GetFields(
		void) {
	return __pFormFields;
}

result SampleFieldProvider::SetFieldAt(int id, FormField* formField) {
	result r = E_SUCCESS;

	r = __pFormFields->SetValue(id, formField);
	if (r != E_SUCCESS)
		AppLogException(
				"Error setting field with id [%d] in field map: [%s]", id, GetErrorMessage(r));
	return r;
}

result SampleFieldProvider::Construct(void) {
	result r = E_SUCCESS;

	__pFormFields = new HashMapT<int, FormField*>();
	r = __pFormFields->Construct(2, 1.5);
	if (r != E_SUCCESS) {
		AppLogException(
				"Error constructing field map: [%s]", GetErrorMessage(r));
		return r;
	}

	FormField* pTitleField = new FormField;
	pTitleField->fieldName = new String(L"Title");
	pTitleField->fieldData = new String(L"");
	r = __pFormFields->Add(FORM_FIELD_ID_TITLE, pTitleField);
	if (r != E_SUCCESS) {
		AppLogException( "Error adding to field map: [%s]", GetErrorMessage(r));
		return r;
	}

	FormField* pDescField = new FormField;
	pDescField->fieldName = new String(L"Description");
	pDescField->fieldData = new String(L"");
	__pFormFields->Add(FORM_FIELD_ID_DESCRIPTION, pDescField);
	if (r != E_SUCCESS) {
		AppLogException( "Error adding to field map: [%s]", GetErrorMessage(r));
		return r;
	}
}

int SampleFieldProvider::GetFieldCount(void) {
	return __pFormFields->GetCount();
}

bool PopupEventListener::OnKeyReleased(Tizen::Ui::Control& source,
		const Tizen::Ui::KeyEventInfo& keyEventInfo) {
	KeyCode key = keyEventInfo.GetKeyCode();
	if (key == KEY_BACK || key == KEY_ESC) {
		EditFormPopup* pPopup = static_cast<EditFormPopup*>(&source);
		pPopup->SetShowState(false);
		pPopup->Invalidate(true);
	}

	return false;
}

