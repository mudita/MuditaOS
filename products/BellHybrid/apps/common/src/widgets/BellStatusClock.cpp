// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include <common/widgets/BellStatusClock.hpp>
#include <common/data/StyleCommon.hpp>

namespace gui
{
    BellStatusClock::BellStatusClock(Item *parent) : TimeSetFmtSpinner{parent}
    {
        setFont(bell_style::font_status_clock);
        setEditMode(EditMode::Browse);
        activeItem = false;
        setTimeFormatSpinnerVisibility(true);
    }
} // namespace gui
