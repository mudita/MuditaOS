// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "OptionsWidgetMaker.hpp"

#include <OptionSetting.hpp>

namespace gui
{
    OptionsWidgetMaker::OptionsWidgetMaker(OptionWindow *window) : mWindow(window)
    {}

    void OptionsWidgetMaker::addSwitchOption(
        std::list<gui::Option> &list, const UTF8 &text, bool &option, std::function<void()> callback, bool indent)
    {
        list.emplace_back(std::make_unique<option::OptionSettings>(
            std::move(text),
            [=](Item &item) {
                callback();
                return true;
            },
            [=](Item &item) {
                if (item.focus) {
                    mWindow->setBottomBarText(utils::translate(style::strings::common::Switch),
                                              BottomBar::Side::CENTER);
                }
                else {
                    mWindow->setBottomBarText(utils::translate(style::strings::common::select),
                                              BottomBar::Side::CENTER);
                }
                return true;
            },
            nullptr,
            option ? option::SettingRightItem::On : option::SettingRightItem::Off,
            indent));
    }

    void OptionsWidgetMaker::addSelectOption(std::list<gui::Option> &list,
                                             const UTF8 &text,
                                             std::function<void(void)> callback,
                                             bool indent)
    {
        list.emplace_back(std::make_unique<option::OptionSettings>(
            std::move(text),
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
