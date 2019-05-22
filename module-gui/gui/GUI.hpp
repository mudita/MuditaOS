/*
 * GUI.hpp
 *
 *  Created on: 10 maj 2019
 *      Author: robert
 */

#ifndef GUI_GUI_HPP_
#define GUI_GUI_HPP_

#include "core/Font.hpp"

namespace gui {

class GUI {
	std::string assetsPath;
public:
	GUI( std::string assetsPath );
	virtual ~GUI();
};

} /* namespace gui */

#endif /* GUI_GUI_HPP_ */
