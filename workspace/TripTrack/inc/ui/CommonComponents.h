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

	virtual FormField* GetFieldAt(int &id)=0;
	virtual Tizen::Base::Collection::HashMapT<int, FormField*>* GetFields(
			void)=0;
	virtual result SetFieldAt(int &id, FormField* formField)=0;
	virtual int GetFieldCount(void)=0;
};

class EditFormPopup: public Tizen::Ui::Controls::Popup {
public:
	EditFormPopup(void);
	~EditFormPopup(void);
	result Construct(IFormFieldProvider* fieldProvider, Tizen::Graphics::Dimension dimension);
	result Save(void);

	const static int ID_BUTTON_POPUP_SAVE = 1001;
	const static int ID_BUTTON_POPUP_CANCEL = 1002;

private:
	Tizen::Base::Collection::HashMapT<int, IFormFieldProvider::FormField*>* __pFieldMap;
	Tizen::Base::Collection::HashMapT<int, Tizen::Ui::Controls::ExpandableEditArea*>* __pExTxtAreasMap;
	Tizen::Ui::Controls::Button __pSaveButton, __pCancelButton;
	IFormFieldProvider* __pFieldProvider;
};

#endif /* COMMONCOMPONENTS_H_ */
