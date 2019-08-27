/*
 * @file NotesMainWindow.cpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 31 lip 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#include <memory>
#include <functional>

#include "service-appmgr/ApplicationManager.hpp"

#include "../ApplicationNotes.hpp"

#include "service-db/messages/DBMessage.hpp"
#include "i18/i18.hpp"

#include "Label.hpp"
#include "Margins.hpp"
#include "NotesMainWindow.hpp"

namespace gui {

NotesMainWindow::NotesMainWindow( app::Application* app ) :
	AppWindow(app,"MainWindow"),
	notesModel{ new NotesModel( app ) }{
	setSize( 480, 600 );

	buildInterface();
}

void NotesMainWindow::rebuild() {
	destroyInterface();
	buildInterface();
}
void NotesMainWindow::buildInterface() {
	AppWindow::buildInterface();

	list = new gui::ListView(this, 16, 104, 480-32, 440 );
	list->setMaxElements(3);
	list->setPageSize(3);
	list->setProvider( notesModel );

	setFocusItem(list);

	bottomBar->setActive( BottomBar::Side::CENTER, true );
	bottomBar->setActive( BottomBar::Side::RIGHT, true );
	bottomBar->setText( BottomBar::Side::CENTER, utils::localize.get("common_open"));
	bottomBar->setText( BottomBar::Side::RIGHT, utils::localize.get("common_back"));

	topBar->setActive(TopBar::Elements::TIME, true );

}
void NotesMainWindow::destroyInterface() {
	AppWindow::destroyInterface();
	children.clear();
	delete notesModel;
}

NotesMainWindow::~NotesMainWindow() {
	destroyInterface();
}

void NotesMainWindow::onBeforeShow( ShowMode mode, uint32_t command, SwitchData* data ) {
	notesModel->clear();
	notesModel->requestRecordsCount();
	list->clear();
	list->setElementsCount( notesModel->getItemCount() );
}

bool NotesMainWindow::onInput( const InputEvent& inputEvent ) {
	//check if any of the lower inheritance onInput methods catch the event
	bool ret = AppWindow::onInput( inputEvent );
	if( ret ) {
		//refresh window only when key is other than enter
		if( inputEvent.keyCode != KeyCode::KEY_ENTER )
			application->render( RefreshModes::GUI_REFRESH_FAST );
		return true;
	}

	//process only if key is released
	if(( inputEvent.state != InputEvent::State::keyReleasedShort ) &&
	   (( inputEvent.state != InputEvent::State::keyReleasedLong )))
		return false;

	if( inputEvent.keyCode == KeyCode::KEY_ENTER ) {
		LOG_INFO("Enter pressed");
	}
	else if( inputEvent.keyCode == KeyCode::KEY_RF ) {
		sapm::ApplicationManager::messageSwitchPreviousApplication(application);
		return true;
	}
	else if( inputEvent.keyCode == KeyCode::KEY_LEFT ) {
		application->switchWindow( "EditWindow", 0, nullptr );
		return true;
	}

	return false;
}

bool NotesMainWindow::onDatabaseMessage( sys::Message* msgl ) {
	DBNotesResponseMessage* msg = reinterpret_cast<DBNotesResponseMessage*>( msgl );
	if( notesModel->updateRecords( std::move(msg->records), msg->offset, msg->limit, msg->count ) )
		return true;

	return false;
}

} /* namespace gui */
