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
				null), __pPoiMap(null), __pPoiListContextMenu(null), __pDeleteBitmap(
				null) {
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

	//construct context menu
	__pPoiListContextMenu = new (std::nothrow) ContextMenu();

	r = __pPoiListContextMenu->Construct(FloatPoint(200.0, 600.0),
			CONTEXT_MENU_STYLE_LIST);
	if (r != E_SUCCESS) {
		AppLogException(
				"Error constructing track list context menu: [%s]", GetErrorMessage(r));
	}
	__pPoiListContextMenu->SetFocusable(true);
	__pPoiListContextMenu->AddItem(L"Delete", ID_CONTEXT_ITEM_DELETE,
			*__pDeleteBitmap, __pDeleteBitmap, __pDeleteBitmap);
	__pPoiListContextMenu->SetColor(Color(46, 151, 199));
	__pPoiListContextMenu->AddActionEventListener(*this);

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
	__pPoiGroupedListView->AddTouchEventListener(*this);

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

	IListT<int>* pTimeCollection = __pPoiMap->GetKeysN();
	int grpKey;
	r = pTimeCollection->GetAt(groupIndex, grpKey);
	if (r != E_SUCCESS)
		AppLogException(
				"Error getting grp key from poi map", GetErrorMessage(r));

	LinkedListT<POI*>* pPoiCollection = null;
	r = __pPoiMap->GetValue(grpKey, pPoiCollection);
	if (r != E_SUCCESS)
		AppLogException(
				"Error getting poi collection for grp key [%d] from poi map", grpKey, GetErrorMessage(r));

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
	int grpKey;

	IListT<int>* pTimeCollection = __pPoiMap->GetKeysN();
	r = pTimeCollection->GetAt(groupIndex, grpKey);
	if (r != E_SUCCESS)
		AppLogException("Error getting time key:", GetErrorMessage(r));
	else
		AppLog("Got key %d", grpKey);

	LinkedListT<POI*>* pPoiCollection = null;
	r = __pPoiMap->GetValue(grpKey, pPoiCollection);
	if (r != E_SUCCESS)
		AppLogException(
				"Error poi collection using grp key [%d]:", grpKey, GetErrorMessage(r));
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

	int grpKey;
	r = __pPoiMap->GetKeysN()->GetAt(groupIndex, grpKey);
	if (r != E_SUCCESS)
		AppLogException("Ërror getting date key from map:", GetErrorMessage(r));

	LinkedListT<POI*>* pPoiCollection = null;
	r = __pPoiMap->GetValue(grpKey, pPoiCollection);
	if (r != E_SUCCESS)
		AppLogException(
				"Error poi collection for date key [%d] from map:", grpKey, GetErrorMessage(r));

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

	IListT<int>* pTimeCollection = __pPoiMap->GetKeysN();

	int grpKey;

	r = pTimeCollection->GetAt(groupIndex, grpKey);
	if (r != E_SUCCESS)
		AppLogException(
				"Error getting group key from keys:", GetErrorMessage(r));

	r = retVal->Construct(Dimension(0, 100));
	if (r != E_SUCCESS)
		AppLogException("Error constructing group item:", GetErrorMessage(r));
	r = retVal->SetElement(GraphicsUtils::GetTimeText(grpKey), null);
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

void PoiIconListPanel::OnActionPerformed(const Tizen::Ui::Control& source,
		int actionId) {
	switch (actionId) {
	case ID_CONTEXT_ITEM_DELETE: {
		POI* pPoi = GetPoiFromClick();
		DeletePoi(pPoi);
		Update();
	}
		break;
	}
}

void PoiIconListPanel::OnTouchPressed(const Tizen::Ui::Control& source,
		const Tizen::Graphics::Point& currentPosition,
		const Tizen::Ui::TouchEventInfo& touchInfo) {
	if (&source == __pPoiGroupedListView) {
		lastClickedPosition = source.ConvertToScreenPosition(currentPosition);
		int grpIdx, idx;
		__pPoiGroupedListView->GetItemIndexFromPosition(
				source.ConvertToControlPosition(lastClickedPosition), grpIdx,
				idx);
		AppLog(
				"Clicked on poi list view, position: [%d] [%d], on list view item with grp id: [%d] id: [%d]", currentPosition.x, currentPosition.y, grpIdx, idx);
	}
}

void PoiIconListPanel::DeletePoi(POI* pPoi) {
	result r = E_SUCCESS;

	r = pPoi->RemoveAllMedia();
	if (r != E_SUCCESS)
		AppLogException("Error removing media for POI: ", GetErrorMessage(r));

	AppLog(
			"Removing existing POI [%ls] with ID [%d] from database.", pPoi->GetTitle()->GetPointer(), pPoi->GetId());
	StorageManager::getInstance()->CRUDoperation(pPoi, I_CRUDable::DELETE);
	r = GetLastResult();
	if (r != E_SUCCESS) {
		AppLogException(
				"Error removing POI [%ls] with ID [%d] from database: [%s]", pPoi->GetTitle()->GetPointer(), pPoi->GetId(), GetErrorMessage(r));
		return;
	}
	AppLog(
			"Successfully removed POI [%ls] with ID [%d] from collection and database.", pPoi->GetTitle()->GetPointer(), pPoi->GetId());
}

POI* PoiIconListPanel::GetPoiFromClick(void) {
	POI* pPoi = null;

	result r = E_SUCCESS;

	int grpIdx, idx;
	r = __pPoiGroupedListView->GetItemIndexFromPosition(
			__pPoiGroupedListView->ConvertToControlPosition(
					lastClickedPosition), grpIdx, idx);
	AppLog("Id of selected list item: grp id [%d], id [%d]", grpIdx, idx);

	IListT<int>* groupList = __pPoiMap->GetKeysN();

	int grpKey;
	r = groupList->GetAt(grpIdx, grpKey);

	LinkedListT<POI*>* poiList = null;
	r = __pPoiMap->GetValue(grpKey, poiList);

	r = poiList->GetAt(idx, pPoi);

	return pPoi;
}

void PoiIconListPanel::OnGroupedListViewItemLongPressed(
		Tizen::Ui::Controls::GroupedListView& listView, int groupIndex,
		int itemIndex, int elementId, bool& invokeListViewItemCallback) {
	__pPoiListContextMenu->SetAnchorPosition(lastClickedPosition);
	__pPoiListContextMenu->SetShowState(true);
	__pPoiListContextMenu->Show();
}

result PoiIconListPanel::LoadResources(void) {
	result r = E_SUCCESS;

	AppResource* pAppRes = Application::GetInstance()->GetAppResource();
	__pDeleteBitmap = pAppRes->GetBitmapN(L"delete.png");

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
	canvas.SetLineWidth(5);
	canvas.SetForegroundColor(Color(46, 151, 199));
	r = canvas.DrawRoundRectangle(presentationRect, FloatDimension(2, 2));

	FloatDimension overlayDim = FloatDimension(
			presentationRect.width - 2 * BITMAP_PADDING_X,
			presentationRect.height - 2 * BITMAP_PADDING_Y);
	EnrichedText* pOverlayText = CreateTextOverLay(overlayDim);
	r = canvas.DrawText(
			Point(presentationRect.x + BITMAP_PADDING_X,
					presentationRect.y + BITMAP_PADDING_Y), *pOverlayText);
	pOverlayText->RemoveAll(true);
	delete pOverlayText;

	return true;
}

Tizen::Graphics::EnrichedText* PoiListElement::CreateTextOverLay(
		Tizen::Graphics::FloatDimension overlayDim) {
	result r = E_SUCCESS;
	EnrichedText* retVal = new EnrichedText();

	r = retVal->Construct(overlayDim);

	retVal->SetHorizontalAlignment(TEXT_ALIGNMENT_LEFT);
	retVal->SetVerticalAlignment(TEXT_ALIGNMENT_TOP);
	retVal->SetTextWrapStyle(TEXT_WRAP_WORD_WRAP);
	retVal->SetTextAbbreviationEnabled(true);

	Font boldFont, plainFont;
	boldFont.Construct(FONT_STYLE_BOLD, 40);
	plainFont.Construct(FONT_STYLE_PLAIN, 40);

	TextElement* pTimeElement = new TextElement();
	r = pTimeElement->Construct(__pPoi->GetTimestamp()->ToString() + "\n");

	pTimeElement->SetTextColor(Color::GetColor(COLOR_ID_WHITE));
	pTimeElement->SetFont(boldFont);

	TextElement* pTitleElement = new TextElement();
	r = pTitleElement->Construct(*(__pPoi->GetTitle()) + "\n\n");

	pTitleElement->SetTextColor(Color::GetColor(COLOR_ID_WHITE));
	pTitleElement->SetFont(boldFont);

	TextElement* pDescElement = new TextElement();
	r = pDescElement->Construct(*(__pPoi->GetDescription()));

	pDescElement->SetTextColor(Color::GetColor(COLOR_ID_WHITE));
	pDescElement->SetFont(plainFont);

	retVal->Add(*pTimeElement);
	retVal->Add(*pTitleElement);
	retVal->Add(*pDescElement);

	return retVal;
}

