/*
 * @file CallLogMainWindow.cpp
 * @author Aleksander Rudnik (aleksander.rudnik@mudita.com)
 * @date 19.09.2019
 * @brief Application Call Log Main Window
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#include "CallLogMainWindow.hpp"
#include <memory>
#include <functional>

#include "service-appmgr/ApplicationManager.hpp"

#include "../ApplicationCallLog.hpp"

#include "service-db/messages/DBMessage.hpp"
#include "i18/i18.hpp"

#include "Label.hpp"
#include "Margins.hpp"
#include "../data/CallLogInternals.hpp" // TODO: alek: add easier paths
#include <Style.hpp>

using namespace style;

namespace gui {

CallLogMainWindow::CallLogMainWindow( app::Application* app ) :
	AppWindow( app, calllog::settings::MainWindowStr ), calllogModel{ new CalllogModel( app ) } {

	buildInterface();
}

void CallLogMainWindow::rebuild() {
	destroyInterface();
	buildInterface();
}
void CallLogMainWindow::buildInterface() {
	AppWindow::buildInterface();

	setTitle(utils::localize.get("app_calllog_title_main"));

	bottomBar->setText( BottomBar::Side::LEFT, utils::localize.get("common_call") );
	bottomBar->setText( BottomBar::Side::CENTER, utils::localize.get("common_open") );
	bottomBar->setText( BottomBar::Side::RIGHT, utils::localize.get("common_back") );

	topBar->setActive( TopBar::Elements::TIME, true );

	list = new gui::ListView(this, window::default_left_margin, 105,
							 w()- window::default_left_margin- window::default_right_margin, h() - 105 - 50);
	list->setMaxElements(calllog::settings::pageSize);
	list->setPageSize(calllog::settings::pageSize);
	list->setPenFocusWidth(0);
	list->setPenWidth(0);
	list->setProvider(calllogModel);

	setFocusItem( list );
}
void CallLogMainWindow::destroyInterface() {
	AppWindow::destroyInterface();

	if( list ) { removeWidget(list); delete list; list= nullptr; };

	children.clear();
	delete calllogModel;
}

CallLogMainWindow::~CallLogMainWindow() {
	destroyInterface();
}

void CallLogMainWindow::onBeforeShow( ShowMode mode, SwitchData* data ) {
	if( mode == ShowMode::GUI_SHOW_INIT ){ 
		calllogModel->clear();
		calllogModel->requestRecordsCount();
		list->clear();
		list->setElementsCount( calllogModel->getItemCount() );
	}
}

bool CallLogMainWindow::onInput( const InputEvent& inputEvent ) {
	//process only if key is released
	if(( inputEvent.state != InputEvent::State::keyReleasedShort ) || ( inputEvent.state != InputEvent::State::keyReleasedLong )) {
		if( inputEvent.keyCode == KeyCode::KEY_LF ) {
			// TODO: alek: add calling
			return true;
		}
	}

	return AppWindow::onInput( inputEvent );
}

bool CallLogMainWindow::onDatabaseMessage( sys::Message* msgl ) {
	DBCalllogResponseMessage* msg = reinterpret_cast<DBCalllogResponseMessage*>( msgl );
	if( calllogModel->updateRecords( std::move(msg->records), msg->offset, msg->limit, msg->count ) ) {
		return true;
	}

	return false;
}

} /* namespace gui */
