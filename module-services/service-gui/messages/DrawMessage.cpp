/*
 * @file DrawMessage.cpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 22 maj 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#include "GUIMessage.hpp"
#include "DrawMessage.hpp"
#include "MessageType.hpp"

namespace sgui {

DrawMessage::DrawMessage( const std::list< gui::DrawCommand* >& commandsList, gui::RefreshModes mode, bool suspend ) :
	GUIMessage(MessageType::GUICommands),
	suspend{ suspend } {

	this->mode = mode;
	for( auto cmd : commandsList ) {
		if( cmd )
			commands.push_back( std::unique_ptr<gui::DrawCommand>(cmd));
	}
}

DrawMessage::~DrawMessage() {
}

} /* namespace sgui */
