/*
 * CommonComponents.cpp
 *
 *  Created on: Oct 3, 2013
 *      Author: bohemius
 */
#include "ui/CommonComponents.h"
#include <math.h>
#include <FGraphics.h>
#include <geo/TrackerManager.h>

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
	__pButtonPanel->RemoveAllControls();
	delete (__pButtonPanel);
}

result EditFormPopup::Construct(IFormFieldProvider* fieldProvider,
		IOnDataChangedListener* resultListener, Dimension dimension,
		String title) {
	result r = E_SUCCESS;

	__pFieldProvider = fieldProvider;
	__pOnDataChangedListener = resultListener;

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

		TextBox* pTxtLabel = new TextBox();

		pTxtLabel->Construct(Rectangle(0, 0, pFormField->fieldDim.width, 80),
				TEXT_BOX_BORDER_NONE);
		pTxtLabel->SetText(pFormField->fieldName);
		pTxtLabel->SetColor(TEXT_BOX_STATUS_NORMAL, Color(46, 151, 199));
		pTxtLabel->SetColor(TEXT_BOX_STATUS_HIGHLIGHTED, Color(46, 151, 199));
		pTxtLabel->SetTextColor(TEXT_BOX_TEXT_COLOR_NORMAL,
				Color::GetColor(COLOR_ID_WHITE));
		pTxtLabel->SetTextColor(TEXT_BOX_TEXT_COLOR_HIGHLIGHTED,
				Color::GetColor(COLOR_ID_WHITE));
		pTxtLabel->SetTextStyle(TEXT_BOX_TEXT_STYLE_BOLD);
		AddControl(pTxtLabel);
		layout.SetHorizontalMargin(*pTxtLabel, 20, 20);
		layout.SetSpacing(*pTxtLabel, 5);

		EditArea* pEditArea = new EditArea();

		pEditArea->Construct(
				Rectangle(0, 0, pFormField->fieldDim.width,
						pFormField->fieldDim.height), INPUT_STYLE_OVERLAY,
				pFormField->limit);
		pEditArea->SetText(pFormField->fieldData);
		pEditArea->SetColor(EDIT_STATUS_NORMAL, Color(46, 141, 180));
		pEditArea->SetColor(EDIT_STATUS_HIGHLIGHTED, Color(46, 151, 180));
		pEditArea->SetColor(EDIT_STATUS_PRESSED, Color(46, 141, 180));
		pEditArea->SetTextColor(EDIT_TEXT_COLOR_NORMAL,
				Color::GetColor(COLOR_ID_WHITE));
		pEditArea->SetTextColor(EDIT_TEXT_COLOR_HIGHLIGHTED,
				Color::GetColor(COLOR_ID_WHITE));
		AddControl(pEditArea);
		layout.SetHorizontalMargin(*pEditArea, 20, 20);
		layout.SetSpacing(*pEditArea, 5);

		r = __pExTxtAreasList->Add(pEditArea);
		if (r != E_SUCCESS) {
			AppLogException(
					"Error adding edit area to list:[%s]", GetErrorMessage(r));
			return r;
		}
	}

	//add buttons
	HorizontalBoxLayout buttonLayout;
	buttonLayout.Construct(HORIZONTAL_DIRECTION_RIGHTWARD);

	__pButtonPanel = new Panel();
	__pButtonPanel->Construct(buttonLayout, Rectangle(0, 0, 400, 80),
			GROUP_STYLE_NONE);

	__pSaveButton = new Button();
	__pSaveButton->Construct(Rectangle(0, 0, 160, 60), L"Save");
	__pSaveButton->SetActionId(ID_BUTTON_POPUP_SAVE);
	__pSaveButton->SetColor(BUTTON_STATUS_NORMAL, Color(46, 141, 180));
	__pSaveButton->SetTextColor(Color::GetColor(COLOR_ID_WHITE));
	__pSaveButton->AddActionEventListener(*this);
	__pButtonPanel->AddControl(__pSaveButton);
	buttonLayout.SetVerticalMargin(*__pSaveButton, 20, 20);
	buttonLayout.SetSpacing(*__pSaveButton, 10);

	__pCancelButton = new Button();
	__pCancelButton->Construct(Rectangle(0, 0, 160, 60), L"Cancel");
	__pCancelButton->SetActionId(ID_BUTTON_POPUP_CANCEL);
	__pCancelButton->SetColor(BUTTON_STATUS_NORMAL, Color(46, 141, 180));
	__pCancelButton->SetTextColor(Color::GetColor(COLOR_ID_WHITE));
	__pCancelButton->AddActionEventListener(*this);
	__pButtonPanel->AddControl(__pCancelButton);
	buttonLayout.SetVerticalMargin(*__pCancelButton, 20, 20);
	buttonLayout.SetSpacing(*__pCancelButton, 10);

	AddControl(__pButtonPanel);
	layout.SetHorizontalAlignment(*__pButtonPanel,
			LAYOUT_HORIZONTAL_ALIGN_CENTER);

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

void EditFormPopup::OnActionPerformed(const Tizen::Ui::Control& source,
		int actionId) {
	switch (actionId) {
	case ID_BUTTON_POPUP_SAVE: {
		IEnumeratorT<EditArea*>* pEnum = __pExTxtAreasList->GetEnumeratorN();
		int i = 0;

		while (pEnum->MoveNext() == E_SUCCESS) {
			EditArea* pEdit;
			pEnum->GetCurrent(pEdit);
			IFormFieldProvider::FormField* pField;
			__pFieldList->GetAt(i, pField);
			pField->fieldData = String(pEdit->GetText());
			i++;
		}

		__pFieldProvider->SaveFields(__pFieldList);
		__pOnDataChangedListener->Update();

		this->SetShowState(false);
		this->Invalidate(true);

		//TrackerManager::getInstance()->AddTracker(title, desc);
		delete pEnum;

	}
		break;
	case ID_BUTTON_POPUP_CANCEL: {
		this->SetShowState(false);
		this->Invalidate(true);
	}
		break;
	}
}

HMapsFieldProvider::HMapsFieldProvider(String & fromCity, String & fromDetail) {
	__pFieldList = new LinkedListT<IFormFieldProvider::FormField*>();

	//TODO localize this
	IFormFieldProvider::FormField* pTitleField =
			new IFormFieldProvider::FormField();
	pTitleField->fieldName = String(L"Title");
	pTitleField->fieldData = String(fromCity);
	pTitleField->id = 1;
	pTitleField->limit = 255;
	pTitleField->fieldDim = Dimension(600, 80);

	__pFieldList->Add(pTitleField);

	IFormFieldProvider::FormField* pDescField =
			new IFormFieldProvider::FormField();
	pDescField->fieldName = String(L"Description");
	pDescField->fieldData = String(fromDetail);
	pDescField->id = 2;
	pDescField->limit = 1000;
	pDescField->fieldDim = Dimension(600, 400);

	__pFieldList->Add(pDescField);

}

HMapsFieldProvider::~HMapsFieldProvider(void) {
	__pFieldList->RemoveAll();
	delete __pFieldList;
}

LinkedListT<IFormFieldProvider::FormField*>* HMapsFieldProvider::GetFields(
		void) {
	return __pFieldList;
}

result HMapsFieldProvider::SaveFields(void) {
	return E_INVALID_OPERATION;
}

int HMapsFieldProvider::GetFieldCount(void) {
	return 2;
}

int HMapsFieldProvider::GetProviderID(void) {
	return ID_FIELD_PROVIDER_HMAPS;
}
