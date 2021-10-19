// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <widgets/TimeSetSpinner.hpp>
#include <BoxLayout.hpp>
#include <Label.hpp>

namespace gui
{
    namespace clock_date_widget
    {
        inline constexpr auto h              = 142;
        inline constexpr auto day_h          = 35;
        inline constexpr auto day_top_margin = 8;
    } // namespace clock_date_widget

    class ClockDateWidget : public VBox
    {
      public:
        ClockDateWidget(Item *parent, Position x, Position y, Length w, Length h);
        void setTime(std::time_t time);

      private:
        gui::TimeSetSpinner *clock = nullptr;
        gui::Label *date           = nullptr;
    };
} // namespace gui
