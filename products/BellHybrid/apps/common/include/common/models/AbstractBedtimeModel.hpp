// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "common/models/AbstractSettingsModel.hpp"
#include <time.h>

namespace app
{
    class AbstractBedtimeModel
    {
      public:
        virtual ~AbstractBedtimeModel()                                       = default;
        virtual auto getBedtimeOnOff() -> gui::AbstractSettingsModel<bool> &  = 0;
        virtual auto getBedtimeTime() -> gui::AbstractSettingsModel<time_t> & = 0;
    };
} // namespace app
