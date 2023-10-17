// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "common/models/AbstractSettingsModel.hpp"
#include <utf8/UTF8.hpp>
#include <time.h>
#include <stdint.h>
#include <string>

namespace app
{
    class AbstractBedtimeModel
    {
      public:
        virtual ~AbstractBedtimeModel()                                          = default;
        virtual auto getBedtimeOnOff() -> gui::AbstractSettingsModel<bool> &     = 0;
        virtual auto getBedtimeTime() -> gui::AbstractSettingsModel<time_t> &    = 0;
        virtual auto getBedtimeTone() -> gui::AbstractSettingsModel<UTF8> &      = 0;
        virtual auto getBedtimeVolume() -> gui::AbstractSettingsModel<uint8_t> & = 0;
    };
} // namespace app
