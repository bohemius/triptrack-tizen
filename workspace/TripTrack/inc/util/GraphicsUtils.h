/*
 * GraphicsUtils.h
 *
 *  Created on: Aug 24, 2013
 *      Author: bohemius
 */

#ifndef GRAPHICSUTILS_H_
#define GRAPHICSUTILS_H_

#include <FBase.h>
#include <FGraphics.h>

class GraphicsUtils {
public:
	GraphicsUtils();
	virtual ~GraphicsUtils();
	static Tizen::Graphics::Bitmap* CreateBitmap(Tizen::Base::String &file);
	static Tizen::Graphics::Bitmap* CreateBitmap(Tizen::Base::ByteBuffer* imgBuffer, float &width, float &height);
	static Tizen::Base::ByteBuffer* CreateImageBuffer(Tizen::Graphics::Bitmap* bitmap);
	static Tizen::Graphics::FloatRectangle* GetTTMediaDimensions(void);
};

#endif /* GRAPHICSUTILS_H_ */
