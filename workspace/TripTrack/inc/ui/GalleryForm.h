/*
 * GalleryForm.h
 *
 *  Created on: Sep 7, 2014
 *      Author: bohemius
 */

#ifndef GALLERYFORM_H_
#define GALLERYFORM_H_

#include <FUiCtrlIGalleryEventListener.h>
#include <FUi.h>
#include "dao/POI.h"

class GalleryForm: public Tizen::Ui::Controls::IGalleryEventListener,
		public Tizen::Ui::Controls::IGalleryItemProvider,
		public Tizen::Ui::Controls::IFormBackEventListener,
		public Tizen::Ui::Scenes::ISceneEventListener,
		public Tizen::Ui::Controls::Form {

public:
	GalleryForm(void) :
			__pGallery(null), __pPoi(null) {
	}
	;
	virtual ~GalleryForm();

	bool Initialize(void);
	virtual result OnInitializing(void);

	// IFormBackEventListener
	virtual void OnFormBackRequested(Tizen::Ui::Controls::Form& source);

	// ISceneEventListener
	virtual void OnSceneActivatedN(
			const Tizen::Ui::Scenes::SceneId& previousSceneId,
			const Tizen::Ui::Scenes::SceneId& currentSceneId,
			Tizen::Base::Collection::IList* pArgs);
	virtual void OnSceneDeactivated(
			const Tizen::Ui::Scenes::SceneId& currentSceneId,
			const Tizen::Ui::Scenes::SceneId& nextSceneId);

	// IGalleryEventListener
	virtual void OnGalleryCurrentItemChanged(
			Tizen::Ui::Controls::Gallery &gallery, int index);
	virtual void OnGalleryItemClicked(Tizen::Ui::Controls::Gallery &gallery,
			int index);
	virtual void OnGallerySlideShowStarted(
			Tizen::Ui::Controls::Gallery& gallery);
	virtual void OnGallerySlideShowStopped(
			Tizen::Ui::Controls::Gallery& gallery);

	//IGalleryItemProvider
	virtual Tizen::Ui::Controls::GalleryItem* CreateItem(int index);
	virtual bool DeleteItem(int index, Tizen::Ui::Controls::GalleryItem *pItem);
	virtual int GetItemCount(void);

private:

	Tizen::Ui::Controls::Gallery* __pGallery;
	POI* __pPoi;
	Tizen::Base::Integer __selectedIndex;

};

#endif /* GALLERYFORM_H_ */
