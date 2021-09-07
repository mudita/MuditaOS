// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <string>
#include <utf8/UTF8.hpp>

#include "ListItem.hpp"

namespace gui::option
{
    ///  @brief Base Option virtual class.
    ///
    ///  Virtual class to be based on when creating new option GUI element.
    ///
    ///  All element need to implement build method which returns ListItem to be displayed on Options List.

    class Base
    {
      public:
        virtual ~Base()                                        = default;
        [[nodiscard]] virtual auto build() const -> ListItem * = 0;
        [[nodiscard]] virtual auto str() const -> std::string
        {
            return "";
        }
    };
}; // namespace gui::option
