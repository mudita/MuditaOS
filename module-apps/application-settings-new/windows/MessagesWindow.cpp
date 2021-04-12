// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "MessagesWindow.hpp"

#include <application-settings-new/ApplicationSettings.hpp>
#include <i18n/i18n.hpp>
#include <OptionWindow.hpp>
#include <OptionSetting.hpp>

namespace gui
{
    MessagesWindow::MessagesWindow(app::Application *app, app::settingsInterface::ConnectedSettings *connectedSettings)
        : BaseSettingsWindow(app, gui::window::name::messages), mWidgetMaker(this), connectedSettings(connectedSettings)
    {
        mShowUnreadMessagesFirst = true;
        setTitle(utils::localize.get("app_settings_apps_messages"));
    }

    std::list<Option> MessagesWindow::buildOptionsList()
    {
        std::list<gui::Option> optionList;

        mWidgetMaker.addSwitchOption(optionList,
                                     utils::translateI18("app_settings_vibration"),
                                     connectedSettings->isMessageVibrationEnabled(),
                                     [&]() {
                                         connectedSettings->setMessageVibrationEnabled(
                                             !connectedSettings->isMessageVibrationEnabled());
                                         refreshOptionsList();
                                     });

        mWidgetMaker.addSwitchOption(
            optionList, utils::translateI18("app_settings_sound"), connectedSettings->isMessageSoundEnabled(), [&]() {
                connectedSettings->setMessageSoundEnabled(!connectedSettings->isMessageSoundEnabled());
                refreshOptionsList();
            });

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
