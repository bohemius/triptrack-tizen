/*
 * MapForm.h
 *
 *  Created on: Aug 7, 2014
 *      Author: bohemius
 */

#ifndef MAPFORM_H_
#define MAPFORM_H_

#include <FUi.h>
#include <FBase.h>
#include <FApp.h>
#include <HMaps.h>

class MapForm: public Tizen::Ui::Controls::Form,
		public HMaps::IMapEventListener,
		public Tizen::Ui::Controls::IFormBackEventListener,
		public Tizen::Ui::Scenes::ISceneEventListener {
public:
	MapForm();
	virtual ~MapForm();

	// IMapEventListener
	virtual void OnMapLongPressed (HMaps::Map &map, const Tizen::Locations::Coordinates &coordinate);
	virtual void OnMapRegionChanged (HMaps::Map &map);
	virtual void OnMapTapped (HMaps::Map &map, const Tizen::Locations::Coordinates &coordinate);

	// override from Form
	bool Initialize(void);
	virtual result OnInitializing(void);
	virtual result OnTerminating(void);

	// IFormBackEventListener
	virtual void OnFormBackRequested(Tizen::Ui::Controls::Form &source);

	// ISceneEventListener
	virtual void OnSceneActivatedN(
			const Tizen::Ui::Scenes::SceneId &previousSceneId,
			const Tizen::Ui::Scenes::SceneId &currentSceneId,
			Tizen::Base::Collection::IList *pArgs);
	virtual void OnSceneDeactivated(
			const Tizen::Ui::Scenes::SceneId &currentSceneId,
			const Tizen::Ui::Scenes::SceneId &nextSceneId);

private:
	HMaps::Map* __pMap;
};

#endif /* MAPFORM_H_ */
