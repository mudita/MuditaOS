// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "layouts/BaseHomeScreenLayoutProvider.hpp"

#include <apps-common/widgets/BellBaseLayout.hpp>

namespace style::homescreen_vertical
{
    constexpr inline auto side_box_w        = 115U;
    constexpr inline auto center_box_w      = 250U;
    constexpr inline auto center_box_h      = 220U;
    constexpr inline auto digit_box_w       = 125U;
    constexpr inline auto leftMargin        = 60U;
    constexpr inline auto rightMargin       = 60U;
    constexpr inline auto topNegativeMargin = -35;
} // namespace style::homescreen_vertical

namespace gui
{
    class BellBaseLayout;
    class TextFixedSize;
    class AlarmIcon;
    class AlarmSetSpinner;
    class ClockVertical;
    class TimeSetFmtSpinner;
    class SnoozeTimer;
    class BellBattery;
    class DuoHBox;
    class Icon;

    class LayoutVertical : public HBox
    {
      public:
        LayoutVertical(
            Item *parent, const uint32_t &x = 0, const uint32_t &y = 0, const uint32_t &w = 0, const uint32_t &h = 0);

        HBox *mainScreen{};
        BellBaseLayout *setAlarmScreen{};
        Icon *alarmActivatedDeactivatedScreen{};
        AlarmIcon *alarmMainIcon{};
        BellBattery *battery{};

        // Main Screen
        ClockVertical *time{};
        VBox *leftBox{};
        VBox *rightBox{};

        // Set Alarm Screen
        AlarmIcon *alarmTopIcon{};
        TimeSetFmtSpinner *setAlarmFmtSpinner{};
    };
}; // namespace gui
