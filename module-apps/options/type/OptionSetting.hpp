// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "OptionWindow.hpp"
#include "Application.hpp"

namespace gui::option
{
    class OptionSettings : public option::Base
    {
      protected:
        UTF8 text;
        std::function<bool(Item &)> activatedCallback    = nullptr;
        std::function<bool(Item &)> focusChangedCallback = nullptr;
        AppWindow *app                                   = nullptr;
        SettingRightItem rightItem                       = SettingRightItem::Disabled;
        bool indent                                      = false;

      public:
        OptionSettings(UTF8 text,
                       std::function<bool(Item &)> activatedCallback,
                       std::function<bool(Item &)> focusChangedCallback,
                       AppWindow *app,
                       SettingRightItem rightItem = SettingRightItem::Disabled,
                       bool indent                = false)
            : text(std::move(text)), activatedCallback(std::move(activatedCallback)),
              focusChangedCallback(std::move(focusChangedCallback)), app(app), rightItem(rightItem), indent(indent)
        {}
        [[nodiscard]] auto build() const -> ListItem * override;
        [[nodiscard]] auto str() const -> std::string override
        {
            return text;
        }
    };
} // namespace gui::option
