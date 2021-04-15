// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "PhoneModesWindow.hpp"
#include "application-settings-new/ApplicationSettings.hpp"

#include "OptionSetting.hpp"

#include <i18n/i18n.hpp>

namespace gui
{
    PhoneModesWindow::PhoneModesWindow(app::Application *app,
                                       app::settingsInterface::SimParams *simParams,
                                       app::settingsInterface::OperatorsSettings *operatorsSettings)
        : OptionWindow(app, gui::window::name::phone_modes), simParams(simParams), operatorsSettings(operatorsSettings)
    {
        addOptions(modesOptList());
    }

    auto PhoneModesWindow::modesOptList() -> std::list<gui::Option>
    {
        std::list<gui::Option> optList;

        optList.emplace_back(std::make_unique<gui::option::OptionSettings>(
            utils::translateI18("app_settings_connected"),
            [=](gui::Item &item) { return true; },
            [=](gui::Item &item) {
                if (item.focus) {
                    this->clearBottomBarText(BottomBar::Side::CENTER);
                }
                return true;
            },
            this));

        optList.emplace_back(std::make_unique<gui::option::OptionSettings>(
            utils::translateI18("app_settings_title_do_not_disturb"),
            [=](gui::Item &item) {
                this->application->switchWindow(gui::window::name::do_not_disturb, nullptr);
                return true;
            },
            [=](gui::Item &item) {
                if (item.focus) {
                    this->setBottomBarText(utils::localize.get(style::strings::common::adjust),
                                           BottomBar::Side::CENTER);
                }
                return true;
            },
            this,
            gui::option::SettingRightItem::ArrowWhite));

        optList.emplace_back(std::make_unique<gui::option::OptionSettings>(
            utils::translateI18("app_settings_title_offline"),
            [=](gui::Item &item) {
                this->application->switchWindow(gui::window::name::offline, nullptr);
                return true;
            },
            [=](gui::Item &item) {
                if (item.focus) {
                    this->setBottomBarText(utils::localize.get(style::strings::common::adjust),
                                           BottomBar::Side::CENTER);
                }
                return true;
            },
            this,
            gui::option::SettingRightItem::ArrowWhite));

        bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get(style::strings::common::back));

        return optList;
    }
} // namespace gui
