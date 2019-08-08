#include "PhonebookMainWindow.hpp"

#include <functional>
#include <memory>

#include "service-appmgr/ApplicationManager.hpp"

#include "../ApplicationPhonebook.hpp"

#include "i18/i18.hpp"

#include "Label.hpp"
#include "Margins.hpp"
#include "PhonebookMainWindow.hpp"

#include "service-db/api/DBServiceAPI.hpp"

namespace gui {

PhonebookMainWindow::PhonebookMainWindow(app::Application *app) : AppWindow(app, "MainWindow") {
    setSize(480, 600);
    buildInterface();
}

void PhonebookMainWindow::rebuild() {
    // find which widget has focus
    uint32_t index = 0;
    for (uint32_t i = 0; i < options.size(); i++)
        if (options[i] == getFocusItem()) {
            index = i;
            break;
        }

    destroyInterface();
    buildInterface();
    setFocusItem(options[index]);
}
void PhonebookMainWindow::buildInterface() {
    AppWindow::buildInterface();
    bottomBar->setActive(BottomBar::Side::CENTER, true);
    bottomBar->setActive(BottomBar::Side::RIGHT, true);
    bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get("common_open"));
    bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get("common_back"));

    topBar->setActive(TopBar::Elements::SIGNAL, true);
    topBar->setActive(TopBar::Elements::BATTERY, true);

    title = new gui::Label(this, 0, 50, 480, 50);
    title->setFilled(false);
    title->setBorderColor(gui::ColorNoColor);
    title->setFont("gt_pressura_bold_24");
    title->setText(utils::localize.get("app_phonebook_title_main"));
    title->setAlignement(gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_CENTER, gui::Alignment::ALIGN_VERTICAL_CENTER));

    std::unique_ptr<std::vector<ContactRecord>> tmp = DBServiceAPI::ContactGetLimitOffset(this->application, 0,7);
    LOG_INFO("CONTACTS: %d", tmp->size());
    if(tmp != nullptr && tmp->size() != 0) {
        for(auto a:*tmp) {
            options.push_back(addOptionLabel(a.primaryName, [=](gui::Item &) { return true; }));
        }
    }

    if( options.size() == 0) {
            options.push_back(addOptionLabel(utils::localize.get("app_phonebook_empty"), [=](gui::Item &) { return true; }));
    }

    // set position and navigation for labels
    for (uint32_t i = 0, size = options.size(), posY = 100; i < options.size(); i++) {
        options[i]->setPosition(17, posY);
        posY += 60;
        options[i]->setNavigationItem(NavigationDirection::DOWN, options[(i + 1) % size]);
        options[i]->setNavigationItem(NavigationDirection::UP, options[(size + i - 1) % size]);
    }
}
void PhonebookMainWindow::destroyInterface() {
    AppWindow::destroyInterface();
    delete title;
    for (uint32_t i = 0; i < options.size(); i++)
        delete options[i];
    options.clear();
    this->focusItem = nullptr;
    LOG_INFO("options size: %d", options.size());
    children.clear();
}

PhonebookMainWindow::~PhonebookMainWindow() { destroyInterface(); }

gui::Item *PhonebookMainWindow::addOptionLabel(const UTF8 &text, std::function<bool(Item &)> activatedCallback) {
    gui::Label *label = new gui::Label(this, 17, 0, 480 - 34, 60, text);
    label->setMargins(gui::Margins(16, 0, 0, 0));
    label->setFilled(false);
    label->setPenFocusWidth(3);
    label->setPenWidth(0);
    label->setFont("gt_pressura_regular_24");
    label->setAlignement(gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_LEFT, gui::Alignment::ALIGN_VERTICAL_CENTER));
    label->setRadius(11);
    label->activatedCallback = activatedCallback;

    return label;
}

void PhonebookMainWindow::onBeforeShow(ShowMode mode, uint32_t command, SwitchData *data) { setFocusItem(options[0]); }

bool PhonebookMainWindow::onInput(const InputEvent &inputEvent) {
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

    if (inputEvent.keyCode == KeyCode::KEY_ENTER) {
        LOG_INFO("Enter pressed");
    } else if (inputEvent.keyCode == KeyCode::KEY_RF) {
        sapm::ApplicationManager::messageSwitchApplication(application, "ApplicationDesktop", "MenuWindow", nullptr);
        return true;
    }

    return false;
}

} /* namespace gui */
