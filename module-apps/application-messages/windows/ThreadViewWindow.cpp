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
#include <Style.hpp>

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

    setTitle(utils::localize.get("app_messages_title_main"));
}
void ThreadViewWindow::destroyInterface() {
    AppWindow::destroyInterface();
    children.clear();
}

ThreadViewWindow::~ThreadViewWindow() {
	destroyInterface();
}


void ThreadViewWindow::onBeforeShow(ShowMode mode, SwitchData *data) {
}

bool ThreadViewWindow::onInput(const InputEvent &inputEvent) {
	return AppWindow::onInput( inputEvent );
}

bool ThreadViewWindow::onDatabaseMessage( sys::Message* msgl ) {
//	DBContactResponseMessage* msg = reinterpret_cast<DBContactResponseMessage*>( msgl );
//	if( phonebookModel->updateRecords( std::move(msg->records), msg->offset, msg->limit, msg->count, msg->favourite ) )
//		return true;

	return false;
}

} /* namespace gui */
