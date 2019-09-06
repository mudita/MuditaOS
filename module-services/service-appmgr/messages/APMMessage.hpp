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

#include "SwitchData.hpp"

#include "i18/i18.hpp"

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
	std::unique_ptr<gui::SwitchData> data;
public:
	APMSwitch( const std::string& senderName, const std::string& applicationName, const std::string& windowName, std::unique_ptr<gui::SwitchData> data ) :
		APMMessage( MessageType::APMSwitch, senderName ),
		application{ applicationName },
		window{ windowName },
		data{ std::move(data) }{
	}
	const std::string& getName() const { return application; };
	const std::string& getWindow() const { return window; };
	std::unique_ptr<gui::SwitchData>& getData() { return data; };
};

class APMSwitchPrevApp : public APMMessage {
	std::unique_ptr<gui::SwitchData> data;
public:
	APMSwitchPrevApp( const std::string& name, std::unique_ptr<gui::SwitchData> data = nullptr) :
		APMMessage( MessageType::APMSwitchPrevApp, name ),
		data{ std::move(data) } {}
	std::unique_ptr<gui::SwitchData>& getData() { return data; };
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

class APMRegister: public APMMessage {
protected:
	bool status;
	bool startBackground;
public:
	APMRegister( const std::string& senderName, const bool& status, const bool& startBackground  ) :
	APMMessage( MessageType::APMRegister, senderName ),
	status{ status },
	startBackground{ startBackground }{}

	const bool& getStatus() { return status; };
	const bool& getStartBackground() { return startBackground; };
};

class APMDelayedClose: public APMMessage {
protected:
	std::string application;
public:
	APMDelayedClose( const std::string& senderName, std::string application  ) :
	APMMessage( MessageType::APMDeleydClose, senderName ),
	application{ application } {}

	const std::string& getApplication() { return application; };
};

class APMChangeLanguage: public APMMessage {
protected:
	utils::Lang language;
public:
	APMChangeLanguage( const std::string& senderName, utils::Lang language ) :
	APMMessage( MessageType::APMChangeLanguage, senderName ),
	language{ language } {}

	const utils::Lang& getLanguage() { return language; };
};

class APMClose: public APMMessage {
public:
	APMClose( const std::string& senderName) :
	APMMessage( MessageType::APMClose, senderName )
	{}
};

class APMPreventBlocking: public APMMessage {
public:
	APMPreventBlocking( const std::string& senderName) :
	APMMessage( MessageType::APMPreventBlocking, senderName )
	{}
};

} /* namespace sapm */



#endif /* MODULE_SERVICES_SERVICE_APPMGR_MESSAGES_APMMESSAGE_HPP_ */
