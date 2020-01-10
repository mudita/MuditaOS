/*
 * Alignment.hpp
 *
 *  Created on: 7 mar 2019
 *      Author: robert
 */

#ifndef MIDDLEWARES_GUI_WIDGETS_ALIGNMENT_HPP_
#define MIDDLEWARES_GUI_WIDGETS_ALIGNMENT_HPP_

#include <cstdint>

namespace gui {

class Alignment {
public:
	static const uint32_t ALIGN_VERTICAL_CENTER   = 0x01;
	static const uint32_t ALIGN_VERTICAL_TOP      = 0x02;
	static const uint32_t ALIGN_VERTICAL_BOTTOM   = 0x04;
	static const uint32_t ALIGN_HORIZONTAL_CENTER = 0x08;
	static const uint32_t ALIGN_HORIZONTAL_LEFT   = 0x10;
	static const uint32_t ALIGN_HORIZONTAL_RIGHT  = 0x20;

	uint32_t alignment;

	Alignment();
	Alignment( const uint32_t valH, const uint32_t valV );
    Alignment(const uint32_t align);
    virtual ~Alignment();

	bool setHorizontal( const uint32_t val );
	bool setVertical( const uint32_t val );
	bool isAligned( const uint32_t val );
};

} /* namespace gui */

#endif /* MIDDLEWARES_GUI_WIDGETS_ALIGNMENT_HPP_ */

