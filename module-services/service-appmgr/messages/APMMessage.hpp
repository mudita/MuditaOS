/*
 * @file AppManMessage.hpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 4 cze 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#ifndef MODULE_SERVICES_SERVICE_APPMGR_MESSAGES_APMMESSAGE_HPP_
#define MODULE_SERVICES_SERVICE_APPMGR_MESSAGES_APMMESSAGE_HPP_

#include "Service/Message.hpp"
#include "MessageType.hpp"

namespace sapm {

/*
 * @brief Template for all messages that go to application manager
 */
class APMMessage: public sys::DataMessage {
protected:
	//name of the application that is sending message to application manager.
	std::string senderName;
public:
	APMMessage( MessageType messageType, const std::string& senderName ) :
		sys::DataMessage( static_cast<uint32_t>(messageType)),
		senderName{senderName} {};
	virtual ~APMMessage() {};

	std::string getSenderName() { return senderName;};
};


//	APMSwitch, //request to switch to given application, optionally also to specified window
//	APMSwitchData, //request to switch to given application, optionally also to specified window with provided data.
//	APMSwitchPrevApp, //Request to switch to previous application.
//	APMConfirmSwitch, //Used when application confirms that it is loosing focus and also when application confirms that is has gained focus
//	APMConfirmClose, //Sent by application to confirm completion of the close procedure

class APMSwitch : public APMMessage {
	std::string application;
	std::string window;
public:
	APMSwitch( const std::string& senderName, const std::string& applicationName, const std::string& windowName = "" ) :
		APMMessage( MessageType::APMSwitch, senderName ),
		application{ applicationName },
		window{ windowName } {

	}
};

class APMSwitchData : public APMMessage {
protected:
	std::string application;
	std::string window;
	std::unique_ptr<app::SwitchData> data;
public:
	APMSwitchData( const std::string& senderName, const std::string& applicationName, const std::string& windowName, std::unique_ptr<app::SwitchData> data ) :
		APMMessage( MessageType::APMSwitchData, senderName),
		application{ applicationName },
		window{ windowName },
		data{ std::move(data)} {
	}
};

class APMSwitchPrevApp : public APMMessage {
public:
	APMSwitchPrevApp( const std::string& name ) : APMMessage( MessageType::APMSwitchPrevApp, name ) {}
};

class APMConfirmSwitch : public APMMessage {
public:
	APMConfirmSwitch( const std::string& name ) : APMMessage( MessageType::APMConfirmSwitch, name ) {}
};

class APMConfirmClose: public APMMessage {
public:
	APMConfirmClose( const std::string& name ) :
		APMMessage( MessageType::APMConfirmClose, name ) {}
};


} /* namespace seink */



#endif /* MODULE_SERVICES_SERVICE_APPMGR_MESSAGES_APMMESSAGE_HPP_ */
