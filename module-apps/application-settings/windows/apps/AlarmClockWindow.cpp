// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AlarmClockWindow.hpp"

#include <application-settings/widgets/SpinBoxOptionSettings.hpp>
#include <application-settings/windows/WindowNames.hpp>

namespace gui
{
    AlarmClockWindow::AlarmClockWindow(app::ApplicationCommon *app,
                                       std::unique_ptr<audio_settings::AbstractAudioSettingsModel> &&audioModel)
        : BaseSettingsWindow(app, gui::window::name::alarm_clock), mWidgetMaker(this),
          mAudioModel(std::move(audioModel))
    {
        setTitle(utils::translate("app_settings_apps_alarm_clock"));
    }

    std::list<Option> AlarmClockWindow::buildOptionsList()
    {
        std::list<gui::Option> optionList;
        mVibrationsEnabled   = mAudioModel->isVibrationEnabled();
        mManualVolumeEnabled = mAudioModel->isSystemSoundEnabled();
        mWidgetMaker.addSwitchOption(optionList, utils::translate("app_settings_vibration"), mVibrationsEnabled, [&]() {
            switchVibrationState();
        });

        mWidgetMaker.addSwitchOption(optionList,
                                     utils::translate("app_settings_apps_alarm_clock_manual_volume"),
                                     mManualVolumeEnabled,
                                     [&]() { switchManualVolumeState(); });

        if (mManualVolumeEnabled) {
            constexpr auto maxVolumeLevel = 10;
            optionList.emplace_back(std::make_unique<gui::SpinBoxOptionSettings>(
                utils::translate("app_settings_volume"),
                utils::translate("app_settings_volume_focused"),
                mAudioModel->getVolume(),
                maxVolumeLevel,
                [&](uint8_t value) {
                    setVolume(value);
                    return true;
                },
                [&](const UTF8 &text) {
                    application->getCurrentWindow()->navBarTemporaryMode(text, nav_bar::Side::Center, false);
                },
                [&]() { application->getCurrentWindow()->navBarRestoreFromTemporaryMode(); },
                true));
        }

        return optionList;
    }

    void AlarmClockWindow::switchVibrationState()
    {
        (mVibrationsEnabled) ? mAudioModel->setVibrationDisabled() : mAudioModel->setVibrationEnabled();
        refreshOptionsList();
    }

    void AlarmClockWindow::switchManualVolumeState()
    {
        (mManualVolumeEnabled) ? mAudioModel->setIsSystemSoundEnabled() : mAudioModel->setIsSystemSoundDisabled();
        refreshOptionsList();
    }

    void AlarmClockWindow::setVolume(uint8_t vol)
    {
        LOG_INFO("New volume: %d", vol);
        mAudioModel->setVolume(vol);
    }

} // namespace gui
