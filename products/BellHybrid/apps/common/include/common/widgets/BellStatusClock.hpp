// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/widgets/TimeSetFmtSpinner.hpp>

namespace gui
{
    class BellStatusClock : public TimeSetFmtSpinner
    {
      public:
        explicit BellStatusClock(
            Item *parent                               = nullptr,
            TimeSetSpinner::Size size                  = TimeSetSpinner::Size::SMALL,
            uint32_t x                                 = 0U,
            uint32_t y                                 = 0U,
            uint32_t w                                 = 0U,
            uint32_t h                                 = 0U,
            utils::time::Locale::TimeFormat timeFormat = utils::time::Locale::TimeFormat::FormatTime12H);
    };

} // namespace gui
