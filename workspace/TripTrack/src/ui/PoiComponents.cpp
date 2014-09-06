/*
 * PoiComponents.cpp
 *
 *  Created on: Aug 14, 2013
 *      Author: bohemius
 */
#include <FApp.h>
#include <FMedia.h>
#include <FSystem.h>
#include "ui/PoiComponents.h"
#include "dao/StorageManager.h"
#include "util/GraphicsUtils.h"
#include "SceneRegister.h"

using namespace Tizen::Ui::Controls;
using namespace Tizen::Base;
using namespace Tizen::Base::Collection;
using namespace Tizen::Graphics;
using namespace Tizen::App;
using namespace Tizen::Media;
using namespace Tizen::Ui::Scenes;
using namespace Tizen::Ui;

PoiIconListPanel::PoiIconListPanel(Rectangle &rect) :
		__pPoiGroupedListView(null), __pContainerRectangle(null), __pBitmapRectangle(
				null), __pPoiMap(null) {
	Panel::Construct(Rectangle(0, 0, rect.width, rect.height));
}

PoiIconListPanel::~PoiIconListPanel(void) {
	if (__pPoiMap != null) {
		__pPoiMap->RemoveAll();
		delete __pPoiMap;
	}
	/*if (__pPoiGroupedListView != null) {
		__pPoiGroupedListView->SetShowState(false);
		__pPoiGroupedListView->Invalidate(true);
		delete __pPoiGroupedListView;
	}*/
	delete __pPoiGroupedListView;
}

result PoiIconListPanel::Construct() {
	result r = E_SUCCESS;

	//load items into the hashmap
	r = LoadResources();
	if (r != E_SUCCESS) {
		AppLogException(
				"Error loading resources for poi scroll panel: [%s]", GetErrorMessage(r));
		return r;
	}
	//define dimensions
	__pContainerRectangle = new Rectangle(0, 0, GetBounds().width,
			GetBounds().height);
	__pBitmapRectangle = GraphicsUtils::GetTTMediaDimensions();

	__pPoiGroupedListView = new GroupedListView();
	r = __pPoiGroupedListView->Construct(*__pContainerRectangle,
			GROUPED_LIST_VIEW_STYLE_INDEXED, false, false);
	if (r != E_SUCCESS) {
		AppLogException(
				"Error constructing poi grouped list view: [%s]", GetErrorMessage(r));
		return r;
	}
	r = __pPoiGroupedListView->SetItemProvider(*this);
	if (r != E_SUCCESS) {
		AppLogException(
				"Error setting poi icon list item provider: [%s]", GetErrorMessage(r));
		return r;
	}
	__pPoiGroupedListView->AddGroupedListViewItemEventListener(*this);

	AddControl(*__pPoiGroupedListView);

	return r;
}

Tizen::Base::String* PoiIconListPanel::BuildTileText(POI* pPoi) {
	String* retVal = new String(L"");

	retVal->Append(pPoi->GetTimestamp()->ToString());
	retVal->Append(L"\n\n");
	retVal->Append(*(pPoi->GetTitle()));
	retVal->Append(L"\n");
	retVal->Append(*(pPoi->GetDescription()));

	return retVal;
}

void PoiIconListPanel::OnGroupedListViewContextItemStateChanged(
		Tizen::Ui::Controls::GroupedListView& listView, int groupIndex,
		int itemIndex, int elementId,
		Tizen::Ui::Controls::ListContextItemStatus state) {
}

void PoiIconListPanel::OnGroupedListViewItemStateChanged(
		Tizen::Ui::Controls::GroupedListView& listView, int groupIndex,
		int itemIndex, int elementId,
		Tizen::Ui::Controls::ListItemStatus state) {
	result r = E_SUCCESS;

	POI* selectedPoi = null;
	LinkedList* parameterList = new LinkedList();

	IListT<long long int>* pTimeCollection = __pPoiMap->GetKeysN();
	long long int timeTicks;
	r = pTimeCollection->GetAt(groupIndex, timeTicks);
	if (r != E_SUCCESS)
		AppLogException(
				"Error getting time key from poi map", GetErrorMessage(r));

	LinkedListT<POI*>* pPoiCollection = null;
	r = __pPoiMap->GetValue(timeTicks, pPoiCollection);
	if (r != E_SUCCESS)
		AppLogException(
				"Error getting poi collection for time key [%ld] from poi map", timeTicks, GetErrorMessage(r));

	r = pPoiCollection->GetAt(itemIndex, selectedPoi);
	if (r != E_SUCCESS) {
		AppLogException(
				"Error getting poi with item index [%d] and group index [%d] from poi collection:", itemIndex, groupIndex, GetErrorMessage(r));
	} else {
		parameterList->Add(selectedPoi);
		SceneManager* pSceneMngr = SceneManager::GetInstance();
		pSceneMngr->GoForward(ForwardSceneTransition(SCENE_POI_FORM),
				parameterList);
	}
}

void PoiIconListPanel::OnGroupedListViewItemSwept(
		Tizen::Ui::Controls::GroupedListView& listView, int groupIndex,
		int itemIndex, Tizen::Ui::Controls::SweepDirection direction) {
}

int PoiIconListPanel::GetGroupCount(void) {
	return __pPoiMap->GetCount();
}

int PoiIconListPanel::GetItemCount(int groupIndex) {
	result r = E_SUCCESS;
	long long int timeTicks;

	IListT<long long int>* pTimeCollection = __pPoiMap->GetKeysN();
	r = pTimeCollection->GetAt(groupIndex, timeTicks);
	if (r != E_SUCCESS)
		AppLogException("Error getting time key:", GetErrorMessage(r));

	LinkedListT<POI*>* pPoiCollection = null;
	r = __pPoiMap->GetValue(timeTicks, pPoiCollection);
	if (r != E_SUCCESS)
		AppLogException(
				"Error poi collection using time key [%ld]:", timeTicks, GetErrorMessage(r));
	return pPoiCollection->GetCount();
}

Tizen::Ui::Controls::ListItemBase* PoiIconListPanel::CreateItem(int groupIndex,
		int itemIndex, int itemWidth) {

	result r = E_SUCCESS;

	float width = ((float) itemWidth);
	FloatDimension elementDimension = FloatDimension(width,
			width * __pBitmapRectangle->height / __pBitmapRectangle->width
					* 0.85);
	FloatRectangle elementRectangle = FloatRectangle(0.0, 0.0,
			elementDimension.width, elementDimension.height);

	long long int groupTimeTicks;
	r = __pPoiMap->GetKeysN()->GetAt(groupIndex, groupTimeTicks);
	if (r != E_SUCCESS)
		AppLogException("Ërror getting date key from map:", GetErrorMessage(r));

	LinkedListT<POI*>* pPoiCollection = null;
	r = __pPoiMap->GetValue(groupTimeTicks, pPoiCollection);
	if (r != E_SUCCESS)
		AppLogException(
				"Ërror poi collection for date key [%ld] from map:", groupTimeTicks, GetErrorMessage(r));

	POI* pPoi = null;
	r = pPoiCollection->GetAt(itemIndex, pPoi);
	if (r != E_SUCCESS)
		AppLogException(
				"Ërror getting poi from poi collection:", GetErrorMessage(r));

	CustomItem* retVal = new CustomItem();
	r = retVal->Construct(elementDimension, LIST_ANNEX_STYLE_NORMAL);
	if (r != E_SUCCESS)
		AppLogException("Ërror constructing custom item:", GetErrorMessage(r));

	PoiListElement* poiElement = new PoiListElement(pPoi);
	r = retVal->AddElement(elementRectangle, ID_FORMAT_CUSTOM, *poiElement);
	if (r != E_SUCCESS)
		AppLogException(
				"Ërror adding element to custom item:", GetErrorMessage(r));
	return retVal;
}

Tizen::Ui::Controls::GroupItem* PoiIconListPanel::CreateGroupItem(
		int groupIndex, int itemWidth) {

	GroupItem* retVal = new GroupItem();
	result r = E_SUCCESS;

	IListT<long long int>* pTimeCollection = __pPoiMap->GetKeysN();

	long long int groupTimeTicks;
	DateTime currentTime;

	r = Tizen::System::SystemTime::GetCurrentTime(currentTime);
	if (r != E_SUCCESS)
		AppLogException("Error getting system time:", GetErrorMessage(r));

	r = pTimeCollection->GetAt(groupIndex, groupTimeTicks);
	if (r != E_SUCCESS)
		AppLogException("Error getting group time:", GetErrorMessage(r));

	DateTime groupTime;
	r = groupTime.SetValue(groupTimeTicks);
	if (r != E_SUCCESS)
		AppLogException(
				"Error seting time value using tick [%ld]", groupTimeTicks, GetErrorMessage(r));

	String timeText(L"");
	timeText.Append(groupTime.GetMonth());
	timeText.Append(L"/");
	timeText.Append(groupTime.GetDay());
	timeText.Append(L"/");
	timeText.Append(groupTime.GetYear());

	r = retVal->Construct(Dimension(0, 100));
	if (r != E_SUCCESS)
		AppLogException("Error constructing group item:", GetErrorMessage(r));
	r = retVal->SetElement(timeText, null);
	if (r != E_SUCCESS)
		AppLogException(
				"Error setting group item element:", GetErrorMessage(r));

	retVal->SetTextColor(Color::GetColor(COLOR_ID_GREY));
	retVal->SetTextSize(50);

	return retVal;
}

bool PoiIconListPanel::DeleteItem(int groupIndex, int itemIndex,
		Tizen::Ui::Controls::ListItemBase* pItem, int itemWidth) {
	//not used
	return false;
}

bool PoiIconListPanel::DeleteGroupItem(int groupIndex,
		Tizen::Ui::Controls::GroupItem* pItem, int itemWidth) {
	//not used
	return false;
}

result PoiIconListPanel::Update(void) {
	result r = E_SUCCESS;

	if (__pPoiMap != null) {
		__pPoiMap->RemoveAll();
		delete __pPoiMap;
		__pPoiMap = StorageManager::getInstance()->GetPoiHash();
		r = __pPoiGroupedListView->UpdateList();
		if (r != E_SUCCESS)
			AppLogException(
					"Error updating POI panel: [%s]", GetErrorMessage(r));
		return r;
	}
	return r;
}

result PoiIconListPanel::LoadResources(void) {
	result r = E_SUCCESS;

	__pPoiMap = StorageManager::getInstance()->GetPoiHash();
	AppLog("Loaded [%d] poi groups", __pPoiMap->GetCount());

	return r;
}

PoiListElement::PoiListElement(POI* pPoi) :
		__pPoi(pPoi) {
}

PoiListElement::~PoiListElement(void) {
}

bool PoiListElement::OnDraw(Tizen::Graphics::Canvas& canvas,
		const Tizen::Graphics::FloatRectangle& rect,
		Tizen::Ui::Controls::ListItemDrawingStatus itemStatus) {
	result r = E_SUCCESS;

	Bitmap* pBitmap;

	if (__pPoi->GetDefImageId() > 0) {
		TTMedia* pMedia = new TTMedia();
		r = pMedia->Construct(__pPoi->GetDefImageId());
		if (r != E_SUCCESS) {
			AppLogException(
					"Error constructing default media for poi [%ld]:", __pPoi->GetId(), GetErrorMessage(r));
			return false;
		}
		pBitmap = GraphicsUtils::CreateBitmap(*(pMedia->GetSourceUri()));
	} else {
		AppResource* pAppRes = Application::GetInstance()->GetAppResource();
		pBitmap = pAppRes->GetBitmapN(L"BlankPoi.png");
	}

	FloatRectangle presentationRect = FloatRectangle(rect.x + BITMAP_PADDING_X,
			rect.y + BITMAP_PADDING_Y, rect.width - 2.0 * BITMAP_PADDING_X,
			rect.height - 2.0 * BITMAP_PADDING_Y);

	r = canvas.DrawBitmap(presentationRect, *pBitmap);
	if (r != E_SUCCESS) {
		AppLogException("Error drawing bitmap:", GetErrorMessage(r));
		return false;
	}
	return true;
}

