/*
 * @file ThreadViewWindow.cpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 25 wrz 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#include <functional>
#include <memory>

#include "service-appmgr/ApplicationManager.hpp"

#include "i18/i18.hpp"

#include "Label.hpp"
#include "ListView.hpp"
#include "Margins.hpp"

#include "service-db/messages/DBMessage.hpp"
#include "service-db/api/DBServiceAPI.hpp"

#include <log/log.hpp>

#include "ThreadViewWindow.hpp"

namespace gui {

ThreadViewWindow::ThreadViewWindow(app::Application *app) :
	AppWindow(app, "ThreadViewWindow")
//	phonebookModel{ new PhonebookModel(app)}
{
    setSize(480, 600);
    buildInterface();

}

void ThreadViewWindow::rebuild() {
    destroyInterface();
    buildInterface();
}
void ThreadViewWindow::buildInterface() {

	AppWindow::buildInterface();

//	list = new gui::PhonebookListView(this, 11, 105, 480-22, 600-105-50 );
//	list->setMaxElements(7);
//	list->setPageSize(7);
//	list->setPenFocusWidth(0);
//	list->setPenWidth(0);
//	list->setProvider( phonebookModel );
//	list->setApplication( application );

	bottomBar->setActive(BottomBar::Side::LEFT, true);
    bottomBar->setActive(BottomBar::Side::CENTER, true);
    bottomBar->setActive(BottomBar::Side::RIGHT, true);
    bottomBar->setText(BottomBar::Side::LEFT, utils::localize.get("common_options"));
    bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get("common_open"));
    bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get("common_back"));

    topBar->setActive(TopBar::Elements::TIME, true);

    title = new gui::Label(this, 0, 50, 480, 54);
    title->setFilled(false);
    title->setBorderColor( gui::ColorFullBlack );
    title->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM );
    title->setMargins( Margins(0,0,0,18));
    title->setFont("gt_pressura_bold_24");
    title->setText(utils::localize.get("app_messages_title_main"));
    title->setAlignement(gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_CENTER, gui::Alignment::ALIGN_VERTICAL_BOTTOM));
}
void ThreadViewWindow::destroyInterface() {
    AppWindow::destroyInterface();
    if( title ) { removeWidget(title);    delete title; title = nullptr; }

    children.clear();
}

ThreadViewWindow::~ThreadViewWindow() {
	destroyInterface();
}


void ThreadViewWindow::onBeforeShow(ShowMode mode, SwitchData *data) {
}

bool ThreadViewWindow::onInput(const InputEvent &inputEvent) {
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
		application->switchWindow( "MainWindow" );
		return true;
	}

	return false;

}

bool ThreadViewWindow::onDatabaseMessage( sys::Message* msgl ) {
//	DBContactResponseMessage* msg = reinterpret_cast<DBContactResponseMessage*>( msgl );
//	if( phonebookModel->updateRecords( std::move(msg->records), msg->offset, msg->limit, msg->count, msg->favourite ) )
//		return true;

	return false;
}

} /* namespace gui */
