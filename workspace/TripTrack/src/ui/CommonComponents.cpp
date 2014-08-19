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
	__pFieldProvider = fieldProvider;

	VerticalBoxLayout layout;
	layout.Construct(VERTICAL_DIRECTION_DOWNWARD);

	r = Popup::Construct(layout, true, dimension);
	if (r != E_SUCCESS) {
		AppLogException("Error constructing popup: [%s]", GetErrorMessage(r));
		return r;
	}
	SetColor(Color(46, 151, 199));

	__pFieldList = fieldProvider->GetFields();
	IEnumeratorT<IFormFieldProvider::FormField*>* pEnum =
			__pFieldList->GetEnumeratorN();

	__pExTxtAreasList = new LinkedListT<EditArea*>();

	while (pEnum->MoveNext() == E_SUCCESS) {

		IFormFieldProvider::FormField* pFormField;

		r = pEnum->GetCurrent(pFormField);
		if (r != E_SUCCESS) {
			AppLogException(
					"Error getting form field from collection:[%s]", GetErrorMessage(r));
			return r;
		}

		//int deltaWidth = abs(fieldWidth - pFormField->fieldDim->width);
		//if (deltaWidth > 10)
		//	pFormField->fieldDim->width = fieldWidth;
		TextBox* pTxtLabel = new TextBox();

		pTxtLabel->Construct(Rectangle(0,0, pFormField->fieldDim->width,80), TEXT_BOX_BORDER_NONE);
		pTxtLabel->SetText(*(pFormField->fieldName));
		pTxtLabel->SetColor(TEXT_BOX_STATUS_NORMAL, Color(46, 151, 199));
		pTxtLabel->SetColor(TEXT_BOX_STATUS_HIGHLIGHTED, Color(46, 151, 199));
		pTxtLabel->SetTextColor(TEXT_BOX_TEXT_COLOR_NORMAL, Color::GetColor(COLOR_ID_WHITE));
		pTxtLabel->SetTextColor(TEXT_BOX_TEXT_COLOR_HIGHLIGHTED, Color::GetColor(COLOR_ID_WHITE));
		pTxtLabel->SetTextStyle(TEXT_BOX_TEXT_STYLE_BOLD);
		AddControl(pTxtLabel);
		layout.SetHorizontalMargin(*pTxtLabel, 20, 20);
		layout.SetSpacing(*pTxtLabel, 5);

		EditArea* pEditArea = new EditArea();

		pEditArea->Construct(
				Rectangle(0, 0, pFormField->fieldDim->width,
						pFormField->fieldDim->height), INPUT_STYLE_OVERLAY,
				pFormField->limit);
		pEditArea->SetText(*(pFormField->fieldData));
		pEditArea->SetColor(EDIT_STATUS_NORMAL, Color(46, 141, 180));
		pEditArea->SetColor(EDIT_STATUS_HIGHLIGHTED, Color(46, 151, 180));
		pEditArea->SetColor(EDIT_STATUS_PRESSED, Color(46, 141, 180));
		pEditArea->SetTextColor(EDIT_TEXT_COLOR_NORMAL, Color::GetColor(COLOR_ID_WHITE));
		pEditArea->SetTextColor(EDIT_TEXT_COLOR_HIGHLIGHTED, Color::GetColor(COLOR_ID_WHITE));
		AddControl(pEditArea);
		layout.SetHorizontalMargin(*pEditArea, 20, 20);
		layout.SetSpacing(*pEditArea, 5);
		r = __pExTxtAreasList->Add(pEditArea);
		if (r != E_SUCCESS) {
			AppLogException(
					"Error adding edit area to map:[%s]", GetErrorMessage(r));
			return r;
		}
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

