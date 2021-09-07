// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include "AudioCommon.hpp"

/// @brief Converts volume between system and bluetooth ranges.
namespace audio::volume::scaler
{
    namespace a2dp
    {
        /// @brief Takes volume level and converts it to according one for the system.
        /// @param avrcpVolume - AVRCP volume level.
        /// @return Volume level scaled to satisfy system's range [audio::minVolume, audio::maxVolume].
        Volume toSystemVolume(std::uint8_t avrcpVolume) noexcept;
        /// @brief Takes volume level and converts it to according one for the AVRCP.
        /// @param systemVolume - system volume level.
        /// @return Volume level scaled to satisfy AVRCP's range [0, 127].
        std::uint8_t toAvrcpVolume(float systemVolume) noexcept;

    } // namespace a2dp
    namespace hsp
    {
        /// @brief Takes volume level and converts it to according one for the system.
        /// @param hspSpeakerGain - HSP speaker gain.
        /// @return Volume level scaled to satisfy system's range [audio::minVolume, audio::maxVolume].
        Volume toSystemVolume(std::uint8_t hspSpeakerGain) noexcept;
        /// @brief Takes volume level and converts it to according one for the HSP speaker gain.
        /// @param systemVolume - system volume level.
        /// @return Volume level scaled to satisfy HSP's range [0, 15].
        std::uint8_t toHSPGain(float systemVolume) noexcept;
    } // namespace hsp
} // namespace audio::volume::scaler
