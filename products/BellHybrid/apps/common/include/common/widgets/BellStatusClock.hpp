// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
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
            utils::time::Locale::TimeFormat timeFormat = utils::time::Locale::TimeFormat::FormatTime12H);
    };

} // namespace gui
