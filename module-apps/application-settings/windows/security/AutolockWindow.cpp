// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AutolockWindow.hpp"

#include <application-settings/ApplicationSettings.hpp>
#include <application-settings/data/AutoLockData.hpp>
#include <application-settings/windows/WindowNames.hpp>

#include <OptionSetting.hpp>
#include <i18n/i18n.hpp>

namespace gui
{
    namespace
    {
        const std::vector<std::pair<std::string, std::chrono::seconds>> autoLockTimes = {
            {"15 s", std::chrono::seconds{15}},
            {"30 s", std::chrono::seconds{30}},
            {"1 m", std::chrono::minutes{1}},
            {"2 m", std::chrono::minutes{2}},
            {"5 m", std::chrono::minutes{5}},
            {"10 m", std::chrono::minutes{10}},
            {"20 m", std::chrono::minutes{20}}};
    } // namespace

    AutolockWindow::AutolockWindow(app::ApplicationCommon *app,
                                   app::settingsInterface::AutoLockSettings *autoLockSettings)
        : BaseSettingsWindow(app, window::name::autolock), autoLockSettings(autoLockSettings)
    {
        setTitle(utils::translate("app_settings_display_security_autolock"));
    }

    auto AutolockWindow::buildOptionsList() -> std::list<gui::Option>
    {
        std::list<gui::Option> optionsList;
        for (const auto &[timeString, time] : autoLockTimes) {
            optionsList.emplace_back(std::make_unique<gui::option::OptionSettings>(
                timeString,
                [=](gui::Item &item) {
                    autoLockSettings->setAutoLockTime(time);
                    currentAutoLockTimeout = time;
                    refreshOptionsList();
                    return true;
                },
                [=](gui::Item &item) {
                    if (item.focus) {
                        this->setNavBarText(utils::translate(style::strings::common::select), nav_bar::Side::Center);
                    }
                    return true;
                },
                this,
                currentAutoLockTimeout == time ? gui::option::SettingRightItem::Checked
                                               : gui::option::SettingRightItem::Disabled));
        }

        return optionsList;
    }

    void AutolockWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        currentAutoLockTimeout = autoLockSettings->getAutoLockTime();
        auto it                = std::find_if(
            autoLockTimes.begin(), autoLockTimes.end(), [&](const std::pair<std::string, std::chrono::seconds> &elem) {
                return elem.second == currentAutoLockTimeout;
            });

        refreshOptionsList(std::distance(autoLockTimes.begin(), it));
    }

} // namespace gui
