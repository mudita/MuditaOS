/*
 * Window.cpp
 *
 *  Created on: 6 mar 2019
 *      Author: robert
 */
#include <algorithm>
//gui
#include "../core/BoundingBox.hpp"
#include "../Common.hpp"
#include "Window.hpp"
#include "../core/DrawCommand.hpp"

namespace gui {

Window::Window( std::string name, uint32_t id ) :
	Item(),
	windowID{id},
	refreshMode{RefreshModes::GUI_REFRESH_FAST},
	name{name},
	focusItem{nullptr}
	 {
}

Window::~Window() {
}

void Window::onBeforeShow( ShowMode mode, uint32_t command, SwitchData* data ) {
}

void Window::getRefreshArea( RefreshModes& mode, uint16_t& x, uint16_t&y, uint16_t& w, uint16_t& h ) {
	x = widgetArea.x;
	y = widgetArea.y;
	w = widgetArea.w;
	h = widgetArea.h;
	mode = refreshMode;
}

bool Window::handleSwitchData( SwitchData* data ) {
	return true;
}

void Window::setFocusItem( Item* item ) {

	//check if item is a child of the window
	auto it = std::find(children.begin(), children.end(), item);
	if( it == children.end())
		return;

	//remove focus from previous item
	if( focusItem != nullptr )
		focusItem->setFocus( false );

	focusItem = item;
	if( focusItem )
		focusItem->setFocus( true );
}

std::list<DrawCommand*> Window::buildDrawList() {

	std::list<DrawCommand*> commands;
	std::list<DrawCommand*> childrenCommands = Item::buildDrawList();

	DrawCommand* clearCommand = new DrawCommand();
	clearCommand->id = DrawCommandID::GUI_DRAW_CLEAR;

	commands.push_back(clearCommand);

	if( !childrenCommands.empty() ) {
		commands.splice( commands.end(), childrenCommands );
	}

	return commands;
}

//bool Window::onDatabaseMessage( const dbus_msg_t* msg ) {
//	return false;
//}


bool Window::onInput( const InputEvent& inputEvent) {
	bool res;
	if( focusItem != nullptr )
		res =  focusItem->onInput(inputEvent);

	if( res )
		return true;
	//if focused item didn't handle the key event and it was navigation key
	//check if moving focus is possible
	gui::Item* newFocusItem = nullptr;
	if( !res && inputEvent.state == InputEvent::State::keyReleasedShort ) {
		switch( inputEvent.keyCode ) {
		case KeyCode::KEY_LEFT:
			newFocusItem = focusItem->getNavigationItem(gui::NavigationDirection::LEFT);
			break;
		case KeyCode::KEY_RIGHT:
			newFocusItem = focusItem->getNavigationItem(gui::NavigationDirection::RIGHT);
			break;
		case KeyCode::KEY_UP:
			newFocusItem = focusItem->getNavigationItem(gui::NavigationDirection::UP);
			break;
		case KeyCode::KEY_DOWN:
			newFocusItem = focusItem->getNavigationItem(gui::NavigationDirection::DOWN);
			break;
		default:
			break;
		}

	}

	if( newFocusItem != nullptr ) {
		setFocusItem(newFocusItem);
		return true;
	}

	return false;
}

} /* namespace gui */
