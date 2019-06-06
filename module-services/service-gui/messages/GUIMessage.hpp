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
#include "MessageType.hpp"

namespace sgui {

/*
 * @brief Template for all messages that go to gui service
 */
class GUIMessage: public sys::DataMessage {
public:
	GUIMessage( MessageType messageType ) : sys::DataMessage(static_cast<uint32_t>(messageType)) {};
	virtual ~GUIMessage() {};
};

} /* namespace sgui */

#endif /* MODULE_SERVICES_SERVICE_GUI_GUIMESSAGE_HPP_ */
