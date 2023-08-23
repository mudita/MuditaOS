// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "popups/BellTurnOffOptionWindow.hpp"

#include <application-bell-main/ApplicationBellMain.hpp>
#include <apps-common/widgets/BellBaseLayout.hpp>
#include <apps-common/ApplicationCommon.hpp>
#include <common/data/StyleCommon.hpp>
#include <common/windows/BellTurnOffWindow.hpp>
#include <i18n/i18n.hpp>
#include <options/OptionBellMenu.hpp>
#include <service-appmgr/ServiceApplicationManagerName.hpp>
#include <service-appmgr/messages/SwitchRequest.hpp>
#include <GuiTimer.hpp>

namespace
{
    constexpr auto timerName{"BellTurnOffPopupTimer"};
} // namespace

namespace gui
{
    BellTurnOffOptionWindow::BellTurnOffOptionWindow(app::ApplicationCommon *app,
                                                     std::chrono::milliseconds duration,
                                                     const char *name)
        : BellShortOptionWindow(app, name), yesStr{utils::translate("common_yes")}, noStr{utils::translate("common_no")}
    {
        addOptions(settingsOptionsList());
        setListTitle(utils::translate("app_bell_turn_off_question"));

        if (duration > std::chrono::milliseconds::zero()) {
            popupTimer = app::GuiTimerFactory::createSingleShotTimer(application, this, timerName, duration);
        }

        timerCallback = [this, name](Item &, sys::Timer &timer) {
            if (application->getCurrentWindow()->getName() == name) {
                application->returnToPreviousWindow();
                return true;
            }
            return false;
        };
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

    void BellTurnOffOptionWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        if (popupTimer.isValid()) {
            popupTimer.start();
        }
    }

    void BellTurnOffOptionWindow::onClose([[maybe_unused]] CloseReason reason)
    {
        if (popupTimer.isValid()) {
            popupTimer.stop();
            popupTimer.reset(nullptr);
        }
    }
} /* namespace gui */
