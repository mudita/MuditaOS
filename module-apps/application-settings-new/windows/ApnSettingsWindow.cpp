// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ApnSettingsWindow.hpp"
#include "application-settings-new/ApplicationSettings.hpp"
#include "application-settings-new/widgets/SettingsStyle.hpp"
#include "application-settings-new/data/SettingsItemData.hpp"
#include "OptionSetting.hpp"

#include <InputEvent.hpp>

namespace gui
{

    ApnSettingsWindow::ApnSettingsWindow(app::Application *app) : OptionWindow(app, gui::window::name::apn_settings)
    {
        buildInterface();
    }

    auto ApnSettingsWindow::onInput(const InputEvent &inputEvent) -> bool
    {
        if (inputEvent.isShortPress()) {
            if (inputEvent.is(KeyCode::KEY_LEFT)) {
                auto apnRecord                        = std::make_shared<packet_data::APN::Config>();
                std::unique_ptr<gui::SwitchData> data = std::make_unique<ApnItemData>(apnRecord);
                application->switchWindow(gui::window::name::new_apn, gui::ShowMode::GUI_SHOW_INIT, std::move(data));
                return true;
            }
        }
        return AppWindow::onInput(inputEvent);
    }

    void ApnSettingsWindow::buildInterface()
    {
        setTitle(utils::localize.get("app_settings_network_apn_settings"));

        topBar->setActive(TopBar::Elements::SIGNAL, false);
        topBar->setActive(TopBar::Elements::BATTERY, false);
        topBar->setActive(TopBar::Elements::SIM, false);

        leftArrowImage = new gui::Image(this,
                                        style::settings::window::leftArrowImage::x,
                                        style::settings::window::leftArrowImage::y,
                                        style::settings::window::leftArrowImage::w,
                                        style::settings::window::leftArrowImage::h,
                                        "arrow_left");
        crossImage     = new gui::Image(this,
                                    style::settings::window::crossImage::x,
                                    style::settings::window::crossImage::y,
                                    style::settings::window::crossImage::w,
                                    style::settings::window::crossImage::h,
                                    "cross");
        emptyListIcon  = new Icon(this,
                                 0,
                                 style::header::height,
                                 style::window_width,
                                 style::window_height - style::header::height - style::footer::height,
                                 "phonebook_empty_grey_circle_W_G",
                                 utils::localize.get("app_settings_apn_settings_no_apns"));

        bottomBar->setActive(gui::BottomBar::Side::CENTER, false);
    }

} // namespace gui
