/*
 * Margins.cpp
 *
 *  Created on: 7 mar 2019
 *      Author: robert
 */

#include "Margins.hpp"

namespace gui {

Margins::Margins() : left{0}, top{0}, right{0}, bottom{0} {
}

Margins::Margins( const short left, const short top, const short right, const short bottom ) :
	left{left}, top{top}, right{right}, bottom{bottom} {
}

Margins::~Margins() {
}

short Margins::getHorizontal() const
{
    return (left + right);
}

short Margins::getVertical() const
{
    return (top + bottom);
}

} /* namespace gui */
