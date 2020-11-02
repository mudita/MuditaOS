// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AutolockWindow.hpp"
#include "application-settings-new/ApplicationSettings.hpp"
#include "windows/OptionSetting.hpp"

#include <i18/i18.hpp>

namespace gui
{

    AutolockWindow::AutolockWindow(app::Application *app) : BaseSettingsWindow(app, window::name::autolock)
    {
        setTitle(utils::localize.get("app_settings_display_locked_screen_autolock"));
    }

    auto AutolockWindow::buildOptionsList() -> std::list<gui::Option>
    {
        std::list<gui::Option> optionsList;
        std::vector<std::string> autoLockTimes = {"15s", "30s", "1m", "2m", "5m", "10m", "20m"};

        for (auto time : autoLockTimes) {
            optionsList.emplace_back(std::make_unique<gui::OptionSettings>(
                time,
                [=](gui::Item &item) {
                    selectedTime = time;
                    rebuildOptionList();
                    return true;
                },
                [=](gui::Item &item) {
                    if (item.focus) {
                        this->setBottomBarText(utils::translateI18(style::strings::common::select),
                                               BottomBar::Side::CENTER);
                    }
                    return true;
                },
                this,
                selectedTime == time ? RightItem::Checked : RightItem::Disabled));
        }

        return optionsList;
    }

} // namespace gui
