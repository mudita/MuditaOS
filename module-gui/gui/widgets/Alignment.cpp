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

} /* namespace gui */
