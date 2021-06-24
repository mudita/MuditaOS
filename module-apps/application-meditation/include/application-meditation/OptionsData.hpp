// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <gui/SwitchData.hpp>

#include <chrono>

namespace gui
{
    class OptionsData : public gui::SwitchData
    {
      public:
        OptionsData() = default;
        OptionsData(std::chrono::seconds preparation, bool showCounter)
            : preparationTime(preparation), showCounter(showCounter)
        {}

        std::chrono::seconds preparationTime{5};
        bool showCounter = false;
    };
} // namespace gui
