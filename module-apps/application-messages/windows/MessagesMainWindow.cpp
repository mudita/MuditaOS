/*
 * @file MessagesMainWindow.cpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 25 wrz 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#include <functional>
#include <memory>

#include "service-appmgr/ApplicationManager.hpp"

//#include "../ApplicationPhonebook.hpp"
#include "service-db/messages/DBMessage.hpp"
#include "i18/i18.hpp"

#include "Label.hpp"
#include "ListView.hpp"
#include "Margins.hpp"
#include "MessagesMainWindow.hpp"

#include "service-db/api/DBServiceAPI.hpp"

#include <log/log.hpp>
#include <Style.hpp>

namespace gui {

MessagesMainWindow::MessagesMainWindow(app::Application *app) :
	AppWindow(app, "MainWindow")
//	phonebookModel{ new PhonebookModel(app)}
{
    setSize(480, 600);
    buildInterface();

}

void MessagesMainWindow::rebuild() {
    destroyInterface();
    buildInterface();
}
void MessagesMainWindow::buildInterface() {

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

    leftArrowImage  = new gui::Image( this, 30,62,0,0, "arrow_left" );
	rightArrowImage = new gui::Image( this, 480-30-13,62,0,0, "arrow_right" );
	newMessageImage = new gui::Image( this, 48,55,0,0, "cross" );
	searchImage     = new gui::Image( this, 480-48-26,55,0,0, "search" );
}
void MessagesMainWindow::destroyInterface() {
    AppWindow::destroyInterface();
//    if( list ) { removeWidget(list);    delete list; list = nullptr; }
    if( leftArrowImage ) { removeWidget(leftArrowImage);    delete leftArrowImage; leftArrowImage = nullptr; }
    if( rightArrowImage ) { removeWidget(rightArrowImage);    delete rightArrowImage; rightArrowImage = nullptr; }
    if( newMessageImage ) { removeWidget(newMessageImage);    delete newMessageImage; newMessageImage = nullptr; }
    if( searchImage ) { removeWidget(searchImage);    delete searchImage; searchImage = nullptr; }

    children.clear();
//    delete phonebookModel;
}

MessagesMainWindow::~MessagesMainWindow() {
	destroyInterface();
}


void MessagesMainWindow::onBeforeShow(ShowMode mode, SwitchData *data) {
//	setFocusItem(list);

//	phonebookModel->clear();
//	phonebookModel->requestRecordsCount();

//	list->clear();
//	list->setElementsCount( phonebookModel->getItemCount() );
}

bool MessagesMainWindow::onInput(const InputEvent &inputEvent) {
	//check if any of the lower inheritance onInput methods catch the event
	return AppWindow::onInput( inputEvent );
}

bool MessagesMainWindow::onDatabaseMessage( sys::Message* msgl ) {
//	DBContactResponseMessage* msg = reinterpret_cast<DBContactResponseMessage*>( msgl );
//	if( phonebookModel->updateRecords( std::move(msg->records), msg->offset, msg->limit, msg->count, msg->favourite ) )
//		return true;

	return false;
}

} /* namespace gui */
