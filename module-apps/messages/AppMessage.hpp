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

};

#endif /* MODULE_APPS_MESSAGES_APPMESSAGE_HPP_ */
