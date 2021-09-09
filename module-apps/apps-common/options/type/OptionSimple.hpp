// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "OptionBase.hpp"
#include "OptionStyle.hpp"

namespace gui::option
{
    class Simple : public Base
    {
      protected:
        UTF8 text                                     = "";
        std::function<bool(Item &)> activatedCallback = nullptr;
        Arrow arrow                                   = Arrow::Disabled;

      public:
        Simple() = default;
        Simple(UTF8 text, std::function<bool(Item &)> activatedCallback, Arrow arrow);

        [[nodiscard]] auto build() const -> ListItem * override;
        [[nodiscard]] auto str() const -> std::string override
        {
            return text;
        }
    };
}; // namespace gui::option
