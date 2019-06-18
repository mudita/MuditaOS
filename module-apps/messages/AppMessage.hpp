/*
 * @file AppMessages.hpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 15 cze 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#ifndef MODULE_APPS_MESSAGES_APPMESSAGE_HPP_
#define MODULE_APPS_MESSAGES_APPMESSAGE_HPP_

#include <memory>
#include <string>
#include "SwitchData.hpp"

namespace app {

/*
 * @brief Template for all messages that go to application manager
 */
class AppMessage: public sys::DataMessage {
protected:
	//name of the application to which switch is performed.
	std::string application;
public:
	AppMessage( MessageType messageType, const std::string& application ) :
		sys::DataMessage( static_cast<uint32_t>(messageType)),
		application{application} {};
	virtual ~AppMessage() {};

	std::string getApplicationName() { return application;};
};

class AppSwitchMessage : public AppMessage {
protected:
	std::string window;
	std::unique_ptr<SwitchData> data;
public:
	AppSwitchMessage( const std::string& application, const std::string& window, std::unique_ptr<SwitchData> data ) :
		AppMessage( MessageType::AppSwitch, application),
		window{window},
		data {std::move(data)} {};
	virtual ~AppSwitchMessage() {};

	std::string getWindowName() { return window; };
	std::unique_ptr<app::SwitchData>& getData() { return data; };
};

};

#endif /* MODULE_APPS_MESSAGES_APPMESSAGE_HPP_ */
