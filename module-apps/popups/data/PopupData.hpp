
// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include <SwitchData.hpp>

namespace gui
{
    class VolumePopupData : public SwitchData
    {
        using AudioContext = std::pair<audio::Profile::Type, audio::PlaybackType>;

      public:
        explicit VolumePopupData(const audio::Volume volume, const AudioContext audioContext)
            : SwitchData(), volume{volume}, audioContext{audioContext}
        {}

        [[nodiscard]] auto getVolume() const noexcept -> audio::Volume
        {
            return volume;
        }

        [[nodiscard]] auto getAudioContext() const noexcept -> AudioContext
        {
            return audioContext;
        }

      private:
        const audio::Volume volume;
        const AudioContext audioContext;
    };
} // namespace gui
