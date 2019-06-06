/*
 * Context.cpp
 *
 *  Created on: 6 maj 2019
 *      Author: robert
 */

#include <ios>
#include <cstring>

#include "BoundingBox.hpp"
#include "Context.hpp"

namespace gui {

Context::Context( ) : x{0}, y{0}, w{0}, h{0}, data{nullptr} {
}

Context::Context( uint16_t width, uint16_t height, uint8_t* buffer ) : x{0}, y{0}, w{width}, h{height} {
	if( buffer == nullptr ){
		data = new uint8_t[w*h];
	}
	else {
		data = buffer;
	}
	memset( data, 15, w*h);
}

Context::~Context() {
	if( data != nullptr )
		delete [] data;
}

Context* Context::get( int16_t gx, int16_t gy, uint16_t width, uint16_t height ) {
	Context* retContext = new Context( width, height );

	retContext->x = x;
	retContext->y = y;

	//create bounding boxes for the current context and return context
	BoundingBox currentBox = BoundingBox( 0, 0, w, h );
	BoundingBox newBox = BoundingBox( gx, gy, width, height );
	BoundingBox resultBox;

	//if boxes overlap copy part defined by result from current context to the new context.
	if( BoundingBox::intersect( currentBox, newBox, resultBox ) ) {
		uint32_t sourceOffset = resultBox.y*w + resultBox.x;
		uint32_t destOffset = (resultBox.y - gy)*width + (resultBox.x-gx);
		for( int32_t h=0; h<resultBox.h; h++) {
			memcpy( retContext->data + destOffset, data + sourceOffset, resultBox.w );
			sourceOffset+=w;
			destOffset +=width;
		}
	}
	//else just return context filled with white colour.

	return retContext;
}

void Context::insert( int16_t ix, int16_t iy, Context* context ) {
	//create bounding boxes for the current context and return context
	BoundingBox currentBox = BoundingBox( 0, 0, w, h );
	BoundingBox insertBox = BoundingBox( ix, iy, context->w, context->h );
	BoundingBox resultBox;

	//if boxes overlap copy part defined by result from current context to the new context.
	if( BoundingBox::intersect( currentBox, insertBox, resultBox ) ) {
		uint32_t sourceOffset = (resultBox.y - iy)*context->w + (resultBox.x-ix);
		uint32_t destOffset = (resultBox.y)*w + (resultBox.x);
		for( int32_t h=0; h<resultBox.h; h++) {
			memcpy( data + destOffset, context->data + sourceOffset, resultBox.w );
			sourceOffset += context->w;
			destOffset += w;
		}
	}
}

void Context::fill( uint8_t colour ) {
	if( data ) {
		memset( data, colour, w*h );
	}
}

std::ostream & operator << (std::ostream &out, const Context &c)
{
	out << "x:" << c.x << "y:" << c.y << "w:" << c.w <<"h:" << c.h <<std::endl;

	uint32_t offset = 0;
	for( uint32_t y = 0; y< c.h; y++ ) {
		for( uint32_t x=0; x<c.w;x++) {
			uint32_t value = *(c.data + offset);
			std::cout<<std::hex<<value;
			offset++;
		}
		std::cout<<std::endl;

	}
    return out;
}

} /* namespace gui */
