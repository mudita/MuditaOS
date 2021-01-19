// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "MessagesWindow.hpp"

#include <application-settings-new/ApplicationSettings.hpp>
#include <i18n/i18n.hpp>
#include <OptionWindow.hpp>
#include <OptionSetting.hpp>

namespace gui
{
    MessagesWindow::MessagesWindow(app::Application *app) : OptionWindow(app, gui::window::name::messages)
    {
        addOptions(messagesOptList());
        setTitle(utils::localize.get("app_settings_apps_messages"));
    }

    std::list<Option> MessagesWindow::messagesOptList()
    {
        std::list<gui::Option> optionList;

        auto addMenuSwitch = [&](UTF8 name, std::string window) {
            optionList.emplace_back(std::make_unique<gui::option::OptionSettings>(
                name,
                [=](gui::Item &item) {
                    showUnreadMessagesFirst = !showUnreadMessagesFirst;
                    if (showUnreadMessagesFirst)
                        LOG_INFO("showUnreadMessagesFirst =true ");
                    else
                        LOG_INFO("showUnreadMessagesFirst =false ");
                    rebuildOptList();
                    return true;
                },
                [&](gui::Item &item) {
                    if (item.focus) {
                        this->setBottomBarText(utils::localize.get(style::strings::common::Switch),
                                               BottomBar::Side::CENTER);
                    }
                    else {
                        this->setBottomBarText(utils::localize.get(style::strings::common::select),
                                               BottomBar::Side::CENTER);
                    }
                    return true;
                },
                nullptr,
                showUnreadMessagesFirst ? gui::option::SettingRightItem::On : gui::option::SettingRightItem::Off));
        };

        auto addMenu = [&](UTF8 name, std::string window) {
            optionList.emplace_back(std::make_unique<gui::option::OptionSettings>(
                name,
                [=](gui::Item &item) {
                    if (window.empty()) {
                        return false;
                    }
                    LOG_INFO("switching to %s page", window.c_str());
                    application->switchWindow(window, nullptr);
                    return true;
                },
                nullptr,
                nullptr,
                gui::option::SettingRightItem::ArrowWhite));
        };

        bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get(style::strings::common::select));
        topBar->setActive(TopBar::Elements::SIGNAL, false);
        topBar->setActive(TopBar::Elements::BATTERY, false);
        topBar->setActive(TopBar::Elements::SIM, false);

        addMenuSwitch(utils::translateI18("app_settings_show_unread_first"), "");
        addMenu(utils::translateI18("app_settings_Templates"), gui::window::name::templates);

        return optionList;
    }

    void MessagesWindow::rebuildOptList()
    {
        clearOptions();
        addOptions(messagesOptList());
    }

    void MessagesWindow::onBeforeShow(ShowMode m, SwitchData *d)
    {
        rebuildOptList();
    }

} // namespace gui
