// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <common/ApplicationBell.hpp>
#include <common/popups/AlarmActivatedWindow.hpp>

namespace app
{
    void ApplicationBell::attachPopups(const std::vector<gui::popup::ID> &popupsList)
    {
        using namespace gui::popup;
        app::Application::attachPopups(popupsList);
        for (auto popup : popupsList) {
            switch (popup) {
            case ID::AlarmActivated:
                windowsFactory.attach(window::alarm_activated_window,
                                      [](app::Application *app, const std::string &name) {
                                          return std::make_unique<gui::AlarmActivatedWindow>(app);
                                      });
                break;
            default:
                break;
            }
        }
    }
} // namespace app
