// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AutolockWindow.hpp"
#include "OptionSetting.hpp"

#include <i18n/i18n.hpp>

namespace gui
{
    namespace
    {
        const std::vector<std::pair<std::string, std::chrono::milliseconds>> autoLockTimes = {
            {"15s", std::chrono::milliseconds{15000}},
            {"30s", std::chrono::milliseconds{30000}},
            {"1m", std::chrono::milliseconds{60000}},
            {"2m", std::chrono::milliseconds{120000}},
            {"5m", std::chrono::milliseconds{300000}},
            {"10m", std::chrono::milliseconds{600000}},
            {"20m", std::chrono::milliseconds{1200000}}};
    } // namespace

    AutolockWindow::AutolockWindow(app::Application *app, app::settingsInterface::AutoLockSettings *autoLockSettings)
        : BaseSettingsWindow(app, window::name::autolock), autoLockSettings(autoLockSettings)
    {
        setTitle(utils::translate("app_settings_display_locked_screen_autolock"));
    }

    auto AutolockWindow::buildOptionsList() -> std::list<gui::Option>
    {
        std::list<gui::Option> optionsList;
        for (const auto &[timeString, time] : autoLockTimes) {
            optionsList.emplace_back(std::make_unique<gui::option::OptionSettings>(
                timeString,
                [=](gui::Item &item) {
                    autoLockSettings->setAutoLockTime(time);
                    refreshOptionsList();
                    return true;
                },
                [=](gui::Item &item) {
                    if (item.focus) {
                        this->setBottomBarText(utils::translate(style::strings::common::select),
                                               BottomBar::Side::CENTER);
                    }
                    return true;
                },
                this,
                autoLockSettings->getAutoLockTime() == time ? gui::option::SettingRightItem::Checked
                                                            : gui::option::SettingRightItem::Disabled));
        }

        return optionsList;
    }

} // namespace gui
