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
	//name of the application that is sending message to application manager.
	std::string senderName;
public:
	AppMessage( MessageType messageType, const std::string& senderName ) :
		sys::DataMessage( static_cast<uint32_t>(messageType)),
		senderName{senderName} {};
	virtual ~AppMessage() {};

	std::string getSenderName() { return senderName;};
};

class AppFocusMessage : public AppMessage {
protected:
	bool focus;
public:
	AppFocusMessage( const std::string& senderName, const bool& focus ) :
		AppMessage( MessageType::AppFocus, senderName ),
		focus{ focus } {};
	bool getFocus() { return focus; };
};

};

#endif /* MODULE_APPS_MESSAGES_APPMESSAGE_HPP_ */
