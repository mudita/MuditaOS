/*
 * Alignment.cpp
 *
 *  Created on: 7 mar 2019
 *      Author: robert
 */

#include "Alignment.hpp"

namespace gui {

Alignment::Alignment() :
	alignment{Alignment::ALIGN_HORIZONTAL_LEFT | ALIGN_VERTICAL_TOP } {
}

Alignment::Alignment(const uint32_t align) : alignment{align}
{
}

Alignment::Alignment(const uint32_t valH, const uint32_t valV) : Alignment(valH | valV)
{
}

Alignment::~Alignment() {
	// TODO Auto-generated destructor stub
}

bool Alignment::setHorizontal( const uint32_t val ) {
	short horizontal =
		Alignment::ALIGN_HORIZONTAL_CENTER |
		Alignment::ALIGN_HORIZONTAL_LEFT |
		Alignment::ALIGN_HORIZONTAL_RIGHT;

	//if flag is valid, reset any horizontal flag and add new
	if( val & horizontal ) {
		alignment &= ~( horizontal );
		alignment |= val;
		return true;
	}
	return false;
}

bool Alignment::setVertical( const uint32_t val ) {
	short vertical =
		Alignment::ALIGN_VERTICAL_CENTER |
		Alignment::ALIGN_VERTICAL_TOP |
		Alignment::ALIGN_VERTICAL_BOTTOM;

	//if flag is valid, reset any horizontal flag and add new
	if( val & vertical) {
		alignment &= ~( vertical);
		alignment |= val;
		return true;
	}
	return false;
}

bool Alignment::isAligned( const uint32_t val ) {
	if( alignment & val )
		return true;
	return false;
}

} /* namespace gui */
