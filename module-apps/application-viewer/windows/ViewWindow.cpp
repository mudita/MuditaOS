/*
 * @file ViewWindow.cpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 2 lip 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#include "vfs.hpp"
#include "gui/widgets/Image.hpp"
#include "gui/core/ImageManager.hpp"
#include "ViewWindow.hpp"
#include "AppWindow.hpp"
#include "Application.hpp"

namespace gui {

    ViewWindow::ViewWindow(app::Application *app) : AppWindow(app, gui::name::window::main_window)
    {
        buildInterface();
    }

void ViewWindow::rebuild() {
	destroyInterface();
	buildInterface();
}

void ViewWindow::buildInterface() {
	AppWindow::buildInterface();
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
			std::string buttonStr = vfs.getline(file);

		    // Vector of string to save pairs of button code and image
		    std::vector <std::string> tokens;
		    std::stringstream check1(buttonStr);
		    std::string intermediate;

		    // Tokenizing w.r.t. space ' '
		    while(getline(check1, intermediate, ','))
		    {
		        tokens.push_back(intermediate);
		    }

			bool deepRefresh = false;
			if( refreshModeStr[0] == 'F' || refreshModeStr[0] == 'f' )
				deepRefresh = true;

			std::map<int, int> buttons;
			for(unsigned int j = 0; j < tokens.size(); j++) {
				LOG_INFO("TOKEN: %s", tokens[j].c_str());
				std::vector <std::string> tokens2;
				std::stringstream check2(tokens[j]);
				while(getline(check2, intermediate, ' ')) {
					tokens2.push_back(intermediate);
				}

				if( tokens2.size() != 2 )
					continue;

				//convert first token to key id
				auto it = buttonKeyCodes.find( tokens2[0]);
				if( it != buttonKeyCodes.end()) {

					int screenCode;
					std::stringstream(tokens2[1]) >> screenCode;
					LOG_INFO("[%d] [%d]", it->second, screenCode );

					buttons.insert( std::pair<int,int>( it->second, screenCode ));

				}
			}

			states.push_back( ViewerState(stateName, screenName, deepRefresh, states.size(), buttons ) );

			LOG_INFO( "State: %s Screen: %s deep: %d", stateName.c_str(), screenName.c_str(), deepRefresh );
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

	for( ViewerState s : states ) {
		gui::Image* img = new gui::Image(this,0,0,0,0);
		uint16_t id = gui::ImageManager::getInstance().getImageMapID(s.screenName);
		img->setImageWithID( id );
		img->setVisible(false);

		images.push_back( img );
	}
}
void ViewWindow::destroyInterface() {
	destroyInterface();
	for( uint32_t i=0; i<images.size(); i++ )
		delete images[i];
}

ViewWindow::~ViewWindow() {
	destroyInterface();
}

void ViewWindow::onBeforeShow( ShowMode mode, SwitchData* data ) {

	currentState = 0;
	if( states.size() )
		images[currentState]->setVisible(true);
}

bool ViewWindow::onInput( const InputEvent& inputEvent ) {
	//process only if key is released
	if( inputEvent.state == InputEvent::State::keyReleasedShort ) {

		uint32_t oldState = currentState;

		for( auto it = states[currentState].buttons.begin(); it!=states[currentState].buttons.end(); it++)
			LOG_INFO("[%d %d]", it->first, it->second);

		auto it = states[currentState].buttons.find(static_cast<int>(inputEvent.keyCode));
		if( it == states[currentState].buttons.end() )
			return false;

		images[oldState]->setVisible(false);
		currentState = it->second;
				images[currentState]->setVisible(true);
		if( oldState != currentState ) {
			if( states[currentState].deepRefresh )
				application->refreshWindow( RefreshModes::GUI_REFRESH_DEEP );
			else
				application->refreshWindow( RefreshModes::GUI_REFRESH_FAST );
		}
		return true;
	}

	//check if any of the lower inheritance onInput methods catch the event
	return AppWindow::onInput( inputEvent );
}

} /* namespace gui */
