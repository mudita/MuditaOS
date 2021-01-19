// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <SwitchData.hpp>
#include <Option.hpp>
#include <list>

namespace gui
{
    class OptionsWindowOptions : public SwitchData
    {
        std::list<gui::Option> options;

      public:
        OptionsWindowOptions(std::list<gui::Option> options) : options(std::move(options))
        {}

        std::list<gui::Option> takeOptions()
        {
            return std::move(options);
        }
    };
}; // namespace gui
