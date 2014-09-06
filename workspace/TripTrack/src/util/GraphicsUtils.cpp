/*
 * GraphicsUtils.cpp
 *
 *  Created on: Aug 24, 2013
 *      Author: bohemius
 */

#include <FMedia.h>
#include "util/GraphicsUtils.h"
#include "dao/StorageManager.h"
#include "dao/TTMedia.h"

using namespace Tizen::Media;
using namespace Tizen::Graphics;
using namespace Tizen::Base;
using namespace Tizen::Base::Collection;

GraphicsUtils::GraphicsUtils() {
	// TODO Auto-generated constructor stub

}

GraphicsUtils::~GraphicsUtils() {
	// TODO Auto-generated destructor stub
}

Tizen::Graphics::Bitmap* GraphicsUtils::CreateBitmap(
		Tizen::Base::String& file) {

	result r = E_SUCCESS;
	Image img;
	Bitmap* retVal = null;

	r = img.Construct();

	// Decodes the image
	retVal = img.DecodeN(file, BITMAP_PIXEL_FORMAT_RGB565);
	if (retVal == null || r != E_SUCCESS) {
		AppLogException(
				"Error creating bitmap from file [%s]:[%s]", file.GetPointer(), GetErrorMessage(r));
	}
	return retVal;
}

Tizen::Graphics::Bitmap* GraphicsUtils::CreateBitmap(
		Tizen::Base::ByteBuffer* imgBuffer, float &width, float &height) {

	result r = E_SUCCESS;
	Image* img = new Image();
	Bitmap* retVal = null;

	r = img->Construct();
	if (r != E_SUCCESS) {
		AppLogException(
				"Error constructing image object for decoding: [%s]", GetErrorMessage(r));
		return null;
	}

	//Encode the image
	retVal = img->DecodeN(*imgBuffer, BITMAP_PIXEL_FORMAT_RGB565, width,
			height);
	r = GetLastResult();
	if (retVal == null || r != E_SUCCESS) {
		AppLogException(
				"Error creating bitmap from image buffer: [%s]", GetErrorMessage(r));
	}
	return retVal;
}

Tizen::Base::ByteBuffer* GraphicsUtils::CreateImageBuffer(
		Tizen::Graphics::Bitmap* bitmap) {
	result r = E_SUCCESS;
	Image img;
	ByteBuffer* retVal = null;

	r = img.Construct();

	retVal = img.EncodeToBufferN(*bitmap, IMG_FORMAT_PNG);
	if (retVal == null || r != E_SUCCESS) {
		AppLogException(
				"Error creating byte buffer from  bitmap: [%s]", GetErrorMessage(r));
	}
	return retVal;
}

Tizen::Graphics::FloatRectangle* GraphicsUtils::GetTTMediaDimensions(void) {
	FloatRectangle* retVal = new FloatRectangle(0.0, 0.0, 640.0, 480.0);
	result r = E_SUCCESS;

	LinkedListT<POI*>* pPoiCollection =
			StorageManager::getInstance()->GetPois();

	if (pPoiCollection->GetCount() > 0) {
		POI* pPoi = null;
		r = pPoiCollection->GetAt(0, pPoi);
		if (r != E_SUCCESS && pPoi != null) {
			AppLogException(
					"Error getting first poi from collection", GetErrorMessage(r));
			return retVal;
		}
		TTMedia* pMedia = new TTMedia;
		r = pMedia->Construct(pPoi->GetDefImageId());
		if (r != E_SUCCESS) {
			AppLogException(
					"Error getting default media for poi", GetErrorMessage(r));
			return retVal;
		}
		Bitmap* pMediaBitmap = GraphicsUtils::CreateBitmap(
				*(pMedia->GetSourceUri()));
		delete retVal;
		retVal = new FloatRectangle(0.0, 0.0, pMediaBitmap->GetWidthF(),
				pMediaBitmap->GetHeightF());

		delete pMediaBitmap;
		delete pMedia;
	} else {
		AppLog("No pois in collection using default media size");
	}
	return retVal;
}

Tizen::Graphics::Bitmap* GraphicsUtils::CreatePoiIconListTile(POI* pPoi) {
}


