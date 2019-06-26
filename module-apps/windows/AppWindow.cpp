/*
 * @file AppWindow.cpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 24 cze 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#include "AppWindow.hpp"

namespace gui {

AppWindow::AppWindow( app::Application* app, std::string name, uint32_t id ) :
	Window ( name, id), application{ app } {
}

AppWindow::~AppWindow() {
}

} /* namespace gui */
