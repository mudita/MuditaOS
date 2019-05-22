/*
 * PixMap.hpp
 *
 *  Created on: 18 maj 2019
 *      Author: robert
 */

#ifndef GUI_CORE_PIXMAP_HPP_
#define GUI_CORE_PIXMAP_HPP_

#include <string>
#include <cstring>
#include <cstdint>

#include "../Common.hpp"

namespace gui {

/**
 * @brief Class that holds pixels and dimensions of the image
 */
class PixMap {
protected:
	//id of the pixmap asigned by the pixmap manager
	uint32_t id;
	//number of columns in the pixmap
	uint16_t width;
	//number of rows in the image
	uint16_t height;
	//data of the image
	uint8_t* data;
	//file name
	std::string name;
public:
	PixMap();
	PixMap( uint16_t w, uint16_t h, uint8_t* data );
	~PixMap();
	gui::Status load( uint8_t* data );

	uint16_t getWidth() { return width; };
	uint16_t getHeight() { return height; };
	uint8_t* getData(){ return data; };
	std::string getName() { return name; };
	uint32_t getID() { return id; };

	void setID( uint32_t id ) { this->id = id; };
	void setName( std::string name ){ this->name = name; };
};

} /* namespace gui */

#endif /* GUI_CORE_PIXMAP_HPP_ */
