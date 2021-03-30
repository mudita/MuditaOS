// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "CalendarWindow.hpp"

#include <application-settings-new/ApplicationSettings.hpp>
#include <i18n/i18n.hpp>
#include "BaseSettingsWindow.hpp"
#include <OptionSetting.hpp>

namespace gui
{
    CalendarWindow::CalendarWindow(app::Application *app)
        : BaseSettingsWindow(app, gui::window::name::calendar), mWidgetMaker(this)
    {
        mVibrationsEnabled = true;
        mSoundEnabled      = true;
        setTitle(utils::localize.get("app_settings_apps_calendar"));
    }

    std::list<Option> CalendarWindow::buildOptionsList()
    {
        std::list<gui::Option> optionList;
        mWidgetMaker.addSwitchOption(optionList,
                                     utils::translateI18("app_settings_vibration"),
                                     mVibrationsEnabled,
                                     [&]() { switchVibrationState(); });

        mWidgetMaker.addSwitchOption(
            optionList, utils::translateI18("app_settings_sound"), mSoundEnabled, [&]() { switchSoundState(); });

        if (mSoundEnabled) {
            mWidgetMaker.addSelectOption(
                optionList,
                utils::translateI18("app_settings_notification_sound"),
                [&]() { openNoticicationSoundWindow(); },
                true);
        }

        return optionList;
    }

    void CalendarWindow::switchVibrationState()
    {
        mVibrationsEnabled = !mVibrationsEnabled;
        LOG_INFO("switchVibrationState %d", static_cast<int>(mVibrationsEnabled));
        refreshOptionsList();
    }

    void CalendarWindow::switchSoundState()
    {
        mSoundEnabled = !mSoundEnabled;
        LOG_INFO("switchSoundState %d", static_cast<int>(mSoundEnabled));
        refreshOptionsList();
    }

    void CalendarWindow::openNoticicationSoundWindow()
    {
        application->switchWindow(gui::window::name::notification_sound);
    }

} // namespace gui
