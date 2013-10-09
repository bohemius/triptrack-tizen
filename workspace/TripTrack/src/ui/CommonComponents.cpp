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

EditFormPopup::EditFormPopup(void) :
		__pFieldMap(null), __pExTxtAreasMap(null), __pFieldProvider(null) {
}

EditFormPopup::~EditFormPopup(void) {
	__pExTxtAreasMap->RemoveAll();
	delete (__pExTxtAreasMap);
}

result EditFormPopup::Construct(IFormFieldProvider* fieldProvider,
		Dimension dimension) {
	result r = E_SUCCESS;

	r = Popup::Construct(true, dimension);
	if (r != E_SUCCESS) {
		AppLogException("Error constructing popup: [%s]", GetErrorMessage(r));
		return r;
	}

	const int xMargin = 10;
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
