// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <Application.hpp>

#include <common/models/AlarmModel.hpp>

#include <common/BellPowerOffPresenter.hpp>
#include <common/popups/presenter/AlarmActivatedPresenter.hpp>
#include <common/popups/AlarmActivatedWindow.hpp>
#include <common/popups/AlarmDeactivatedWindow.hpp>
#include <common/popups/BellTurnOffOptionWindow.hpp>
#include <common/popups/BellRebootWindow.hpp>
#include <common/windows/BellTurnOffWindow.hpp>
#include <common/windows/BellWelcomeWindow.hpp>

namespace app
{
    void Application::attachPopups(const std::vector<gui::popup::ID> &popupsList)
    {
        using namespace gui::popup;
        for (auto popup : popupsList) {
            switch (popup) {
            case ID::AlarmActivated:
                windowsFactory.attach(
                    window::alarm_activated_window, [](app::ApplicationCommon *app, const std::string &name) {
                        auto alarmModel = std::make_shared<app::AlarmModel>(app);
                        auto presenter  = std::make_unique<app::popup::AlarmActivatedPresenter>(alarmModel);
                        return std::make_unique<gui::AlarmActivatedWindow>(app, std::move(presenter));
                    });
                break;
            case ID::AlarmDeactivated:
                windowsFactory.attach(
                    window::alarm_deactivated_window, [](app::ApplicationCommon *app, const std::string &name) {
                        auto alarmModel = std::make_shared<app::AlarmModel>(app);
                        auto presenter  = std::make_unique<app::popup::AlarmActivatedPresenter>(alarmModel);
                        return std::make_unique<gui::AlarmDeactivatedWindow>(app, std::move(presenter));
                    });
                break;
            case ID::PowerOff:
                windowsFactory.attach(window::power_off_window, [](ApplicationCommon *app, const std::string &name) {
                    return std::make_unique<gui::BellTurnOffOptionWindow>(app, window::power_off_window);
                });
                windowsFactory.attach(gui::BellTurnOffWindow::name,
                                      [](ApplicationCommon *app, const std::string &name) {
                                          return std::make_unique<gui::BellTurnOffWindow>(
                                              app, std::make_unique<gui::BellPowerOffPresenter>(app));
                                      });
                windowsFactory.attach(gui::BellWelcomeWindow::defaultName,
                                      [](ApplicationCommon *app, const std::string &name) {
                                          return std::make_unique<gui::BellWelcomeWindow>(app);
                                      });
                break;
            case ID::Reboot:
                windowsFactory.attach(window::reboot_window, [](ApplicationCommon *app, const std::string &name) {
                    return std::make_unique<gui::BellRebootWindow>(app,
                                                                   std::make_unique<gui::BellPowerOffPresenter>(app));
                });
            default:
                break;
            }
        }
    }

    bool Application::isPopupPermitted(gui::popup::ID) const
    {
        return not((isCurrentWindow(gui::popup::resolveWindowName(gui::popup::ID::Reboot))) ||
                   (isCurrentWindow(gui::popup::resolveWindowName(gui::popup::ID::PowerOff))) ||
                   (isCurrentWindow(gui::BellTurnOffWindow::name)));
    }
} // namespace app
