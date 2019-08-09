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

void PhonebookNewContact::Input::build(PhonebookNewContact *ptr,const UTF8 &text)
{
    gui::Label *el = new gui::Label();
    el->setMaxSize(480, 50);
    el->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
    el->setText(text);
    el->setFont("gt_pressura_bold_16");
    ptr->box->addWidget(el);

    el = new gui::Label();
    el->setMaxSize(480, 50);
    el->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM);
    el->setPenWidth(5);
    el->setFont("gt_pressura_regular_16");
    ptr->box->addWidget(el);

}

PhonebookNewContact::PhonebookNewContact(app::Application *app) : AppWindow(app, "NewContact")
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
    topBar->setActive(TopBar::Elements::SIGNAL, true);
    topBar->setActive(TopBar::Elements::BATTERY, true);

    bottomBar->setActive(BottomBar::Side::CENTER, true);
    bottomBar->setActive(BottomBar::Side::RIGHT, true);
    bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get("common_open"));
    bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get("common_back"));

    title = std::make_unique<gui::Label>(this, 0, 50, 480, 50);
    title->setFilled(false);
    title->setBorderColor(gui::ColorNoColor);
    title->setFont("gt_pressura_bold_24");
    title->setText(utils::localize.get("app_phonebook_title_add_contact"));
    title->setAlignement(gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_CENTER, gui::Alignment::ALIGN_VERTICAL_CENTER));

    box = std::make_unique<gui::VBox>(this, 0, 150, 480, 450);
    box->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
    name1.build(this, utils::localize.get("app_phonebook_first_name"));
    // , name2, nr1, nr2, email

}
void PhonebookNewContact::destroyInterface()
{
    AppWindow::destroyInterface();
    children.clear();
}

PhonebookNewContact::~PhonebookNewContact()
{
    destroyInterface();
}


void PhonebookNewContact::onBeforeShow(ShowMode mode, uint32_t command, SwitchData *data)
{
}

bool PhonebookNewContact::onInput(const InputEvent &inputEvent) {
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

}
