/*
 * @file ApplicationViewer.cpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 1 cze 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */

#include <vector>
#include <string>
#include <sstream>

//module-gui
#include "ApplicationViewer.hpp"

#include "service-appmgr/ApplicationManager.hpp"

#include "gui/widgets/Window.hpp"
#include "gui/widgets/Item.hpp"
#include "gui/widgets/Image.hpp"
#include "gui/widgets/Label.hpp"
#include "gui/widgets/BoxLayout.hpp"
#include "gui/core/ImageManager.hpp"
//module-utils
#include "log/log.hpp"
//module-services
#include "service-evtmgr/EventManager.hpp"
#include "service-evtmgr/messages/EVMessages.hpp"
//MessageType
#include "MessageType.hpp"
//this module

namespace app {

ApplicationViewer::ApplicationViewer(std::string name,uint32_t stackDepth,sys::ServicePriority priority) :
	Application( name, stackDepth, priority ) {
}

ApplicationViewer::~ApplicationViewer() {
	// TODO Auto-generated destructor stub
}

// Invoked upon receiving data message
sys::Message_t ApplicationViewer::DataReceivedHandler(sys::DataMessage* msgl) {

	auto retMsg = Application::DataReceivedHandler(msgl);
	//if message was handled by application's template there is no need to process further.
	if( (reinterpret_cast<sys::ResponseMessage*>( retMsg.get() )->retCode ==
		sys::ReturnCodes::Success ) ){
		return retMsg;
	}

	//this variable defines whether message was processed.
	bool handled = false;
	//if keyboard message received
	if(msgl->messageType == static_cast<uint32_t>(MessageType::KBDKeyEvent) )
	{
		sevm::KbdMessage* msg = static_cast<sevm::KbdMessage*>(msgl);
		LOG_INFO("Clock key received %d", static_cast<uint32_t>(msg->keyCode));

		if( msg->keyState == sevm::KeyboardEvents::keyReleasedShort ) {
			if( msg->keyCode == bsp::KeyCodes::JoystickLeft ) {
				sapm::ApplicationManager::messageSwitchApplication(this, "ApplicationClock", "", nullptr );
			}


 			uint32_t oldState = currentState;
			images[currentState]->setVisible(false);
			if( msg->keyCode == bsp::KeyCodes::FnLeft ) {
				currentState = states[currentState].leftState;
			}
			else if( msg->keyCode == bsp::KeyCodes::JoystickEnter ) {
				currentState = states[currentState].middleState;
			}
			else if( msg->keyCode == bsp::KeyCodes::FnRight ) {
				currentState = states[currentState].rightState;
			}
			images[currentState]->setVisible(true);
			if( oldState != currentState ) {
				if( states[currentState].deepRefresh )
					render(gui::RefreshModes::GUI_REFRESH_DEEP );
				else
					render(gui::RefreshModes::GUI_REFRESH_FAST );
			}
			handled = true;
		}
	}

	if( handled )
		return std::make_shared<sys::ResponseMessage>();
	else
		return std::make_shared<sys::ResponseMessage>(sys::ReturnCodes::Unresolved);
}

    // Invoked when timer ticked
void ApplicationViewer::TickHandler(uint32_t id) {

	render(gui::RefreshModes::GUI_REFRESH_FAST );
}

// Invoked during initialization
sys::ReturnCodes ApplicationViewer::InitHandler() {

	auto ret = Application::InitHandler();
	if( ret != sys::ReturnCodes::Success )
		return ret;

	//load all states from file
	auto file =  vfs.fopen("sys/viewerStates.txt","r");
	//read line with number of states
	std::string str = vfs.getline(file);
	uint32_t statesCount;
	std::stringstream(str) >> statesCount;

	LOG_INFO("States count: %d", statesCount );
	bool err = false;
	for( uint32_t i=0; i<statesCount; ++i ) {
		std::string stateName = vfs.getline(file);
		//check if there are brackets
		if( (stateName[0] == '[' ) && (stateName[stateName.size()-1] == ']')) {
			stateName = stateName.substr(1, stateName.size()-2 );

			std::string screenName = vfs.getline(file);
			std::string refreshModeStr = vfs.getline(file);
			std::string leftStateStr = vfs.getline(file);
			std::string middleStateStr = vfs.getline(file);
			std::string rightStateStr = vfs.getline(file);

			int32_t leftState = -1;
			int32_t middleState = -1;
			int32_t rightState = -1;

			bool deepRefresh = false;
			if( refreshModeStr[0] == 'F' || refreshModeStr[0] == 'f' )
				deepRefresh = true;

			std::stringstream(leftStateStr) >> leftState;
			std::stringstream(middleStateStr) >> middleState;
			std::stringstream(rightStateStr) >> rightState;
			states.push_back( ViewerState(stateName, screenName, deepRefresh, states.size(), leftState, middleState, rightState ) );

			LOG_INFO( "State: %s Screen: %s deep: %d,  L:%d, M:%d, R:%d", stateName.c_str(), screenName.c_str(), deepRefresh, leftState, middleState, rightState );
		}
		else {
			err = true;
			LOG_FATAL("ERROR reading viwer section");
			break;
		}
	}
	if( err )
		states.clear();

	vfs.fclose(file);

	createUserInterface();

	setActiveWindow("Main");

//	render(gui::RefreshModes::GUI_REFRESH_DEEP );

	return ret;
}

sys::ReturnCodes ApplicationViewer::DeinitHandler() {
	return sys::ReturnCodes::Success;
}

sys::ReturnCodes ApplicationViewer::WakeUpHandler() {
	return sys::ReturnCodes::Success;
}


sys::ReturnCodes ApplicationViewer::SleepHandler() {
	return sys::ReturnCodes::Success;
}

void ApplicationViewer::createUserInterface() {

	gui::Window* clockWin = new gui::Window("Main");
	clockWin->setSize( 480, 600 );

	for( ViewerState s : states ) {
		gui::Image* img = new gui::Image(clockWin,0,0,0,0);
		uint16_t id = gui::ImageManager::getInstance().getImageMapID(s.screenName);
		img->setImageWithID( id );
		img->setVisible(false);

		images.push_back( img );
	}

	currentState = 0;
	if( states.size() )
		images[currentState]->setVisible(true);

	windows.insert(std::pair<std::string,gui::Window*>(clockWin->getName(), clockWin));
}

void ApplicationViewer::destroyUserInterface() {
}

} /* namespace app */
