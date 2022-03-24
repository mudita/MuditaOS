// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "OptionsWidgetMaker.hpp"

#include <OptionSetting.hpp>

namespace gui
{
    OptionsWidgetMaker::OptionsWidgetMaker(OptionWindow *window) : mWindow(window)
    {}

    void OptionsWidgetMaker::addSwitchOption(std::list<gui::Option> &list,
                                             const UTF8 &text,
                                             bool &option,
                                             const std::function<void()> &callback,
                                             bool indent)
    {
        list.emplace_back(std::make_unique<option::OptionSettings>(
            text,
            [=](Item &item) {
                callback();
                return true;
            },
            [=](Item &item) {
                if (item.focus) {
                    mWindow->setNavBarText(utils::translate(style::strings::common::Switch), nav_bar::Side::Center);
                }
                else {
                    mWindow->setNavBarText(utils::translate(style::strings::common::select), nav_bar::Side::Center);
                }
                return true;
            },
            nullptr,
            option ? option::SettingRightItem::On : option::SettingRightItem::Off,
            indent));
    }

    void OptionsWidgetMaker::addSelectOption(std::list<gui::Option> &list,
                                             const UTF8 &text,
                                             const std::function<void(void)> &callback,
                                             bool indent)
    {
        list.emplace_back(std::make_unique<option::OptionSettings>(
            text,
            [=](Item &item) {
                callback();
                return true;
            },
            nullptr,
            nullptr,
            option::SettingRightItem::ArrowWhite,
            indent));
    }

} // namespace gui
