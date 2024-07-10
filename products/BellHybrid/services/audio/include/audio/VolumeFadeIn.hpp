// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Timers/TimerFactory.hpp>
#include <Audio/AudioCommon.hpp>

namespace audio
{
    class VolumeFadeIn
    {
      public:
        using SetCallback = std::function<void(float)>;

        VolumeFadeIn(sys::Service *parent, SetCallback callback);
        ~VolumeFadeIn();

        void Start(float targetVolume, float minVolume, float maxVolume);
        void Restart();
        void Stop();
        bool IsActive();

      private:
        sys::TimerHandle timerHandle;
        float targetVolume;
        float minVolume;
        float maxVolume;
        float currentVolume;
        SetCallback setVolumeCallback;

        void PerformNextFadeStep();
    };
} // namespace audio
