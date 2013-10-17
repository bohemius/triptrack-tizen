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
	};

	virtual FormField* GetFieldAt(int id)=0;
	virtual Tizen::Base::Collection::HashMapT<int, FormField*>* GetFields(
			void)=0;
	virtual result SetFieldAt(int id, FormField* formField)=0;
	virtual int GetFieldCount(void)=0;
};

class PopupEventListener: public Tizen::Ui::IPropagatedKeyEventListener {
	// key events for back-key
	virtual bool OnKeyPressed(Tizen::Ui::Control& source,
			const Tizen::Ui::KeyEventInfo& keyEventInfo) {
		return false;
	}
	;
	virtual bool OnKeyReleased(Tizen::Ui::Control& source,
			const Tizen::Ui::KeyEventInfo& keyEventInfo);
	virtual bool OnPreviewKeyPressed(Tizen::Ui::Control& source,
			const Tizen::Ui::KeyEventInfo& keyEventInfo) {
		return false;
	}
	;
	virtual bool OnPreviewKeyReleased(Tizen::Ui::Control& source,
			const Tizen::Ui::KeyEventInfo& keyEventInfo) {
		return false;
	}
	;
	virtual bool TranslateKeyEventInfo(Tizen::Ui::Control& source,
			Tizen::Ui::KeyEventInfo& keyEventInfo) {
		return false;
	}
	;
};

class SampleFieldProvider: public IFormFieldProvider {
public:
	SampleFieldProvider(void);
	virtual ~SampleFieldProvider(void);
	result Construct(void);
	virtual FormField* GetFieldAt(int id);
	virtual Tizen::Base::Collection::HashMapT<int, FormField*>* GetFields(void);
	virtual result SetFieldAt(int id, FormField* formField);
	virtual int GetFieldCount(void);

	const static int FORM_FIELD_ID_TITLE;
	const static int FORM_FIELD_ID_DESCRIPTION;
private:
	Tizen::Base::Collection::HashMapT<int, FormField*>* __pFormFields;

};

class EditFormPopup: public Tizen::Ui::Controls::Popup,
		public Tizen::Ui::Controls::IFormBackEventListener {
public:
	EditFormPopup(void);
	~EditFormPopup(void);
	result Construct(IFormFieldProvider* fieldProvider,
			Tizen::Graphics::Dimension dimension, Tizen::Base::String title);
	result Save(void);
	virtual void OnFormBackRequested(Tizen::Ui::Controls::Form& source);

	const static int ID_BUTTON_POPUP_SAVE = 1001;
	const static int ID_BUTTON_POPUP_CANCEL = 1002;

private:
	Tizen::Base::Collection::HashMapT<int, IFormFieldProvider::FormField*>* __pFieldMap;
	Tizen::Base::Collection::HashMapT<int,
			Tizen::Ui::Controls::ExpandableEditArea*>* __pExTxtAreasMap;
	Tizen::Ui::Controls::Button __pSaveButton, __pCancelButton;
	IFormFieldProvider* __pFieldProvider;
	PopupEventListener* __pPopupEventListener;
};

#endif /* COMMONCOMPONENTS_H_ */
