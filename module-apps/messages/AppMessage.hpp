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
#include "gui/input/InputEvent.hpp"
#include "Service/Message.hpp"
#include "MessageType.hpp"

namespace app {

/*
 * @brief Template for all messages that go to application manager
 */
class AppMessage: public sys::DataMessage {
public:
	AppMessage( MessageType messageType ) :	sys::DataMessage( static_cast<uint32_t>(messageType)) {};
	virtual ~AppMessage() {};

};

//this message is used to notify application about switching event. Application will gain or lose focus upon receiving this message.
//Application gains focus when it was in init or active background state. Application loose focus when it was in active foreground state.
//if no window is specified it is assumed that MainWindow is the target
class AppSwitchMessage : public AppMessage {
protected:
	//name of the application to which switch is performed.
	std::string targetApplication;
	//name of the window to which switch should be performed.
	std::string targetWindow;
	//optional data for the target window.
	std::unique_ptr<gui::SwitchData> data;
	//name of the application to which switch should be performed after finishing tasks in target application and window.
	std::string returnApplication;
	//name of the window to which switch should be performed after finishing tasks in target application and window.
	std::string returnWindow;
public:
	AppSwitchMessage( const std::string& targetApplication, const std::string& targetWindow, std::unique_ptr<gui::SwitchData> data,
			const std::string& returnApplication = "", const std::string& returnWindow = "") :
		AppMessage( MessageType::AppSwitch ),
		targetApplication{ targetApplication },
		targetWindow{ targetWindow },
		data {std::move(data)},
		returnApplication{ returnApplication },
		returnWindow{ returnWindow } {};
	virtual ~AppSwitchMessage() {};

	std::string getTargetWindowName() { return targetWindow; };
	std::string getReturnWindowName() { return returnWindow; };
	std::unique_ptr<gui::SwitchData>& getData() { return data; };
	std::string getTargetApplicationName() { return targetApplication;};
	std::string getReturnApplicationName() { return returnApplication;};
};

class AppRefreshMessage : public AppMessage {
protected:
	gui::RefreshModes mode;
public:
//	AppRefreshMessage( const std::string& application, gui::RefreshModes mode ) :
		AppRefreshMessage( gui::RefreshModes mode ) :
		AppMessage( MessageType::AppRefresh ),
		mode{ mode }{};
	virtual ~AppRefreshMessage() {};

	const gui::RefreshModes& getMode() { return mode; };
};

class AppSwitchWindowMessage : public AppMessage {
protected:
	std::string window;
	std::string senderWindow;
	gui::ShowMode command;
	std::unique_ptr<gui::SwitchData> data;
public:
	AppSwitchWindowMessage() = delete;

	AppSwitchWindowMessage( const std::string& window, const std::string senderWindow, std::unique_ptr<gui::SwitchData> data, const gui::ShowMode command = gui::ShowMode::GUI_SHOW_INIT) :
		AppMessage( MessageType::AppSwitchWindow ),
		window{window},
		senderWindow{ senderWindow },
		command{ command },
		data{std::move(data)} {};
	virtual ~AppSwitchWindowMessage() = default;

	const std::string& getWindowName() const { return window; };
	const std::string& getSenderWindowName() const { return senderWindow; };
	const gui::ShowMode& getCommand() const { return command; };
	std::unique_ptr<gui::SwitchData>& getData() { return data; };
};

class AppInputEventMessage : public AppMessage {
protected:
	gui::InputEvent event;
public:
	AppInputEventMessage( gui::InputEvent evt ) :
		AppMessage( MessageType::AppInputEvent ),
		event{evt} {};
	virtual ~AppInputEventMessage() {};

	const gui::InputEvent& getEvent() { return event; };
};

class AppRebuildMessage : public AppMessage {
public:
	AppRebuildMessage(): AppMessage( MessageType::AppRebuild ) {};
	virtual ~AppRebuildMessage() {};
};

};
#endif /* MODULE_APPS_MESSAGES_APPMESSAGE_HPP_ */
