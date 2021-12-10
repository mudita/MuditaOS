
// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include <SwitchData.hpp>
#include <module-audio/Audio/AudioCommon.hpp>

namespace gui
{
    class VolumePopupData : public SwitchData
    {
      public:
        explicit VolumePopupData(const audio::Volume volume, const audio::Context audioContext)
            : SwitchData(), volume{volume}, audioContext{audioContext}
        {}

        [[nodiscard]] auto getVolume() const noexcept -> audio::Volume
        {
            return volume;
        }

        [[nodiscard]] auto getAudioContext() const noexcept -> audio::Context
        {
            return audioContext;
        }

      private:
        const audio::Volume volume;
        const audio::Context audioContext;
    };

    class ModesPopupData : public SwitchData
    {
      public:
        explicit ModesPopupData(sys::phone_modes::PhoneMode phoneMode, bool flightMode)
            : SwitchData(), phoneMode{phoneMode}, isFlightModeEnabled(flightMode)
        {}

        [[nodiscard]] auto getPhoneMode() const noexcept -> sys::phone_modes::PhoneMode
        {
            return phoneMode;
        }

        [[nodiscard]] auto getFlightMode() const noexcept -> bool
        {
            return isFlightModeEnabled;
        }

      private:
        const sys::phone_modes::PhoneMode phoneMode;
        const bool isFlightModeEnabled;
    };
} // namespace gui
