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

#include "service-db/messages/DBMessage.hpp"
#include "i18/i18.hpp"

#include "Label.hpp"
#include "ListView.hpp"
#include "Margins.hpp"
#include "MessagesMainWindow.hpp"
#include "../MessagesStyle.hpp"

#include "service-db/api/DBServiceAPI.hpp"
#include "service-cellular/api/CellularServiceAPI.hpp"

#include <log/log.hpp>
#include <Style.hpp>

namespace gui {

MessagesMainWindow::MessagesMainWindow(app::Application *app) :
		AppWindow(app, "MainWindow"), threadModel { new ThreadModel(app) } {
	setSize(style::window_width, style::window_height);
	buildInterface();

}

void MessagesMainWindow::rebuild() {
	destroyInterface();
	buildInterface();
}
void MessagesMainWindow::buildInterface() {

	using namespace messages;

	AppWindow::buildInterface();

	list = new gui::ListView(this, threads::listPositionX,
			threads::ListPositionY, threads::listWidth, threads::listWidth);
	list->setMaxElements(threads::pageSize);
	list->setPageSize(threads::pageSize);
	list->setPenFocusWidth(0);
	list->setPenWidth(0);
	list->setProvider(threadModel);

	bottomBar->setActive(BottomBar::Side::LEFT, true);

	bottomBar->setActive(BottomBar::Side::CENTER, true);
	bottomBar->setActive(BottomBar::Side::RIGHT, true);
	bottomBar->setText(BottomBar::Side::LEFT,
			utils::localize.get("common_options"));
	bottomBar->setText(BottomBar::Side::CENTER,
			utils::localize.get("common_open"));
	bottomBar->setText(BottomBar::Side::RIGHT,
			utils::localize.get("common_back"));

	topBar->setActive(TopBar::Elements::TIME, true);

	setTitle(utils::localize.get("app_messages_title_main"));

	leftArrowImage = new gui::Image(this, 30, 62, 0, 0, "arrow_left");
	rightArrowImage = new gui::Image(this, 480 - 30 - 13, 62, 0, 0,
			"arrow_right");
	newMessageImage = new gui::Image(this, 48, 55, 0, 0, "cross");
	searchImage = new gui::Image(this, 480 - 48 - 26, 55, 0, 0, "search");

//	setFocusItem( list );

}
void MessagesMainWindow::destroyInterface() {
	AppWindow::destroyInterface();
	if (list) {
		removeWidget(list);
		delete list;
		list = nullptr;
	}
	if (leftArrowImage) {
		removeWidget(leftArrowImage);
		delete leftArrowImage;
		leftArrowImage = nullptr;
	}
	if (rightArrowImage) {
		removeWidget(rightArrowImage);
		delete rightArrowImage;
		rightArrowImage = nullptr;
	}
	if (newMessageImage) {
		removeWidget(newMessageImage);
		delete newMessageImage;
		newMessageImage = nullptr;
	}
	if (searchImage) {
		removeWidget(searchImage);
		delete searchImage;
		searchImage = nullptr;
	}

	children.clear();
	delete threadModel;
}

MessagesMainWindow::~MessagesMainWindow() {
	destroyInterface();
}

void MessagesMainWindow::onBeforeShow(ShowMode mode, SwitchData *data) {

	if (mode == ShowMode::GUI_SHOW_INIT) {
		threadModel->clear();
		threadModel->requestRecordsCount();
		list->clear();
		list->setElementsCount(threadModel->getItemCount());

		setFocusItem(list);
	}

}

bool MessagesMainWindow::onInput(const InputEvent &inputEvent) {
	//check if any of the lower inheritance onInput methods catch the event
	return AppWindow::onInput(inputEvent);
}

bool MessagesMainWindow::onDatabaseMessage(sys::Message *msgl) {
	DBThreadResponseMessage *msg =
			reinterpret_cast<DBThreadResponseMessage*>(msgl);
	if (threadModel->updateRecords(std::move(msg->records), msg->offset,
			msg->limit, msg->count))
		return true;

	return false;
}

} /* namespace gui */
