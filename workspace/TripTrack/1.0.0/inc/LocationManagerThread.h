#ifndef _LOCATION_MANAGER_THREAD_H_
#define _LOCATION_MANAGER_THREAD_H_

class LocationManagerThread
	:public Tizen::Base::Runtime::Thread
{
public:
	LocationManagerThread(void);
	virtual ~LocationManagerThread(void);

	result Construct(const Tizen::Ui::Control& uiControl);
	Tizen::Base::Object* Run(void);

private:
	const Tizen::Ui::Control* __pUiControl;
};

#endif /* _LOCATION_MANAGER_THREAD_H_ */
