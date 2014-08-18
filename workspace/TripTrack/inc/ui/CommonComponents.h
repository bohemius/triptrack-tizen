/*
 * CommonComponents.h
 *
 *  Created on: Oct 3, 2013
 *      Author: bohemius
 */

#ifndef COMMONCOMPONENTS_H_
#define COMMONCOMPONENTS_H_

#include <FBase.h>
#include <FUi.h>

class IFormFieldProvider {
public:
	struct FormField {
		Tizen::Base::String* fieldName;
		Tizen::Base::String* fieldData;
		Tizen::Graphics::Dimension* fieldDim;
		int limit;
		int id;
	};

	virtual Tizen::Base::Collection::LinkedListT<FormField*>* GetFields(void)=0;
	virtual result SaveFields(void)=0;
	virtual int GetFieldCount(void)=0;
};

class EditFormPopup: public Tizen::Ui::Controls::Popup,
		public Tizen::Ui::IPropagatedKeyEventListener {
public:
	EditFormPopup(void);
	~EditFormPopup(void);
	result Construct(IFormFieldProvider* fieldProvider,
			Tizen::Graphics::Dimension dimension, Tizen::Base::String title);

	virtual bool OnKeyPressed(Tizen::Ui::Control& source,
			const Tizen::Ui::KeyEventInfo& keyEventInfo);
	virtual bool OnKeyReleased(Tizen::Ui::Control& source,
			const Tizen::Ui::KeyEventInfo& keyEventInfo);
	virtual bool OnPreviewKeyPressed(Tizen::Ui::Control& source,
			const Tizen::Ui::KeyEventInfo& keyEventInfo);
	virtual bool OnPreviewKeyReleased(Tizen::Ui::Control& source,
			const Tizen::Ui::KeyEventInfo& keyEventInfo);
	virtual bool TranslateKeyEventInfo(Tizen::Ui::Control& source,
			Tizen::Ui::KeyEventInfo& keyEventInfo);

	const static int ID_BUTTON_POPUP_SAVE = 1001;
	const static int ID_BUTTON_POPUP_CANCEL = 1002;

private:
	Tizen::Base::Collection::LinkedListT<IFormFieldProvider::FormField*>* __pFieldList;
	Tizen::Base::Collection::LinkedListT<
			Tizen::Ui::Controls::EditArea*>* __pExTxtAreasList;
	Tizen::Ui::Controls::Button __pSaveButton, __pCancelButton;
	IFormFieldProvider* __pFieldProvider;
};

#endif /* COMMONCOMPONENTS_H_ */
