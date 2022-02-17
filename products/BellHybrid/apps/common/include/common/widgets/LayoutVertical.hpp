// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "layouts/BaseHomeScreenLayoutProvider.hpp"

#include <apps-common/widgets/BellBaseLayout.hpp>

namespace style::homescreen_vertical
{
    constexpr inline auto side_box_w   = 180U;
    constexpr inline auto center_box_w = 240U;
} // namespace style::homescreen_vertical

namespace gui
{
    class BellBaseLayout;
    class TextFixedSize;
    class AlarmIcon;
    class AlarmSetSpinner;
    class TimeSetSpinnerVertical;
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
        TimeSetSpinnerVertical *time{};
        VBox *leftBox{};
        VBox *rightBox{};

        // Set Alarm Screen
        AlarmIcon *alarmTopIcon{};
        TimeSetFmtSpinner *setAlarmFmtSpinner{};
    };
}; // namespace gui
