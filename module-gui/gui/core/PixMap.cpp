/*
 * PixMap.cpp
 *
 *  Created on: 18 maj 2019
 *      Author: robert
 */

#include "PixMap.hpp"

namespace gui {

PixMap::PixMap() : id{0},  width{ 0 }, height{0}, data{nullptr}, name{""} {}

PixMap::PixMap( uint16_t w, uint16_t h, uint8_t* data ) : id{0}, width{ w }, height{ h }, name{""} {

	this->data = new uint8_t[width*height];

	//no data provided - allocat buffer and clear it with white color
	if( data == nullptr ) {
		if( this->data ) memset( this->data, 0x0F, width*height );
	}
	else {
		if( this->data ) memcpy( this->data, data, width*height );
	}
}

PixMap::~PixMap() {
	if( data )
		delete []data;
}

gui::Status PixMap::load( uint8_t* data ) {

	uint32_t offset = 0;

	//read width and height of the image
	memcpy( &width, data + offset, sizeof(uint16_t)); offset += sizeof(uint16_t);
	memcpy( &height, data + offset, sizeof(uint16_t)); offset += sizeof(uint16_t);



	this->data = new uint8_t[width*height];
	if( this->data ) memcpy( this->data, data + offset, width*height );

	return gui::Status::GUI_SUCCESS;
}
} /* namespace gui */
