/*
 * GraphicsUtils.cpp
 *
 *  Created on: Aug 24, 2013
 *      Author: bohemius
 */

#include <FMedia.h>
#include "util/GraphicsUtils.h"

using namespace Tizen::Media;
using namespace Tizen::Graphics;
using namespace Tizen::Base;

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
	Image* img=new Image();
	Bitmap* retVal = null;

	r = img->Construct();
	if (r != E_SUCCESS) {
		AppLogException("Error constructing image object for decoding: [%s]", GetErrorMessage(r));
		return null;
	}

	//Encode the image
	retVal = img->DecodeN(*imgBuffer, BITMAP_PIXEL_FORMAT_RGB565, width, height);
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

