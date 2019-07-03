/*
 * Common.cpp
 *
 *  Created on: 8 maj 2019
 *      Author: robert
 */
#include <cstdint>
#include <type_traits>
#include "Common.hpp"

namespace gui {

static timeSecondsFunctionPtr timeSecondsFunction = nullptr;

bool operator&( const gui::RectangleEdgeFlags& lhs, const gui::RectangleEdgeFlags& rhs ) {
	return static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs);
}

RectangleEdgeFlags operator|( const RectangleEdgeFlags& lhs, const RectangleEdgeFlags& rhs ) {
	using T = std::underlying_type_t <RectangleEdgeFlags>;
	return static_cast<RectangleEdgeFlags>(static_cast<T>(lhs) | static_cast<T>(rhs));
}

RectangleEdgeFlags operator|=( RectangleEdgeFlags& lhs, const RectangleEdgeFlags& rhs ) {
	lhs = lhs | rhs;
	return lhs;
}

uint32_t getTime() {
	if( timeSecondsFunction != nullptr )
		return timeSecondsFunction();
	return 0;
}

void setTimeFunction( timeSecondsFunctionPtr fptr ) {
	timeSecondsFunction = fptr;
}

} //namespace gui


