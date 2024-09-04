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
        void SetVolume(float volume);
        bool IsActive();

      private:
        enum class Phase
        {
            Idle,
            FadeIn,
            Wait,
            FadeOut
        };

        audio::FadeParams fadeParams;
        sys::TimerHandle timerHandle;
        float targetVolume;
        float minVolume;
        float maxVolume;
        float currentVolume;
        std::chrono::milliseconds fadeInterval;
        SetCallback setVolumeCallback;
        Phase phase{Phase::Idle};
        std::chrono::time_point<std::chrono::system_clock> timestamp;

        void PerformNextFadeStep();
        void RestartWaitingTimer();
        void TurnUpVolume();
        void TurnDownVolume();
        void SetTargetVolume(float volume);
        bool IsFadePhaseActive();
        std::chrono::milliseconds calculateFadeInterval(std::chrono::seconds maxDuration);
        std::chrono::milliseconds calculateWaitingTimerPeriod(std::chrono::seconds timeElapsed);
    };
} // namespace audio
