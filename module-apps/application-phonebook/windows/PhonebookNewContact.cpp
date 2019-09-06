#include "PhonebookNewContact.hpp"

#include "service-appmgr/ApplicationManager.hpp"
#include "../ApplicationPhonebook.hpp"
#include "i18/i18.hpp"
#include "Label.hpp"
#include "Margins.hpp"
#include "PhonebookNewContact.hpp"
#include <log/log.hpp>

#include "service-db/api/DBServiceAPI.hpp"

namespace gui {

PhonebookNewContact::PhonebookNewContact(app::Application *app) :
	AppWindow(app, "NewContact")
{
    setSize(480, 600);
    buildInterface();
}

void PhonebookNewContact::rebuild()
{
    destroyInterface();
    buildInterface();
}

void PhonebookNewContact::buildInterface() {
    AppWindow::buildInterface();

    bottomBar->setActive(BottomBar::Side::LEFT, false);
	bottomBar->setActive(BottomBar::Side::CENTER, true);
	bottomBar->setActive(BottomBar::Side::RIGHT, true);
	bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get("app_phonebook_save"));
	bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get("app_phonebook_back"));

	topBar->setActive(TopBar::Elements::TIME, true);

	title = new gui::Label(this, 0, 50, 480, 54);
	title->setFilled(false);
	title->setBorderColor( gui::ColorFullBlack );
	title->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM );
	title->setMargins( Margins(0,0,0,18));
	title->setFont("gt_pressura_bold_24");
	title->setText(utils::localize.get("app_phonebook_contact_title"));
	title->setAlignement(gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_CENTER, gui::Alignment::ALIGN_VERTICAL_BOTTOM));

}

void PhonebookNewContact::destroyInterface()
{
    AppWindow::destroyInterface();

    if( title ) { removeWidget(title); delete title; title = nullptr; }
    children.clear();
}

PhonebookNewContact::~PhonebookNewContact()
{
    destroyInterface();
}

void PhonebookNewContact::onBeforeShow(ShowMode mode, uint32_t command, SwitchData *data) {
}

bool PhonebookNewContact::onInput(const InputEvent &inputEvent)
{
    // check if any of the lower inheritance onInput methods catch the event
    bool ret = AppWindow::onInput(inputEvent);

    // process only if key is released
    if ((inputEvent.state != InputEvent::State::keyReleasedShort) && ((inputEvent.state != InputEvent::State::keyReleasedLong)))
        return false;

    if (inputEvent.keyCode == KeyCode::KEY_RF) {
    	LOG_INFO("going to list of contacts");
		application->switchWindow("MainWindow",0, nullptr );
        return true;
    }

    return ret;
}
}
