// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "TorchWindow.hpp"
#include "application-settings-new/ApplicationSettings.hpp"
#include "application-settings-new/widgets/SettingsStyle.hpp"

#include "OptionSetting.hpp"

#include <i18n/i18n.hpp>

namespace gui
{

    TorchWindow::TorchWindow(app::Application *app) : BaseSettingsWindow(app, gui::window::name::torch)
    {
        buildInterface();
    }

    void TorchWindow::buildInterface()
    {
        setTitle(utils::translateI18("app_settings_title_torch"));
        optionsList->setSize(optionsList->getWidth(),
                             optionsList->getHeight() - style::settings::window::torch::body_offset);
        bar = new Rect(this,
                       style::window::default_left_margin,
                       style::settings::window::torch::bar_y,
                       style::window::default_body_width,
                       style::settings::window::torch::bar_h);
        bar->setVisible(false);
        descriptionText = new Text(this,
                                   style::window::default_left_margin,
                                   style::settings::window::torch::description_y,
                                   style::window::default_body_width,
                                   style::settings::window::torch::description_h);
        descriptionText->setFont(style::window::font::medium);
        descriptionText->setText(utils::translateI18("app_settings_torch_description"));
        descriptionText->setVisible(false);
    }

    auto TorchWindow::buildOptionsList() -> std::list<gui::Option>
    {
        std::list<gui::Option> optionsList;

        optionsList.emplace_back(std::make_unique<gui::option::OptionSettings>(
            utils::translateI18("app_settings_torch_sunset_red_light_option"),
            [=](gui::Item &item) {
                toggleSwitchState();
                return true;
            },
            [=](gui::Item &item) {
                if (item.focus) {
                    this->setBottomBarText(utils::translateI18(style::strings::common::Switch),
                                           BottomBar::Side::CENTER);
                }
                return true;
            },
            this,
            switchState ? gui::option::SettingRightItem::On : gui::option::SettingRightItem::Off));

        if (switchState) {
            optionsList.emplace_back(std::make_unique<gui::option::OptionSettings>(
                utils::translateI18("app_settings_torch_nightshift_time_option"),
                [=](gui::Item &item) {
                    application->switchWindow(gui::window::name::nightshift, gui::ShowMode::GUI_SHOW_INIT);
                    return true;
                },
                [=](gui::Item &item) {
                    if (item.focus) {
                        this->setBottomBarText(utils::translateI18(style::strings::common::select),
                                               BottomBar::Side::CENTER);
                    }
                    return true;
                },
                nullptr,
                gui::option::SettingRightItem::ArrowWhite));
        }

        return optionsList;
    }

    void TorchWindow::toggleSwitchState()
    {
        switchState = !switchState;
        rebuildOptionList();
        bar->setVisible(switchState);
        descriptionText->setVisible(switchState);
    }

} // namespace gui
