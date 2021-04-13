// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "CalendarWindow.hpp"

#include <application-settings-new/ApplicationSettings.hpp>
#include <i18n/i18n.hpp>
#include "BaseSettingsWindow.hpp"
#include <OptionSetting.hpp>

namespace gui
{
    CalendarWindow::CalendarWindow(app::Application *app,
                                   std::unique_ptr<audio_settings::AbstractAudioSettingsModel> &&audioModel)
        : BaseSettingsWindow(app, gui::window::name::calendar), mWidgetMaker(this), mAudioModel(std::move(audioModel))
    {
        setTitle(utils::translate("app_settings_apps_calendar"));
    }

    std::list<Option> CalendarWindow::buildOptionsList()
    {
        std::list<gui::Option> optionList;
        mVibrationsEnabled = mAudioModel->isVibrationEnabled();
        mSoundEnabled      = mAudioModel->isSoundEnabled();

        mWidgetMaker.addSwitchOption(optionList, utils::translate("app_settings_vibration"), mVibrationsEnabled, [&]() {
            switchVibrationState();
        });

        mWidgetMaker.addSwitchOption(
            optionList, utils::translate("app_settings_sound"), mSoundEnabled, [&]() { switchSoundState(); });

        if (mSoundEnabled) {
            mWidgetMaker.addSelectOption(
                optionList,
                utils::translate("app_settings_notification_sound"),
                [&]() { openNoticicationSoundWindow(); },
                true);
        }

        return optionList;
    }

    void CalendarWindow::switchVibrationState()
    {
        (mVibrationsEnabled) ? mAudioModel->setVibrationDisabled() : mAudioModel->setVibrationEnabled();
        refreshOptionsList();
    }

    void CalendarWindow::switchSoundState()
    {
        mSoundEnabled ? mAudioModel->setSoundDisabled() : mAudioModel->setSoundEnabled();
        refreshOptionsList();
    }

    void CalendarWindow::openNoticicationSoundWindow()
    {
        application->switchWindow(gui::window::name::notification_sound);
    }

} // namespace gui
