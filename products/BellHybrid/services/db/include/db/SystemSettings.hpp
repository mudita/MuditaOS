// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

namespace bell::settings
{
    namespace Temperature
    {
        constexpr inline auto unit = "temperature_unit";
    } // namespace Temperature
    namespace Ringing
    {
        constexpr inline auto duration = "ringing_duration";
        constexpr inline auto tone     = "ringing_tone";
    } // namespace Ringing
    namespace Snooze
    {
        constexpr inline auto active   = "snooze_active";
        constexpr inline auto length   = "snooze_length";
        constexpr inline auto interval = "snooze_interval";
        constexpr inline auto tone     = "snooze_tone";
        constexpr inline auto volume   = "snooze_volume";
    } // namespace Snooze
};    // namespace bell::settings
