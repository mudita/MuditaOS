// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "OptionWindow.hpp"

namespace style::bell_options
{
    inline constexpr auto h                  = 60U;
    inline constexpr auto option_margin      = 2;
    inline constexpr auto default_text_width = 380;
} // namespace style::bell_options

namespace gui::option
{
    class OptionBellMenu : public option::Base
    {
      private:
        UTF8 text;
        std::function<bool(Item &)> activatedCallback    = nullptr;
        std::function<bool(Item &)> focusChangedCallback = nullptr;
        AppWindow *app                                   = nullptr;

      public:
        OptionBellMenu(const UTF8 &text,
                       std::function<bool(Item &)> activatedCallback,
                       std::function<bool(Item &)> focusChangedCallback,
                       AppWindow *app)
            : text(text), activatedCallback(std::move(activatedCallback)),
              focusChangedCallback(std::move(focusChangedCallback)), app(app)
        {}
        [[nodiscard]] auto build() const -> ListItem * override;
        [[nodiscard]] auto str() const -> std::string override
        {
            return text;
        }
    };
} // namespace gui::option
