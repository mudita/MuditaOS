/*
 * @file SettingsMainWindow.cpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 8 lip 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#include <memory>
#include <functional>

#include "service-appmgr/ApplicationManager.hpp"

#include "../ApplicationSettings.hpp"


#include "i18/i18.hpp"

#include "Label.hpp"
#include "Margins.hpp"
#include "SettingsMainWindow.hpp"
#include <Style.hpp>
#include "Info.hpp"

namespace gui {

SettingsMainWindow::SettingsMainWindow( app::Application* app ) : AppWindow(app,"MainWindow"){
	setSize( 480, 600 );

	buildInterface();
}

void SettingsMainWindow::rebuild() {
	destroyInterface();
	buildInterface();
}

void SettingsMainWindow::buildInterface()
{
    AppWindow::buildInterface();
    bottomBar->setActive(BottomBar::Side::CENTER, true);
    bottomBar->setActive(BottomBar::Side::RIGHT, true);
    bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get("common_open"));
    bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get("common_back"));

    topBar->setActive(TopBar::Elements::SIGNAL, true);
    topBar->setActive(TopBar::Elements::BATTERY, true);

    setTitle(utils::localize.get("app_settings_title_main"));

    body = new gui::PageLayout(this, {0, title->offset_h(), this->getWidth(), this->getHeight() - this->title->offset_h() - bottomBar->getHeight()});

    addOptionLabel("Info", [=](gui::Item &item) {
        LOG_INFO("switching to TEST UI page");
        application->switchWindow(gui::window::hw_info, nullptr);
        return true;
    });

    addOptionLabel("UI TEST", [=](gui::Item &item) {
        LOG_INFO("switching to TEST UI page");
        application->switchWindow("TEST_UI", nullptr);
        return true;
    });

    addOptionLabel(utils::localize.get("app_settings_bt"), [=](gui::Item &item) {
        LOG_INFO("switching to bluetooth page");
        application->switchWindow("Bluetooth", nullptr);
        return true;
    });

    addOptionLabel(utils::localize.get("app_settings_language"), [=](gui::Item &) {
        LOG_INFO("switching to language page");
        application->switchWindow("Languages");
        return true;
    });

    addOptionLabel("[None] " + utils::localize.get("app_settings_date_and_time"), [=](gui::Item &) { return true; });
    addOptionLabel("[None] " + utils::localize.get("app_settings_display"), [=](gui::Item &) { return true; });
    addOptionLabel("[None] " + utils::localize.get("app_settings_phone_modes"), [=](gui::Item &) { return true; });
    addOptionLabel("[None] " + utils::localize.get("app_settings_security"), [=](gui::Item &) { return true; });
    addOptionLabel("[None] " + utils::localize.get("app_settings_about"), [=](gui::Item &) { return true; });
    addOptionLabel("[Dummy]", [=](gui::Item &) { return true; });
    addOptionLabel("[Dummy]", [=](gui::Item &) { return true; });
    addOptionLabel("[Dummy]", [=](gui::Item &) { return true; });
    addOptionLabel("[Dummy]", [=](gui::Item &) { return true; });
    addOptionLabel("[Dummy]", [=](gui::Item &) { return true; });

    setFocusItem(body);
    body->switchPage(0);
}
void SettingsMainWindow::destroyInterface() {
	AppWindow::destroyInterface();
	this->focusItem = nullptr;
	children.clear();
}

SettingsMainWindow::~SettingsMainWindow() {
	destroyInterface();
}

void SettingsMainWindow::addOptionLabel(const std::string &text, std::function<bool(Item &)> activatedCallback)
{
    gui::Label *label = new gui::Label(nullptr, 20, 0, 480 - 2 * 20, 55, text);
    style::window::decorateOption(label);
    label->activatedCallback = activatedCallback;
    new gui::Image(label, 425 - 17, 24, 0, 0, "right_label_arrow");
    body->addWidget(label);
}

void SettingsMainWindow::onBeforeShow( ShowMode mode, SwitchData* data ) {
}


} /* namespace gui */
