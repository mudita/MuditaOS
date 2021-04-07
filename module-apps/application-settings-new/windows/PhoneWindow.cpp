// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "PhoneWindow.hpp"

#include <application-settings-new/ApplicationSettings.hpp>
#include <i18n/i18n.hpp>
#include <OptionWindow.hpp>
#include <OptionSetting.hpp>

namespace gui
{
    PhoneWindow::PhoneWindow(app::Application *app,
                             std::unique_ptr<audio_settings::AbstractAudioSettingsModel> &&audioModel)
        : BaseSettingsWindow(app, gui::window::name::phone), mWidgetMaker(this), mAudioModel(std::move(audioModel))
    {
        setTitle(utils::localize.get("app_settings_apps_phone"));
    }

    std::list<Option> PhoneWindow::buildOptionsList()
    {
        std::list<gui::Option> optionList;
        mVibrationsEnabled = mAudioModel->isVibrationEnabled();
        mSoundEnabled      = mAudioModel->isSoundEnabled();

        mWidgetMaker.addSwitchOption(optionList,
                                     utils::translateI18("app_settings_vibration"),
                                     mVibrationsEnabled,
                                     [&]() { switchVibrationState(); });

        mWidgetMaker.addSwitchOption(
            optionList, utils::translateI18("app_settings_sound"), mSoundEnabled, [&]() { switchSoundState(); });

        if (mSoundEnabled) {
            mWidgetMaker.addSelectOption(
                optionList, utils::translateI18("app_settings_call_ringtome"), [&]() { openRingtoneWindow(); }, true);
        }

        return optionList;
    }

    void PhoneWindow::switchVibrationState()
    {
        (mVibrationsEnabled) ? mAudioModel->setVibrationDisabled() : mAudioModel->setVibrationEnabled();
        refreshOptionsList();
    }

    void PhoneWindow::switchSoundState()
    {
        mSoundEnabled ? mAudioModel->setSoundDisabled() : mAudioModel->setSoundEnabled();
        refreshOptionsList();
    }

    void PhoneWindow::openRingtoneWindow()
    {
        application->switchWindow(gui::window::name::call_ringtone);
    }

} // namespace gui
