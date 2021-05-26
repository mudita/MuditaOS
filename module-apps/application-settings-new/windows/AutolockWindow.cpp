// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AutolockWindow.hpp"
#include <application-settings-new/data/AutoLockData.hpp>
#include <OptionSetting.hpp>
#include <i18n/i18n.hpp>

namespace gui
{
    namespace
    {
        const std::vector<std::pair<std::string, std::chrono::seconds>> autoLockTimes = {
            {"15s", std::chrono::seconds{15}},
            {"30s", std::chrono::seconds{30}},
            {"1m", std::chrono::minutes{1}},
            {"2m", std::chrono::minutes{2}},
            {"5m", std::chrono::minutes{5}},
            {"10m", std::chrono::minutes{10}},
            {"20m", std::chrono::minutes{20}}};
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
                    currentAutoLockTimeout = time;
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
                currentAutoLockTimeout == time ? gui::option::SettingRightItem::Checked
                                               : gui::option::SettingRightItem::Disabled));
        }

        return optionsList;
    }

    void AutolockWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        if (auto autoLockData = dynamic_cast<const AutoLockData *>(data); data != nullptr) {
            currentAutoLockTimeout = autoLockData->getValue();
        }
        else if (mode == ShowMode::GUI_SHOW_INIT) {
            autoLockSettings->getAutoLockTime();
        }
        BaseSettingsWindow::onBeforeShow(mode, data);
    }

} // namespace gui
