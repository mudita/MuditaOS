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

namespace gui {

ViewWindow::ViewWindow( app::Application* app ) : AppWindow(app,"MainWindow"){
	setSize( 480, 600 );

	buildInterface();
}

void ViewWindow::rebuild() {

}
void ViewWindow::buildInterface() {
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

	for( ViewerState s : states ) {
		gui::Image* img = new gui::Image(this,0,0,0,0);
		uint16_t id = gui::ImageManager::getInstance().getImageMapID(s.screenName);
		img->setImageWithID( id );
		img->setVisible(false);

		images.push_back( img );
	}
}
void ViewWindow::destroyInterface() {
	for( uint32_t i=0; i<images.size(); i++ )
		delete images[i];
}

ViewWindow::~ViewWindow() {
	destroyInterface();
}

void ViewWindow::onBeforeShow( ShowMode mode, uint32_t command, SwitchData* data ) {

	currentState = 0;
	if( states.size() )
		images[currentState]->setVisible(true);
}

bool ViewWindow::onInput( const InputEvent& inputEvent ) {
	//check if any of the lower inheritance onInput methods catch the event
	bool ret = AppWindow::onInput( inputEvent );
	if( ret )
		return true;

	//process only if key is released
	if(( inputEvent.state != InputEvent::State::keyReleasedShort ) &&
	   (( inputEvent.state != InputEvent::State::keyReleasedLong )))
		return true;

	if( (inputEvent.keyCode == KeyCode::KEY_ENTER) ||
		(inputEvent.keyCode == KeyCode::KEY_LF) ||
		(inputEvent.keyCode == KeyCode::KEY_RF )) {
		uint32_t oldState = currentState;
		images[currentState]->setVisible(false);
		if( inputEvent.keyCode == KeyCode::KEY_LF) {
			currentState = states[currentState].leftState;
		}
		else if( inputEvent.keyCode == KeyCode::KEY_ENTER ) {
			currentState = states[currentState].middleState;
		}
		else if( inputEvent.keyCode == KeyCode::KEY_RF ) {
			currentState = states[currentState].rightState;
		}
		images[currentState]->setVisible(true);
		if( oldState != currentState ) {
			if( states[currentState].deepRefresh )
				application->refreshWindow( RefreshModes::GUI_REFRESH_DEEP );
			else
				application->refreshWindow( RefreshModes::GUI_REFRESH_FAST );
		}
		return true;
	}

	return false;
}

} /* namespace gui */
