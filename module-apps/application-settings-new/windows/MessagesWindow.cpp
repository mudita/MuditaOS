// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "MessagesWindow.hpp"

#include <application-settings-new/ApplicationSettings.hpp>
#include <i18n/i18n.hpp>
#include <OptionWindow.hpp>
#include <OptionSetting.hpp>

namespace gui
{
    MessagesWindow::MessagesWindow(app::Application *app)
        : BaseSettingsWindow(app, gui::window::name::messages), mWidgetMaker(this)
    {
        mVibrationsEnabled       = false;
        mSoundEnabled            = true;
        mShowUnreadMessagesFirst = true;
        setTitle(utils::localize.get("app_settings_apps_messages"));
    }

    std::list<Option> MessagesWindow::buildOptionsList()
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
                utils::translateI18("app_settings_message_sound"),
                [&]() { openMessageSoundWindow(); },
                true);
        }

        mWidgetMaker.addSwitchOption(optionList,
                                     utils::translateI18("app_settings_show_unread_first"),
                                     mShowUnreadMessagesFirst,
                                     [&]() { switchShowUnreadFirst(); });

        mWidgetMaker.addSelectOption(
            optionList, utils::translateI18("app_settings_Templates"), [&]() { openMessageTemplates(); });
        return optionList;
    }

    void MessagesWindow::switchVibrationState()
    {
        mVibrationsEnabled = !mVibrationsEnabled;
        LOG_INFO("switchVibrationState %d", static_cast<int>(mVibrationsEnabled));
        refreshOptionsList();
    }

    void MessagesWindow::switchSoundState()
    {
        mSoundEnabled = !mSoundEnabled;
        LOG_INFO("switchSoundState %d", static_cast<int>(mSoundEnabled));
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
        this->application->switchWindow(gui::window::name::message_sound);
    }

    void MessagesWindow::openMessageTemplates()
    {
        this->application->switchWindow(gui::window::name::message_templates);
    }

} // namespace gui
