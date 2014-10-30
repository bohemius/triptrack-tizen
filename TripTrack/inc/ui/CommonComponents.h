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

class IOnDataChangedListener {
public:
	virtual result Update(void)=0;
};

class IFormFieldProvider {
public:
	struct FormField {
		Tizen::Base::String fieldName;
		Tizen::Base::String fieldData;
		Tizen::Graphics::Dimension fieldDim;
		int limit;
		int id;
	};

	virtual Tizen::Base::Collection::LinkedListT<FormField*>* GetFields(void)=0;
	virtual result SaveFields(
			Tizen::Base::Collection::LinkedListT<FormField*>* fieldList)=0;
	virtual int GetFieldCount(void)=0;
	virtual int GetProviderID(void)=0;

	const static int ID_FIELD_PROVIDER_TRACK = 2001;
	const static int ID_FIELD_PROVIDER_POI = 2002;
};

class EditFormPopup: public Tizen::Ui::Controls::Popup,
		public Tizen::Ui::IPropagatedKeyEventListener,
		public Tizen::Ui::IActionEventListener {
public:
	EditFormPopup(void);
	~EditFormPopup(void);
	result Construct(IFormFieldProvider* fieldProvider,
			IOnDataChangedListener* resultListener,
			Tizen::Graphics::Dimension dimension, Tizen::Base::String title);

	//IPropagatedKeyEventListener
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

	//IActionEventListener
	virtual void OnActionPerformed(const Tizen::Ui::Control &source,
			int actionId);

	const static int ID_BUTTON_POPUP_SAVE = 1001;
	const static int ID_BUTTON_POPUP_CANCEL = 1002;

private:
	Tizen::Base::Collection::LinkedListT<IFormFieldProvider::FormField*>* __pFieldList;
	Tizen::Base::Collection::LinkedListT<Tizen::Ui::Controls::EditArea*>* __pExTxtAreasList;
	Tizen::Ui::Controls::Button* __pSaveButton;
	Tizen::Ui::Controls::Button* __pCancelButton;
	Tizen::Ui::Controls::Panel* __pButtonPanel;
	IFormFieldProvider* __pFieldProvider;
	IOnDataChangedListener* __pOnDataChangedListener;
};

#endif /* COMMONCOMPONENTS_H_ */
