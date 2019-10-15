/*
 * @file Application.cpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 1 cze 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#include <sstream>
#include <iomanip>
//module-utils
#include "log/log.hpp"
//module-services
#include "service-gui/messages/DrawMessage.hpp"
#include "service-appmgr/messages/APMMessage.hpp"
#include "service-evtmgr/messages/EVMessages.hpp"
#include "service-appmgr/ApplicationManager.hpp"
#include "service-db/api/DBServiceAPI.hpp"
#include "service-cellular/ServiceCellular.hpp"
#include "service-cellular/api/CellularServiceAPI.hpp"
//module-gui
#include "gui/core/DrawCommand.hpp"
#include "gui/input/InputEvent.hpp"
//module-sys
#include "SystemManager/SystemManager.hpp"

#include "Application.hpp"
#include "MessageType.hpp"
#include "messages/AppMessage.hpp"
#include "windows/AppWindow.hpp"

namespace app {

Application::Application(std::string name, std::string parent,bool startBackground, uint32_t stackDepth,sys::ServicePriority priority) :
	Service( name, parent, stackDepth, priority ),
	startBackground{ startBackground } {

	longpressTimerID = CreateTimer( 1000 ,false);

	//create translator and set default profile
	translator = std::make_unique<gui::Translator>();
	translator->setProfile("home_screen");

	busChannels.push_back(sys::BusChannels::ServiceCellularNotifications);

}

Application::~Application() {
	for( auto it = windows.begin(); it!= windows.end(); it++)
		delete it->second;
	windows.clear();
}

void Application::TickHandler(uint32_t id) {
	if( id == longpressTimerID ) {
		gui::InputEvent iev = translator->translate(
			false,
			static_cast<int>(translator->getLastEvent().keyCode),
			translator->getLastEvent().keyPressTime,
			translator->getLastEvent().keyPressTime + translator->getLastEvent().timeout,
			true );

		//for press event check if there is a need for starting timer
		if( (iev.state == gui::InputEvent::State::keyPressed) && ( iev.timeout != 0 ) ) {
			Service::setTimerPeriod( longpressTimerID, iev.timeout );
			Service::ReloadTimer( longpressTimerID );
		}

		if( iev.state != gui::InputEvent::State::Undefined )
			messageInputEventApplication( this, this->GetName(), iev );
	}
}

void Application::render( gui::RefreshModes mode ) {
	if( currentWindow == nullptr ){
		LOG_ERROR("Current window is not defined");
		return;
	}

	//send drawing commands only when if application is in active and visible.
	if( state == State::ACTIVE_FORGROUND ) {
		std::list<gui::DrawCommand*> commandsList = currentWindow->buildDrawList();

		if( shutdownInProgress ) {
			auto msg = std::make_shared<sgui::DrawMessage>(commandsList, mode, sgui::DrawMessage::DrawCommand::SHUTDOWN );
			sys::Bus::SendUnicast(msg, "ServiceGUI", this);
		}
		else if( suspendInProgress ) {
			auto msg = std::make_shared<sgui::DrawMessage>(commandsList, mode, sgui::DrawMessage::DrawCommand::SUSPEND );
			sys::Bus::SendUnicast(msg, "ServiceGUI", this);
		}
		else {
			auto msg = std::make_shared<sgui::DrawMessage>(commandsList, mode, sgui::DrawMessage::DrawCommand::NORMAL );
			sys::Bus::SendUnicast(msg, "ServiceGUI", this);
		}
	}

	if( suspendInProgress )
		suspendInProgress = false;
}
void Application::blockEvents(bool isBlocked ) {
	acceptInput = isBlocked;
}
int Application::switchWindow( const std::string& windowName, gui::ShowMode cmd, std::unique_ptr<gui::SwitchData> data ) {

	std::string window;
	LOG_INFO("switching to window: %s", windowName.c_str());

	//case to handle returning to previous application
	if( windowName == "LastWindow" ) {
		window = currentWindow->getName();
		auto msg = std::make_shared<AppSwitchWindowMessage>( window, std::make_unique<gui::SwitchData>("LastWindow"), cmd);
		sys::Bus::SendUnicast(msg, this->GetName(), this );
	}
	else {
		window = windowName.empty() ? "MainWindow" : windowName;
		auto msg = std::make_shared<AppSwitchWindowMessage>( window, std::move(data), cmd );
		sys::Bus::SendUnicast(msg, this->GetName(), this );
	}

	return 0;
}

// TODO: this one seems to be unused
int Application::switchBackWindow( const std::string& windowName, uint32_t cmd, std::unique_ptr<gui::SwitchData> data ) {
	auto msg = std::make_shared<AppMessage>( MessageType::AppSwitchWindowBack );
	sys::Bus::SendUnicast(msg, this->GetName(), this );

	return 0;
}
int Application::refreshWindow(gui::RefreshModes mode) {
	auto msg = std::make_shared<AppRefreshMessage>( mode );
	sys::Bus::SendUnicast(msg, this->GetName(), this );

	return 0;
}

sys::Message_t Application::DataReceivedHandler(sys::DataMessage* msgl) {

	bool handled = false;

	if( msgl->messageType == static_cast<int32_t>(MessageType::CellularNotification) ) {
		CellularNotificationMessage *msg = reinterpret_cast<CellularNotificationMessage *>(msgl);
		if( msg->type == CellularNotificationMessage::Type::SignalStrengthUpdate ) {
			if( ( state == State::ACTIVE_FORGROUND ) && (currentWindow->updateSignalStrength( msg->signalStrength) ) ) {
				//loop and update all widnows
				for ( auto it = windows.begin(); it != windows.end(); it++ ) {
					it->second->updateSignalStrength( msg->signalStrength);
				}
				handled = true;
				refreshWindow( gui::RefreshModes::GUI_REFRESH_FAST );
			}
		}
	}

	if(msgl->messageType == static_cast<uint32_t>(MessageType::AppInputEvent) ) {
		AppInputEventMessage* msg = reinterpret_cast<AppInputEventMessage*>( msgl );
		if( currentWindow != nullptr )
			currentWindow->onInput( msg->getEvent() );

//		LOG_INFO( "Key event :%s", msg->getEvent().to_string().c_str());
		handled = true;
	}
	else if(msgl->messageType == static_cast<uint32_t>(MessageType::KBDKeyEvent) )
	{
		sevm::KbdMessage* msg = static_cast<sevm::KbdMessage*>(msgl);

		gui::InputEvent iev = translator->translate(
			(msg->keyState==sevm::KeyboardEvents::keyPressed),
			static_cast<uint32_t>(msg->keyCode),
			msg->keyPressTime,
			msg->keyRelaseTime
			);

		//for press event check if there is a need for starting timer
		if( (iev.state == gui::InputEvent::State::keyPressed) && ( iev.timeout != 0 ) ) {
			Service::setTimerPeriod( longpressTimerID, iev.timeout );
			Service::ReloadTimer( longpressTimerID );
		}
		else if(iev.state == gui::InputEvent::State::keyReleasedShort ) {
			Service::stopTimer( longpressTimerID );
		}

		if( iev.state != gui::InputEvent::State::Undefined )
			messageInputEventApplication( this, this->GetName(), iev );

		handled = true;
	}
	else if(msgl->messageType == static_cast<uint32_t>(MessageType::EVMBatteryLevel) )
	{
		sevm::BatteryLevelMessage* msg = static_cast<sevm::BatteryLevelMessage*>(msgl);
		LOG_INFO("Application battery level: %d", msg->levelPercents );

		if( currentWindow ) {
			if( currentWindow->updateBatteryLevel( msg->levelPercents ) )
				refreshWindow( gui::RefreshModes::GUI_REFRESH_FAST );
		}

		handled = true;
	}
	else if(msgl->messageType == static_cast<uint32_t>(MessageType::EVMChargerPlugged) )
	{
		sevm::BatteryPlugMessage* msg = static_cast<sevm::BatteryPlugMessage*>(msgl);
		if(msg->plugged == true) {
			//TODO show plug icon
			LOG_INFO("Application charger connected" );
		}
		else {
			//hide plug icon
			LOG_INFO("Application charger disconnected" );
		}

		refreshWindow( gui::RefreshModes::GUI_REFRESH_FAST );
		handled = true;
	}
	else if(msgl->messageType == static_cast<uint32_t>(MessageType::EVMMinuteUpdated))
	{
		sevm::RtcMinuteAlarmMessage* msg = static_cast<sevm::RtcMinuteAlarmMessage*>(msgl);
		LOG_INFO("Application time updated");

		currentWindow->updateTime( msg->timestamp, !settings.timeFormat12 );

		if( state == State::ACTIVE_FORGROUND )
			refreshWindow( gui::RefreshModes::GUI_REFRESH_FAST );

		handled = true;
	}

	else if(msgl->messageType == static_cast<uint32_t>(MessageType::AppSwitch) ) {

		AppSwitchMessage* msg = reinterpret_cast<AppSwitchMessage*>( msgl );
		//Application is starting or it is in the background. Upon switch command if name if correct it goes foreground
		if( ( state == State::INITIALIZING ) ||	( state == State::ACTIVE_BACKGROUND )){

			if( msg->getTargetApplicationName() == this->GetName()) {
				if( sapm::ApplicationManager::messageConfirmSwitch(this) ) {
					state = State::ACTIVE_FORGROUND;

					switchWindow( msg->getTargetWindowName(), std::move( msg->getData()));
					handled = true;
				}
				else {
					//TODO send to itself message to close
					LOG_ERROR("Failed to communicate ");
				}
			}
			else {
				LOG_ERROR("Received switch message outside of activation flow");
			}
		}
		else if( state == State::ACTIVE_FORGROUND ) {
			if( msg->getTargetApplicationName() == this->GetName()) {
				//if window name and data are null pointers this is a message informing
				//that application should go to background mode
				if( (msg->getTargetWindowName() == "") && (msg->getData() == nullptr ) ) {
					if( sapm::ApplicationManager::messageConfirmSwitch(this) ) {
						state = State::ACTIVE_BACKGROUND;
						handled = true;
					}
					else {
						//TODO send to itself message to close
						LOG_ERROR("Failed to communicate ");
					}
				}
				//if application is in front and receives message with defined window it should
				//change to that window.
				else {
					switchWindow( msg->getTargetWindowName(), std::move( msg->getData()));
					handled = true;
				}
			}
			else {
				LOG_ERROR("Received switch message outside of activation flow");
			}
		}
		else {
			LOG_ERROR("Wrong internal application to switch to active state");
		}
	}
	else if(msgl->messageType == static_cast<uint32_t>(MessageType::AppSwitchWindow ) ) {
		AppSwitchWindowMessage* msg = reinterpret_cast<AppSwitchWindowMessage*>( msgl );
		//check if specified window is in the application
		auto it = windows.find( msg->getWindowName() );
		if( it != windows.end() ) {

			setActiveWindow( msg->getWindowName() );

			currentWindow->handleSwitchData( msg->getData().get() );

			//check if this is case where application is returning to the last visible window.
			if( (msg->getData() != nullptr) && (msg->getData()->getDescription() == "LastWindow") ) {
				//do nothing here
			}
			else {
				currentWindow->onBeforeShow( msg->getCommand(), msg->getData().get() );
			}

			refreshWindow( gui::RefreshModes::GUI_REFRESH_DEEP );

		}
		handled = true;
	}

	else if( msgl->messageType == static_cast<uint32_t>(MessageType::AppClose)) {
		state = State::DEACTIVATING;
		sapm::ApplicationManager::messageConfirmClose(this);
		//here should go all the cleaning
		handled = true;
	}
	else if( msgl->messageType == static_cast<uint32_t>(MessageType::AppRebuild )) {

		LOG_INFO("Application %s rebuilding gui", GetName().c_str() );
		//for all windows call rebuild method
		for( auto it = windows.begin(); it!= windows.end(); it++)
			it->second->rebuild();
		//if application has focus call deep refresh
		if( state == State::ACTIVE_FORGROUND )
			refreshWindow( gui::RefreshModes::GUI_REFRESH_DEEP );
		handled = true;
	}
	else if( msgl->messageType == static_cast<uint32_t>(MessageType::AppRefresh)) {
		AppRefreshMessage* msg = reinterpret_cast<AppRefreshMessage*>( msgl );
		//currentWindow->onBeforeShow( gui::ShowMode::GUI_SHOW_RETURN, 0, nullptr );
		render( msg->getMode() );
		handled = true;
	}

	if( handled)
		return std::make_shared<sys::ResponseMessage>();
	else
		return std::make_shared<sys::ResponseMessage>(sys::ReturnCodes::Unresolved);
}

sys::ReturnCodes Application::InitHandler() {
	bool initState= true;
	state = State::INITIALIZING;
//	uint32_t start = xTaskGetTickCount();
	settings = DBServiceAPI::SettingsGet(this);
//	uint32_t stop = xTaskGetTickCount();
//	LOG_INFO("DBServiceAPI::SettingsGet %d", stop-start);
	initState = (settings.dbID == 1);

	//send response to application manager true if successful, false otherwise.
	sapm::ApplicationManager::messageRegisterApplication( this, initState, startBackground );
	sys::ReturnCodes retCode = (initState?sys::ReturnCodes::Success:sys::ReturnCodes::Failure);
	return retCode;
}

void Application::setActiveWindow( const std::string& windowName ) {
	auto it = windows.find(windowName);

	//if there is a window with specified name set it as active window
	//and unlock accepting keyboard events
	if( it!=windows.end() ) {
		previousWindow = currentWindow;
		currentWindow = it->second;
		acceptInput = true;
	}
}

void Application::setKeyboardProfile( const std::string& profile ) {
	translator->setProfile( profile );
}

bool Application::messageSwitchApplication( sys::Service* sender, std::string application, std::string window, std::unique_ptr<gui::SwitchData> data ) {
	auto msg = std::make_shared<AppSwitchMessage>( application, window, std::move(data) );
	sys::Bus::SendUnicast(msg, application, sender );
	return true;
}

bool Application::messageRefreshApplication( sys::Service* sender, std::string application, std::string window, gui::SwitchData* data ) {
	auto msg = std::make_shared<AppMessage>( MessageType::AppRefresh );
	sys::Bus::SendUnicast(msg, application, sender  );
	return true;
}

bool Application::messageCloseApplication( sys::Service* sender, std::string application ) {

	auto msg = std::make_shared<AppMessage>( MessageType::AppClose );
	sys::Bus::SendUnicast(msg, application, sender  );
	return true;
}

bool Application::messageRebuildApplication( sys::Service* sender, std::string application ) {
	auto msg = std::make_shared<AppRebuildMessage>();
	sys::Bus::SendUnicast(msg, application, sender  );
	return true;
}

bool Application::messageInputEventApplication( sys::Service* sender, std::string application , const gui::InputEvent& event ) {
	auto msg = std::make_shared<AppInputEventMessage>( event );
	sys::Bus::SendUnicast(msg, application, sender );
	return true;
}

} /* namespace app */
