/*
 * Window.h
 *
 *  Created on: 6 mar 2019
 *      Author: robert
 */

#ifndef MIDDLEWARES_GUI_WIDGETS_WINDOW_HPP_
#define MIDDLEWARES_GUI_WIDGETS_WINDOW_HPP_

#include <list>
#include "Item.hpp"
#include "Common.hpp"
#include "SwitchData.hpp"

namespace gui {

static uint32_t GUIWindowID=0;

class Window: public Item {
protected:
	//unique ID of the window within application.
	uint32_t windowID;
	RefreshModes refreshMode;
	std::string name;
	//pointer to the widget that has focus
	Item* focusItem;
public:

	Window( std::string name, uint32_t id=GUIWindowID++ );
	virtual ~Window();

	Item* getFocusItem(){return focusItem; };
	/**
	 * True should be returned when changes requires screen to be refreshed
	 */
//	virtual bool onDatabaseMessage( const dbus_msg_t* msg );
	//mode is respons
	virtual void onBeforeShow( ShowMode mode, uint32_t command, SwitchData* data  );
	virtual void getRefreshArea( RefreshModes& mode, uint16_t& x, uint16_t&y, uint16_t& w, uint16_t& h );
	virtual void setFocusItem( Item* item );
	virtual int getWindowID() {return windowID; };
	virtual bool handleSwitchData( SwitchData* data );

	//virtual methods from Item
	bool onInput( const InputEvent& inputEvent ) override;
	std::list<DrawCommand*> buildDrawList() override;
	std::string getName() { return name; };
};

} /* namespace gui */

#endif /* MIDDLEWARES_GUI_WIDGETS_WINDOW_HPP_ */
