// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "StatusBarWidgetBase.hpp"
#include <time/time_conversion.hpp>
#include <Label.hpp>

namespace gui::status_bar
{
    class Time : public StatusBarWidgetBase<Label>
    {
        std::unique_ptr<utils::time::Timestamp> time;

      public:
        Time(Item *parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h);
        void update();
    };
} // namespace gui::status_bar
