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

PhonebookNewContact::Input::Input(gui::VBox* box, const UTF8 &text)
{
    const unsigned int wall_offset = 10;
    if (!box) {
        return;
    }
    name.setMaxSize(box->getMaxWidth() - wall_offset * 2, 40);
    name.setPosition(wall_offset, 0);
    name.setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
    name.setText(text);
    name.setFont("gt_pressura_bold_16");
    name.setPenFocusWidth(3);
    name.setPenWidth(1);
    box->addWidget(&name);

    input.setMaxSize(box->getMaxWidth() - wall_offset * 2, 40);
    input.setPosition(wall_offset, 0);
    input.setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM);
    input.setPenWidth(3);
    input.setFont("gt_pressura_regular_16");
    input.setPenFocusWidth(3);
    input.setPenWidth(1);
    box->addWidget(&input);
}

PhonebookNewContact::Input::~Input()
{
    LOG_INFO("Input destroyed?");
}

PhonebookNewContact::PhonebookNewContact(app::Application *app) : AppWindow(app, "NewContact"), title(nullptr), box(nullptr)
{
    setSize(480, 600);
    buildInterface();
}

void PhonebookNewContact::rebuild()
{
    destroyInterface();
    buildInterface();
    for ( auto l : el) {
        if (getFocusItem() == &l->input) {
            LOG_INFO("Focus changed to: %s", l->name.getText());
            setFocusItem(&l->input);
            break;
        }
    }
}

void PhonebookNewContact::buildInterface() {
    AppWindow::buildInterface();
    topBar->setActive(TopBar::Elements::SIGNAL, true);
    topBar->setActive(TopBar::Elements::BATTERY, true);

    bottomBar->setActive(BottomBar::Side::CENTER, true);
    bottomBar->setActive(BottomBar::Side::RIGHT, true);
    bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get("common_open"));
    bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get("common_back"));

    title = new gui::Label(this, 0, 30, 480, 50);
    title->setFilled(false);
    title->setBorderColor(gui::ColorNoColor);
    title->setFont("gt_pressura_bold_24");
    title->setText(utils::localize.get("app_phonebook_title_add_contact"));
    title->setAlignement(gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_CENTER, gui::Alignment::ALIGN_VERTICAL_CENTER));

    if(!box) {
        box = new gui::VBox(this, 10, 100, 480, 500);
        box->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        el.push_back(new Input(box, utils::localize.get("app_phonebook_first_name")));
        el.push_back(new Input(box, utils::localize.get("app_phonebook_number")));
        el.push_back(new Input(box, utils::localize.get("app_phonebook_second_number")));
        el.push_back(new Input(box, utils::localize.get("app_phonebook_email")));

        for (unsigned int i =0; i < el.size(); ++i) {
            el[i]->input.setNavigationItem(NavigationDirection::DOWN,   &el[i+1==el.size()?i:i+1]->input);
            el[i]->input.setNavigationItem(NavigationDirection::UP,     &el[i==0?0:i-1]->input);
        }
    }
}

void PhonebookNewContact::destroyInterface()
{
    AppWindow::destroyInterface();
    children.clear();
}

PhonebookNewContact::~PhonebookNewContact()
{
    for( auto p : el) {
        delete p;
    }
    destroyInterface();
}

void PhonebookNewContact::onBeforeShow(ShowMode mode, uint32_t command, SwitchData *data) {
    setFocusItem(&el[0]->input);
}

bool PhonebookNewContact::onInput(const InputEvent &inputEvent)
{
    // check if any of the lower inheritance onInput methods catch the event
    bool ret = AppWindow::onInput(inputEvent);
    if (ret) {
        // refresh window only when key is other than enter
        if (inputEvent.keyCode != KeyCode::KEY_ENTER)
            application->render(RefreshModes::GUI_REFRESH_FAST);
        return true;
    }

    // process only if key is released
    if ((inputEvent.state != InputEvent::State::keyReleasedShort) && ((inputEvent.state != InputEvent::State::keyReleasedLong)))
        return false;

    LOG_INFO("Key pressed %d", inputEvent.keyCode);
    if (inputEvent.keyCode == KeyCode::KEY_RF) {
        sapm::ApplicationManager::messageSwitchPreviousApplication(application);
        return true;
    }

    return false;
}
}
