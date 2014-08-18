/*
 * CommonComponents.cpp
 *
 *  Created on: Oct 3, 2013
 *      Author: bohemius
 */
#include "ui/CommonComponents.h"
#include <math.h>
#include <FGraphics.h>

using namespace Tizen::Ui::Controls;
using namespace Tizen::Base::Collection;
using namespace Tizen::Graphics;
using namespace Tizen::Base;
using namespace Tizen::Ui;

EditFormPopup::EditFormPopup(void) :
		__pFieldList(null), __pExTxtAreasList(null), __pFieldProvider(null) {
}

EditFormPopup::~EditFormPopup(void) {
	__pExTxtAreasList->RemoveAll();
	delete (__pExTxtAreasList);
}

result EditFormPopup::Construct(IFormFieldProvider* fieldProvider,
		Dimension dimension, String title) {
	result r = E_SUCCESS;
	__pFieldProvider=fieldProvider;

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

	__pFieldList = fieldProvider->GetFields();
	IEnumeratorT<IFormFieldProvider::FormField*>* pEnum =
			__pFieldList->GetEnumeratorN();

	__pExTxtAreasList = new LinkedListT<EditArea*>();

	y = yMargin;
	while (pEnum->MoveNext() == E_SUCCESS) {

		IFormFieldProvider::FormField* pFormField;

		r = pEnum->GetCurrent(pFormField);
		if (r != E_SUCCESS) {
			AppLogException(
					"Error getting form field from collection:[%s]", GetErrorMessage(r));
			return r;
		}

		int deltaWidth = abs(fieldWidth - pFormField->fieldDim->width);
		if (deltaWidth > 10)
			pFormField->fieldDim->width = fieldWidth;

		EditArea* pEditArea = new EditArea();

		pEditArea->Construct(
				Rectangle(xMargin, y, pFormField->fieldDim->width,
						pFormField->fieldDim->height), INPUT_STYLE_OVERLAY,
				pFormField->limit);
		//pEditArea->SetTitleText(*(pFormField->fieldName));
		pEditArea->SetText(*(pFormField->fieldData));
		AddControl(pEditArea);
		r = __pExTxtAreasList->Add(pEditArea);
		if (r != E_SUCCESS) {
			AppLogException(
					"Error adding edit area to map:[%s]", GetErrorMessage(r));
			return r;
		}

		y += fieldHeight + yMargin;
	}

	SetPropagatedKeyEventListener(this);
	SetTitleText(title);
	return r;
}

bool EditFormPopup::OnKeyPressed(Tizen::Ui::Control& source,
		const Tizen::Ui::KeyEventInfo& keyEventInfo) {
	return false;
}

bool EditFormPopup::OnKeyReleased(Tizen::Ui::Control& source,
		const Tizen::Ui::KeyEventInfo& keyEventInfo) {
	KeyCode key = keyEventInfo.GetKeyCode();
	if (key == KEY_BACK || key == KEY_ESC) {

		//EditFormPopup* pPopup = static_cast<EditFormPopup*>(&source);
		//pPopup->SetShowState(false);
		//pPopup->Invalidate(true);

		this->SetShowState(false);
		this->Invalidate(true);
	}
}

bool EditFormPopup::OnPreviewKeyPressed(Tizen::Ui::Control& source,
		const Tizen::Ui::KeyEventInfo& keyEventInfo) {
	return false;
}

bool EditFormPopup::OnPreviewKeyReleased(Tizen::Ui::Control& source,
		const Tizen::Ui::KeyEventInfo& keyEventInfo) {
	return false;
}

bool EditFormPopup::TranslateKeyEventInfo(Tizen::Ui::Control& source,
		Tizen::Ui::KeyEventInfo& keyEventInfo) {
	return false;
}

