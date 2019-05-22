/*
 * @file GUIMessage.hpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 22 maj 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#ifndef MODULE_SERVICES_SERVICE_GUI_GUIMESSAGE_HPP_
#define MODULE_SERVICES_SERVICE_GUI_GUIMESSAGE_HPP_

#include "Service/Message.hpp"
#include "core/DrawCommand.hpp"

namespace sgui {

enum class GUIMessageType {
	Uninitialized,
	Commands, // list of rendering commands
	FocusInfo // information about application that gined focus
};

/*
 * @brief Template for all messages that go to gui service
 */
class GUIMessage: public sys::DataMessage {
public:
	//Identifies type of message within GUI Service
	const GUIMessageType messageType;

	GUIMessage( GUIMessageType messageType ) : sys::DataMessage(), messageType{ messageType } {};
	virtual ~GUIMessage() {};
};

} /* namespace sgui */

#endif /* MODULE_SERVICES_SERVICE_GUI_GUIMESSAGE_HPP_ */
