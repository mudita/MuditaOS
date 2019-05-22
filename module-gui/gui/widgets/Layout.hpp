/*
 * Layout.hpp
 *
 *  Created on: 15 maj 2019
 *      Author: robert
 */

#ifndef GUI_WIDGETS_LAYOUT_HPP_
#define GUI_WIDGETS_LAYOUT_HPP_

namespace gui {

enum class LayoutHorizontalPolicy {
	LAYOUT_POLICY_HORIZONTAL_EXPAND = 0x01,
	LAYOUT_POLICY_HORIZONTAL_SHIRNH = 0x02,
	LAYOUT_POLICY_HORIZONTAL_STATIC = 0x04,
};

enum class LayoutVerticalPolicy {
	LAYOUT_POLICY_VERTICAL_EXPAND = 0x01,
	LAYOUT_POLICY_VERTICAL_SHRINK = 0x02,
	LAYOUT_POLICY_VERTICAL_STATIC = 0x04,
};

class Layout {
public:

	Layout();
	virtual ~Layout();
};

} /* namespace gui */

#endif /* GUI_WIDGETS_LAYOUT_HPP_ */
