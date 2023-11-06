// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "OptionWindow.hpp"
#include "ApplicationCommon.hpp"

namespace gui::option
{
    class OptionSettings : public option::Base
    {
      public:
        OptionSettings(const UTF8 &primaryText,
                       std::function<bool(Item &)> activatedCallback,
                       std::function<bool(Item &)> focusChangedCallback,
                       AppWindow *app,
                       SettingRightItem rightItem = SettingRightItem::Disabled,
                       bool indent                = false,
                       const UTF8 &textOnRight    = UTF8(),
                       bool isTextOnRightSmall    = true,
                       const UTF8 &secondaryText  = UTF8());
        [[nodiscard]] auto build() const -> ListItem * override;
        [[nodiscard]] auto str() const -> std::string override;

      private:
        UTF8 primaryText;
        UTF8 secondaryText;
        std::function<bool(Item &)> activatedCallback    = nullptr;
        std::function<bool(Item &)> focusChangedCallback = nullptr;
        AppWindow *app                                   = nullptr;
        SettingRightItem rightItem                       = SettingRightItem::Disabled;
        bool indent                                      = false;
        UTF8 textOnRight;
        bool isTextOnRightSmall = true;
    };
} // namespace gui::option
