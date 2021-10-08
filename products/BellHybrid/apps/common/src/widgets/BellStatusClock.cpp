// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <common/widgets/BellStatusClock.hpp>

#include <common/data/StyleCommon.hpp>

namespace gui
{

    BellStatusClock::BellStatusClock(
        Item *parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h, utils::time::Locale::TimeFormat timeFormat)
        : TimeSetFmtSpinner{parent, x, y, w, h, timeFormat}
    {
        setFont(bell_style::statusClockFont);
        setEditMode(EditMode::Browse);
        activeItem = false;
        setTimeFormatSpinnerVisibility(true);
    }

} // namespace gui
