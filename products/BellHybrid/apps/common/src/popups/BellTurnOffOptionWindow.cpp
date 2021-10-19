// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "popups/BellTurnOffOptionWindow.hpp"

#include <application-bell-main/ApplicationBellMain.hpp>
#include <apps-common/widgets/BellBaseLayout.hpp>
#include <apps-common/ApplicationCommon.hpp>
#include <common/data/StyleCommon.hpp>
#include <common/windows/BellTurnOffWindow.hpp>
#include <i18n/i18n.hpp>
#include <options/OptionBellMenu.hpp>
#include <service-appmgr/Constants.hpp>
#include <service-appmgr/messages/SwitchRequest.hpp>

namespace gui
{
    BellTurnOffOptionWindow::BellTurnOffOptionWindow(app::ApplicationCommon *app, const char *name)
        : BellShortOptionWindow(app, name), yesStr{utils::translate("common_yes")}, noStr{utils::translate("common_no")}
    {
        addOptions(settingsOptionsList());
        setListTitle(utils::translate("app_bell_turn_off_question"));
    }

    std::list<Option> BellTurnOffOptionWindow::settingsOptionsList()
    {
        std::list<gui::Option> settingsOptionList;
        auto addWinSettings = [&](const UTF8 &name, std::function<bool(Item &)> activatedCallback) {
            settingsOptionList.emplace_back(std::make_unique<gui::option::OptionBellMenu>(
                name, activatedCallback, [=](gui::Item &) { return true; }, this));
        };

        addWinSettings(noStr, [this](auto &) {
            application->returnToPreviousWindow();
            return true;
        });
        addWinSettings(yesStr, [this](auto &) {
            auto switchRequest = std::make_unique<app::manager::SwitchRequest>(
                service::name::appmgr, app::applicationBellName, gui::BellTurnOffWindow::name, nullptr);
            application->bus.sendUnicast(std::move(switchRequest), service::name::appmgr);
            return true;
        });

        return settingsOptionList;
    }
} /* namespace gui */
