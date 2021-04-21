// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "VolumeScaler.hpp"

namespace audio::volume::scaler
{
    Volume toSystemVolume(std::uint8_t avrcpVolume) noexcept
    {
        constexpr auto avrcpMaxVolume = float{0x7F}; // from AVRCP documentation
        const auto systemVolume       = (avrcpVolume / avrcpMaxVolume) * audio::maxVolume;
        // prevents conversion to 0 while in fact sound is not muted
        if (systemVolume > 0.01f && systemVolume < 1.0f) {
            return 1;
        }
        return systemVolume;
    }

    std::uint8_t toAvrcpVolume(float systemVolume) noexcept
    {
        constexpr auto avrcpMaxVolume = std::uint8_t{0x7F}; // from AVRCP documentation
        return std::round(systemVolume * avrcpMaxVolume / audio::maxVolume);
    }
} // namespace audio::volume::scaler
