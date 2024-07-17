// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "VolumeFade.hpp"
#include <log/log.hpp>

namespace audio
{
    namespace
    {
        using namespace std::chrono_literals;
        constexpr auto timerName{"volumeFadeTimer"};
        constexpr auto fadeInterval{33ms};
        constexpr auto fadeStep{0.1f};

        std::chrono::milliseconds calculateTimerPeriod(std::chrono::seconds elapsed,
                                                       std::chrono::seconds duration,
                                                       float currentVolume)
        {
            using floatToMs         = std::chrono::duration<float, std::chrono::milliseconds::period>;
            const auto timeRequired = floatToMs(currentVolume / fadeStep) * fadeInterval.count();
            const auto calculatedTime =
                std::chrono::duration_cast<std::chrono::milliseconds>(duration - elapsed - timeRequired);
            return calculatedTime > std::chrono::milliseconds::zero() ? calculatedTime
                                                                      : std::chrono::milliseconds::zero();
        }
    } // namespace

    VolumeFade::VolumeFade(sys::Service *parent, SetCallback callback) : setVolumeCallback(std::move(callback))
    {
        timerHandle = sys::TimerFactory::createSingleShotTimer(
            parent, timerName, fadeInterval, [this]([[maybe_unused]] sys::Timer &timer) { PerformNextFadeStep(); });
    }

    VolumeFade::~VolumeFade()
    {
        Stop();
    }

    void VolumeFade::Start(float targetVolume, float minVolume, float maxVolume, audio::FadeParams fadeParams)
    {
        if (fadeParams.mode == audio::Fade::Disable) {
            return;
        }
        if (targetVolume > maxVolume || minVolume > maxVolume) {
            LOG_ERROR("Incorrect parameters for audio fade!");
            return;
        }
        this->fadeParams   = fadeParams;
        this->targetVolume = targetVolume;
        this->minVolume    = minVolume;
        this->maxVolume    = maxVolume;
        currentVolume      = minVolume + fadeStep;
        phase              = Phase::FadeIn;
        timestamp          = std::chrono::system_clock::now();

        if (this->fadeParams.playbackDuration.has_value()) {
            // If the song is shorter than the fade in and out durations, we reduce the target volume value so that both
            // phases have time to complete
            const auto maxFadePeriod =
                std::chrono::duration_cast<std::chrono::milliseconds>(fadeParams.playbackDuration.value()) / 2;
            const auto maxTargetVolume = (maxFadePeriod.count() * fadeStep) / fadeInterval.count();
            this->targetVolume         = std::clamp(std::min(targetVolume, maxTargetVolume), minVolume, maxVolume);
        }

        Restart();
        timerHandle.restart(fadeInterval);
    }

    void VolumeFade::Restart()
    {
        if (setVolumeCallback != nullptr) {
            setVolumeCallback(currentVolume);
        }
    }

    void VolumeFade::Stop()
    {
        if (timerHandle.isActive()) {
            timerHandle.stop();
        }
        phase = Phase::Idle;
    }

    void VolumeFade::SetVolume(float volume)
    {
        if (IsFadePhaseActive()) {
            SetTargetVolume(volume);
        }
        else {
            currentVolume = std::clamp(volume, minVolume, maxVolume);
            if (setVolumeCallback != nullptr) {
                setVolumeCallback(currentVolume);
            }
            RestartTimer();
        }
    }

    void VolumeFade::SetTargetVolume(float volume)
    {
        targetVolume = std::clamp(volume, minVolume, maxVolume);
        if (currentVolume > targetVolume) {
            currentVolume = std::clamp(currentVolume, minVolume, targetVolume);
            if (setVolumeCallback != nullptr) {
                setVolumeCallback(currentVolume);
            }
        }
    }

    bool VolumeFade::IsActive()
    {
        return timerHandle.isActive();
    }

    bool VolumeFade::IsFadePhaseActive()
    {
        return (phase == Phase::FadeIn) || (phase == Phase::FadeOut);
    }

    void VolumeFade::PerformNextFadeStep()
    {
        switch (phase) {
        case Phase::FadeIn:
            if (currentVolume < targetVolume) {
                TurnUpVolume();
                timerHandle.restart(fadeInterval);
            }
            else if (fadeParams.mode == audio::Fade::InOut && fadeParams.playbackDuration.has_value()) {
                phase = Phase::Wait;
                RestartTimer();
            }
            else {
                Stop();
            }
            break;
        case Phase::Wait:
            phase = Phase::FadeOut;
            [[fallthrough]];
        case Phase::FadeOut:
            if (currentVolume > 0.0f) {
                TurnDownVolume();
                timerHandle.restart(fadeInterval);
            }
            else {
                Stop();
            }
            break;

        default:
            break;
        }
    }

    void VolumeFade::TurnUpVolume()
    {
        currentVolume = std::clamp(std::min(currentVolume + fadeStep, targetVolume), minVolume, maxVolume);
        if (setVolumeCallback != nullptr) {
            setVolumeCallback(currentVolume);
        }
    }
    void VolumeFade::TurnDownVolume()
    {
        currentVolume = std::clamp(std::max(currentVolume - fadeStep, 0.0f), minVolume, maxVolume);
        if (setVolumeCallback != nullptr) {
            setVolumeCallback(currentVolume);
        }
    }

    void VolumeFade::RestartTimer()
    {
        const auto now         = std::chrono::system_clock::now();
        const auto timeElapsed = std::chrono::duration_cast<std::chrono::seconds>(now - timestamp);
        timerHandle.restart(calculateTimerPeriod(timeElapsed, fadeParams.playbackDuration.value(), currentVolume));
    }

} // namespace audio
