// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <string>
#include <utf8/UTF8.hpp>
#include <functional>
#include <memory>

#include "OptionBase.hpp"
#include "OptionSimple.hpp"
#include "OptionStyle.hpp"

namespace gui
{
    ///  @brief Option class with various constructor methods (factory).
    ///
    ///  Option container which holds pointer to Base Option GUI element (or derived Options).
    ///

    class Option
    {
      private:
        std::unique_ptr<option::Base> option;

      public:
        explicit Option(std::unique_ptr<option::Base> option) : option(std::move(option))
        {}

        Option(const UTF8 &text, std::function<bool(Item &)> cb, option::Arrow arrow = option::Arrow::Disabled)
            : Option(std::make_unique<option::Simple>(text, std::move(cb), arrow))
        {}

        Option(const Option &o) = delete;

        Option(Option &&o) noexcept : option{std::move(o.option)}
        {}

        [[nodiscard]] auto build() const -> ListItem *
        {
            assert(option);
            return option->build();
        }

        [[nodiscard]] auto str() const -> std::string
        {
            return option->str();
        }
    };
} // namespace gui
