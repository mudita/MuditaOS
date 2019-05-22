/*
 * Common.hpp
 *
 *  Created on: 15 mar 2019
 *      Author: robert
 */

#ifndef MIDDLEWARES_GUI_WIDGETS_COMMON_HPP_
#define MIDDLEWARES_GUI_WIDGETS_COMMON_HPP_

#include <cstdint>

namespace gui {

enum class NavigationDirection {
	LEFT = 0x01,
	UP,
	RIGHT,
	DOWN
};

enum class Status
{
	GUI_SUCCESS = 0x01,
	GUI_FAILURE,
};

enum class RefreshModes {
	GUI_REFRESH_FAST = 1,
	GUI_REFRESH_DEEP
};

enum class ShowMode {
	GUI_SHOW_INIT = 0x01,
	GUI_SHOW_RETURN
};

enum class AlignementFlags {
	GUI_ALIGN_VERTICAL_CENTER   = 0x01,
	GUI_ALIGN_VERTICAL_TOP      = 0x02,
	GUI_ALIGN_VERTICAL_BOTTOM   = 0x04,
	GUI_ALIGN_HORIZONTAL_CENTER = 0x08,
	GUI_ALIGN_HORIZONTAL_LEFT   = 0x10,
	GUI_ALIGN_HORIZONTAL_RIGHT  = 0x20
};

enum class OrientationFlags {
	GUI_ORIENTATION_HORIZONTAL  = 0x00,
	GUI_ORIENTATION_VERTICAL    = 0x01
};

template <class T> bool operator&( const T& lhs, const T& rhs ) {
	return static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs);
}
template <class T> T operator|( const T& lhs, const T& rhs ) {
	return static_cast<T>(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
}
template <class T> T operator|=( const T& lhs, const T& rhs ) {
	lhs = lhs | rhs;
	return lhs;
}

enum class RectangleEdgeFlags {
	GUI_RECT_EDGE_NO_EDGES		 = 0x00,
	GUI_RECT_EDGE_TOP            = 0x01,
	GUI_RECT_EDGE_BOTTOM         = 0x02,
	GUI_RECT_EDGE_LEFT           = 0x04,
	GUI_RECT_EDGE_RIGHT          = 0x08,
	GUI_RECT_ALL_EDGES           = 0x0F
};

//bool operator&( const RectangleEdgeFlags& lhs, const RectangleEdgeFlags& rhs );
//RectangleEdgeFlags operator|( const RectangleEdgeFlags& lhs, const RectangleEdgeFlags& rhs );
//RectangleEdgeFlags operator|=( const RectangleEdgeFlags& lhs, const RectangleEdgeFlags& rhs );

enum class RectangleCornerFlags {
	GUI_RECT_CORNER_NO_CORNERS   = 0x00,
	GUI_RECT_CORNER_TOP_LEFT     = 0x10,
	GUI_RECT_CORNER_TOP_RIGHT    = 0x20,
	GUI_RECT_CORNER_BOTTOM_LEFT  = 0x40,
	GUI_RECT_CORNER_BOTTOM_RIGHT = 0x80,
	GUI_RECT_ALL_CORNERS         = 0xF0,
};

enum class RectangleFlatFlags {
	GUI_RECT_FLAT_NO_FLAT        = 0x00,
	GUI_RECT_FLAT_TOP_LEFT       = 0x01,
	GUI_RECT_FLAT_TOP_RIGHT      = 0x02,
	GUI_RECT_FLAT_BOTTOM_LEFT    = 0x04,
	GUI_RECT_FLAT_BOTTOM_RIGHT   = 0x08
};

}

#endif /* MIDDLEWARES_GUI_WIDGETS_COMMON_HPP_ */
