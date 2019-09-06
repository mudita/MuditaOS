#include "PhonebookMainWindow.hpp"

#include <functional>
#include <memory>

#include "service-appmgr/ApplicationManager.hpp"

#include "../ApplicationPhonebook.hpp"

#include "i18/i18.hpp"

#include "Label.hpp"
#include "ListView.hpp"
#include "Margins.hpp"
#include "PhonebookMainWindow.hpp"

#include "service-db/api/DBServiceAPI.hpp"

#include <log/log.hpp>

namespace gui {

PhonebookMainWindow::PhonebookMainWindow(app::Application *app) : AppWindow(app, "MainWindow") {
    setSize(480, 600);
    buildInterface();
}

void PhonebookMainWindow::rebuild() {
    destroyInterface();
    buildInterface();
}
void PhonebookMainWindow::buildInterface() {

	AppWindow::buildInterface();

	bottomBar->setActive(BottomBar::Side::LEFT, true);
    bottomBar->setActive(BottomBar::Side::CENTER, true);
    bottomBar->setActive(BottomBar::Side::RIGHT, true);
    bottomBar->setText(BottomBar::Side::LEFT, utils::localize.get("app_phonebook_call"));
    bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get("app_phonebook_open"));
    bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get("app_phonebook_back"));

    topBar->setActive(TopBar::Elements::TIME, true);

    title = new gui::Label(this, 0, 50, 480, 54);
    title->setFilled(false);
    title->setBorderColor( gui::ColorFullBlack );
    title->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM );
    title->setMargins( Margins(0,0,0,18));
    title->setFont("gt_pressura_bold_24");
    title->setText(utils::localize.get("app_phonebook_title_main"));
    title->setAlignement(gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_CENTER, gui::Alignment::ALIGN_VERTICAL_BOTTOM));

    list = new gui::ListView(this, 11, 105, 480-22, 600-105-50 );

}
void PhonebookMainWindow::destroyInterface() {
    AppWindow::destroyInterface();
    if( title ) { removeWidget(title);    delete title; title = nullptr; }
    if( list ) { removeWidget(list);    delete list; list = nullptr; }
    children.clear();
}

PhonebookMainWindow::~PhonebookMainWindow() {
	destroyInterface();
}


void PhonebookMainWindow::onBeforeShow(ShowMode mode, uint32_t command, SwitchData *data) {
}

bool PhonebookMainWindow::onInput(const InputEvent &inputEvent) {
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

    if( inputEvent.keyCode == KeyCode::KEY_LEFT) {
        LOG_INFO("Adding new contact");
        application->switchWindow("NewContact",0,nullptr);
    }
    else if( inputEvent.keyCode == KeyCode::KEY_ENTER ) {
		LOG_INFO("Searching contact");
		application->switchWindow("SearchWindow",0, nullptr );
	}
	else if( inputEvent.keyCode == KeyCode::KEY_RF ) {
		sapm::ApplicationManager::messageSwitchApplication( application, "ApplicationDesktop", "MenuWindow", nullptr );
		return true;
	}

	return false;

}

} /* namespace gui */
