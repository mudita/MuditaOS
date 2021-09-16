// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <Application.hpp>
#include <common/popups/AlarmActivatedWindow.hpp>
#include <common/popups/AlarmDeactivatedWindow.hpp>

namespace app
{
    void Application::attachPopups(const std::vector<gui::popup::ID> &popupsList)
    {
        using namespace gui::popup;
        app::ApplicationCommon::attachPopups(popupsList);
        for (auto popup : popupsList) {
            switch (popup) {
            case ID::AlarmActivated:
                windowsFactory.attach(window::alarm_activated_window,
                                      [](app::ApplicationCommon *app, const std::string &name) {
                                          return std::make_unique<gui::AlarmActivatedWindow>(app);
                                      });
                break;
            case ID::AlarmDeactivated:
                windowsFactory.attach(window::alarm_deactivated_window,
                                      [](app::ApplicationCommon *app, const std::string &name) {
                                          return std::make_unique<gui::AlarmDeactivatedWindow>(app);
                                      });
                break;
            default:
                break;
            }
        }
    }
} // namespace app
