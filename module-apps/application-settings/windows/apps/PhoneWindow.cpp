// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "PhoneWindow.hpp"

#include <application-settings/data/SoundSelectData.hpp>
#include <application-settings/windows/WindowNames.hpp>
#include <widgets/SpinBoxOptionSettings.hpp>

namespace gui
{
    PhoneWindow::PhoneWindow(app::ApplicationCommon *app,
                             std::unique_ptr<audio_settings::AbstractAudioSettingsModel> &&audioModel)
        : BaseSettingsWindow(app, gui::window::name::phone), mWidgetMaker(this), mAudioModel(std::move(audioModel))
    {
        setTitle(utils::translate("app_settings_apps_phone"));
    }

    std::list<Option> PhoneWindow::buildOptionsList()
    {
        std::list<gui::Option> optionList;
        mVibrationsEnabled = mAudioModel->isVibrationEnabled();
        mSoundEnabled      = mAudioModel->isSoundEnabled();

        mWidgetMaker.addSwitchOption(optionList, utils::translate("app_settings_vibration"), mVibrationsEnabled, [&]() {
            switchVibrationState();
        });

#if DEVELOPER_SETTINGS_OPTIONS == 1
        if (mVibrationsEnabled) {
            optionList.emplace_back(std::make_unique<gui::SpinBoxOptionSettings>(
                utils::translate("app_settings_volume"),
                mAudioModel->getVibrationLevel(),
                10,
                [&](uint8_t value) {
                    mAudioModel->setVibrationLevel(value);
                    return true;
                },
                [&](const UTF8 &text) {
                    application->getCurrentWindow()->navBarTemporaryMode(text, nav_bar::Side::Center, false);
                },
                [&]() { application->getCurrentWindow()->navBarRestoreFromTemporaryMode(); },
                true));
        }
#endif // DEVELOPER_SETTINGS_OPTIONS

        mWidgetMaker.addSwitchOption(
            optionList, utils::translate("app_settings_sound"), mSoundEnabled, [&]() { switchSoundState(); });

        if (mSoundEnabled) {
            mWidgetMaker.addSelectOption(
                optionList, utils::translate("app_settings_call_ringtome"), [&]() { openRingtoneWindow(); }, true);
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
        SoundSelectData::Info info;
        info.windowTitle = utils::translate("app_settings_call_ringtome");
        info.audioModel  = mAudioModel.get();
        application->switchWindow(gui::window::name::sound_select, std::make_unique<SoundSelectData>(info));
    }

} // namespace gui
