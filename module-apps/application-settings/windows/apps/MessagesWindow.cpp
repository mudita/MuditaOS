// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "MessagesWindow.hpp"

#include <application-settings/data/SoundSelectData.hpp>
#include <application-settings/windows/WindowNames.hpp>

namespace gui
{
    MessagesWindow::MessagesWindow(app::Application *app,
                                   std::unique_ptr<audio_settings::AbstractAudioSettingsModel> &&audioModel)
        : BaseSettingsWindow(app, gui::window::name::messages), mWidgetMaker(this), mAudioModel(std::move(audioModel))
    {
        mShowUnreadMessagesFirst = false;
        setTitle(utils::translate("app_settings_apps_messages"));
    }

    std::list<Option> MessagesWindow::buildOptionsList()
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
                optionList, utils::translate("app_settings_message_sound"), [&]() { openMessageSoundWindow(); }, true);
        }

#if DISABLED_SETTINGS_OPTIONS == 1
        mWidgetMaker.addSwitchOption(optionList,
                                     utils::translate("app_settings_show_unread_first"),
                                     mShowUnreadMessagesFirst,
                                     [&]() { switchShowUnreadFirst(); });

        mWidgetMaker.addSelectOption(
            optionList, utils::translate("app_settings_Templates"), [&]() { openMessageTemplates(); });
#endif // DISABLED_SETTINGS_OPTIONS

        return optionList;
    }

    void MessagesWindow::switchVibrationState()
    {
        (mVibrationsEnabled) ? mAudioModel->setVibrationDisabled() : mAudioModel->setVibrationEnabled();
        refreshOptionsList();
    }

    void MessagesWindow::switchSoundState()
    {
        mSoundEnabled ? mAudioModel->setSoundDisabled() : mAudioModel->setSoundEnabled();
        refreshOptionsList();
    }

    void MessagesWindow::switchShowUnreadFirst()
    {
        mShowUnreadMessagesFirst = !mShowUnreadMessagesFirst;
        LOG_INFO("switchSoundState %d", static_cast<int>(mSoundEnabled));
        refreshOptionsList();
    }

    void MessagesWindow::openMessageSoundWindow()
    {
        SoundSelectData::Info info;
        info.windowTitle = utils::translate("app_settings_message_sound");
        info.audioModel  = mAudioModel.get();

        application->switchWindow(gui::window::name::sound_select, std::make_unique<SoundSelectData>(info));
    }

    void MessagesWindow::openMessageTemplates()
    {
        this->application->switchWindow(gui::window::name::message_templates);
    }

} // namespace gui
