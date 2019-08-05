/*
 * @file Text.cpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 1 sie 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#include "Text.hpp"

namespace gui {

Text::Text() : Rect() {
}

Text::Text( Item* parent, const uint32_t& x, const uint32_t& y, const uint32_t& w, const uint32_t& h, const UTF8& text , bool expandVertically ) :
	Rect( parent, x, y, w, h ){

}

Text::~Text() {
	// TODO Auto-generated destructor stub
}

} /* namespace gui */
