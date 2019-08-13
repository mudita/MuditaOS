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

	setFocusItem(list);

//	bottomBar->setActive( BottomBar::Side::CENTER, true );
//	bottomBar->setActive( BottomBar::Side::RIGHT, true );
//	bottomBar->setText( BottomBar::Side::CENTER, utils::localize.get("common_open"));
//	bottomBar->setText( BottomBar::Side::RIGHT, utils::localize.get("common_back"));
//
//	topBar->setActive(TopBar::Elements::SIGNAL, true );
//	topBar->setActive(TopBar::Elements::BATTERY, true );
//
//	title = new gui::Label(this, 0, 50, 480, 50 );
//	title->setFilled( false );
//	title->setBorderColor( gui::ColorNoColor );
//	title->setFont("gt_pressura_bold_24");
//	title->setText(utils::localize.get("app_settings_title_main"));
//	title->setAlignement( gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_CENTER, gui::Alignment::ALIGN_VERTICAL_CENTER));
//
//	//add option connectivity option
//	options.push_back( addOptionLabel( utils::localize.get("app_settings_connectivity"), [=] (gui::Item& item){ return true; }) );
//
//	//add option date and time option
//	options.push_back( addOptionLabel( utils::localize.get("app_settings_date_and_time"), [=](gui::Item&){ return true;}) );
//
//	//add option display option
//	options.push_back( addOptionLabel( utils::localize.get("app_settings_display"), [=](gui::Item&){ return true;} ));
//
//	//add option phone mode option
//	options.push_back( addOptionLabel( utils::localize.get("app_settings_phone_modes"), [=](gui::Item&){ return true;} ));
//
//	//add option security option
//	options.push_back( addOptionLabel( utils::localize.get("app_settings_security"), [=](gui::Item&){ return true;} ));
//
//	//add option language option
//	options.push_back( addOptionLabel( utils::localize.get("app_settings_language"), [=](gui::Item&){
//		LOG_INFO("switching to language page" );
//		application->switchWindow("Languages", 0, nullptr );
//		return true;} ));
//
//	//add option security option
//	options.push_back( addOptionLabel( utils::localize.get("app_settings_about"), [=](gui::Item&){ return true;} ));
//
//	//set position and navigation for labels
//	uint32_t posY = 100;
//	uint32_t size = options.size();
//	for( uint32_t i=0; i<options.size(); i++ ){
//		options[i]->setPosition(17,posY);
//		posY += 60;
//		options[i]->setNavigationItem( NavigationDirection::DOWN, options[(i+1)%size]);
//		options[i]->setNavigationItem( NavigationDirection::UP, options[(size+i-1)%size]);
//	}
}
void NotesMainWindow::destroyInterface() {
	AppWindow::destroyInterface();
//	delete title;
//	for( uint32_t i=0; i<options.size(); i++ )
//		delete options[i];
//	options.clear();
//	this->focusItem = nullptr;
//	LOG_INFO("options size: %d", options.size());
	children.clear();
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

	return false;
}

bool NotesMainWindow::onDatabaseMessage( sys::Message* msgl ) {
	DBNotesResponseMessage* msg = reinterpret_cast<DBNotesResponseMessage*>( msgl );
	if( notesModel->updateRecords( std::move(msg->records), msg->offset, msg->limit, msg->count ) )
		return true;

	return false;
}

} /* namespace gui */
