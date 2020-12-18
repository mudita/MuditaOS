// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "OptionWindow.hpp"
#include "Application.hpp"

namespace gui
{
    enum class RightItem
    {
        Disabled,
        ArrowBlack,
        ArrowWhite,
        On,
        Off,
        Bt,
        Checked,
    };

    class OptionSettings : public option::Base
    {
      private:
        UTF8 text;
        std::function<bool(Item &)> activatedCallback = nullptr;
        std::function<bool(Item &)> focusCb           = nullptr;
        AppWindow *app                                = nullptr;
        RightItem rightItem                           = RightItem::Disabled;
        bool indent                                   = false;

      public:
        OptionSettings(const UTF8 text,
                       std::function<bool(Item &)> cb,
                       std::function<bool(Item &)> cbFocus,
                       AppWindow *app,
                       RightItem rightItem = RightItem::Disabled,
                       bool indent         = false)
            : text(text), activatedCallback(std::move(cb)), focusCb(std::move(cbFocus)), app(app), rightItem(rightItem),
              indent(indent)
        {}
        [[nodiscard]] auto build() const -> Item * override;
    };
} // namespace gui
