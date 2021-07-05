// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "VolumeScaler.hpp"

namespace audio::volume::scaler
{
    namespace
    {
        /// @brief Takes volume level from bluetooth profile and converts it to according one for the system.
        /// @param profileVolume - Bluetooth profile volume level.
        /// @param profileMaxVolume - Bluetooth profile max volume level.
        /// @return Volume level scaled to satisfy system's range [audio::minVolume, audio::maxVolume].
        Volume btProfileToSystemVolume(std::uint8_t profileVolume, float profileMaxVolume) noexcept
        {
            const auto systemVolume = (profileVolume / profileMaxVolume) * audio::maxVolume;
            // prevents conversion to 0 while in fact sound is not muted
            if (systemVolume > 0.01f && systemVolume < 1.0f) {
                return 1;
            }
            return systemVolume;
        }

        /// @brief Takes volume level and converts it to according one for the bluetooth profile.
        /// @param systemVolume - system volume level.
        /// @param profileMaxVolume - bluetooth profile max volume level.
        /// @return Volume level scaled to satisfy bluetooth profile range [0, profileMaxVolume].
        std::uint8_t systemToBtProfileVolume(float systemVolume, std::uint8_t profileMaxVolume) noexcept
        {
            return std::round(systemVolume * profileMaxVolume / audio::maxVolume);
        }
    } // namespace
    namespace a2dp
    {
        constexpr auto avrcpMaxVolume = std::uint8_t{0x7F}; // from AVRCP documentation

        Volume toSystemVolume(std::uint8_t avrcpVolume) noexcept
        {
            return btProfileToSystemVolume(avrcpVolume, static_cast<float>(avrcpMaxVolume));
        }

        std::uint8_t toAvrcpVolume(float systemVolume) noexcept
        {
            return systemToBtProfileVolume(systemVolume, avrcpMaxVolume);
        }
    } // namespace a2dp
    namespace hsp
    {
        constexpr auto hspMaxVolume = float{0x0F}; // from HSP documentation

        Volume toSystemVolume(std::uint8_t hspSpeakerGain) noexcept
        {
            return btProfileToSystemVolume(hspSpeakerGain, static_cast<float>(hspMaxVolume));
        }
        std::uint8_t toHSPGain(float systemVolume) noexcept
        {
            return systemToBtProfileVolume(systemVolume, hspMaxVolume);
        }
    } // namespace hsp
} // namespace audio::volume::scaler
