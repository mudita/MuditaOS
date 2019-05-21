/*
 * GUI.cpp
 *
 *  Created on: 10 maj 2019
 *      Author: robert
 */

#include "GUI.hpp"
#include "core/Font.hpp"

namespace gui {

GUI::GUI( std::string assetsPath ) : assetsPath{assetsPath} {
	//initialize Font Manager
	FontManager& fontManager = FontManager::getInstance();

	fontManager.init( assetsPath );
}

GUI::~GUI() {
}

} /* namespace gui */
