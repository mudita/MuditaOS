// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "PhoneModesWindow.hpp"

#include <application-settings/ApplicationSettings.hpp>
#include <application-settings/windows/WindowNames.hpp>

#include <OptionSetting.hpp>

namespace gui
{
    PhoneModesWindow::PhoneModesWindow(app::ApplicationCommon *app,
                                       app::settingsInterface::PhoneModeSettings *phoneModeSettings)
        : OptionWindow(app, gui::window::name::phone_modes), phoneModeSettings(phoneModeSettings)
    {
        addOptions(modesOptList());

        setTitle(utils::translate("app_settings_phone_modes"));
    }

    void PhoneModesWindow::onBeforeShow(ShowMode m, SwitchData *d)
    {
        auto selectedPhoneModeIndex = static_cast<unsigned int>(phoneModeSettings->getCurrentPhoneMode());

        modifiedModesStrings = initialModesStrings;
        modifiedModesStrings[selectedPhoneModeIndex] =
            "<text><b>" + modifiedModesStrings[selectedPhoneModeIndex] + " </b></text>";

        refreshOptions(modesOptList(), selectedPhoneModeIndex);
    }

    auto PhoneModesWindow::modesOptList() -> std::list<gui::Option>
    {
        std::list<gui::Option> optList;

        optList.emplace_back(std::make_unique<gui::option::OptionSettings>(
            modifiedModesStrings[0],
            [=](gui::Item &item) { return true; },
            [=](gui::Item &item) {
                if (item.focus) {
                    this->clearBottomBarText(BottomBar::Side::CENTER);
                }
                return true;
            },
            this));

        optList.emplace_back(std::make_unique<gui::option::OptionSettings>(
            modifiedModesStrings[1],
            [=](gui::Item &item) {
                this->application->switchWindow(gui::window::name::do_not_disturb, nullptr);
                return true;
            },
            [=](gui::Item &item) {
                if (item.focus) {
                    this->setBottomBarText(utils::translate(style::strings::common::adjust), BottomBar::Side::CENTER);
                }
                return true;
            },
            this,
            gui::option::SettingRightItem::ArrowWhite));

        optList.emplace_back(std::make_unique<gui::option::OptionSettings>(
            modifiedModesStrings[2],
            [=](gui::Item &item) {
                this->application->switchWindow(gui::window::name::offline, nullptr);
                return true;
            },
            [=](gui::Item &item) {
                if (item.focus) {
                    this->setBottomBarText(utils::translate(style::strings::common::adjust), BottomBar::Side::CENTER);
                }
                return true;
            },
            this,
            gui::option::SettingRightItem::ArrowWhite));

        bottomBar->setText(BottomBar::Side::RIGHT, utils::translate(style::strings::common::back));

        return optList;
    }
} // namespace gui
