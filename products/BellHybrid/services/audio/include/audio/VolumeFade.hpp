// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Timers/TimerFactory.hpp>
#include <Audio/AudioCommon.hpp>

namespace audio
{
    class VolumeFade
    {
      public:
        using SetCallback = std::function<void(float)>;

        VolumeFade(sys::Service *parent, SetCallback callback);
        ~VolumeFade();

        void Start(float targetVolume, float minVolume, float maxVolume, audio::FadeParams fadeParams);
        void Restart();
        void Stop();
        bool IsActive();

      private:
        enum class State
        {
            Disable,
            FadeIn,
            FadeOut
        };

        audio::FadeParams fadeParams;
        sys::TimerHandle timerHandle;
        float targetVolume;
        float minVolume;
        float maxVolume;
        float currentVolume;
        SetCallback setVolumeCallback;
        State state{State::Disable};
        std::chrono::time_point<std::chrono::system_clock> timestamp;

        void PerformNextFadeStep();
        void TurnUpVolume();
        void TurnDownVolume();
    };

} // namespace audio
