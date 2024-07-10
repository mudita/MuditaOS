// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "VolumeFadeIn.hpp"
#include <log/log.hpp>

namespace audio
{
    namespace
    {
        constexpr auto timerName{"volumeFadeTimer"};
        constexpr std::chrono::milliseconds fadeInterval{300};
        constexpr auto fadeStep{0.1f};
    } // namespace

    VolumeFadeIn::VolumeFadeIn(sys::Service *parent, SetCallback callback) : setVolumeCallback(std::move(callback))
    {
        timerHandle = sys::TimerFactory::createPeriodicTimer(
            parent, timerName, fadeInterval, [this]([[maybe_unused]] sys::Timer &timer) { PerformNextFadeStep(); });
    }

    VolumeFadeIn::~VolumeFadeIn()
    {
        Stop();
    }

    void VolumeFadeIn::Start(float targetVolume, float minVolume, float maxVolume)
    {
        if (targetVolume > maxVolume || minVolume > maxVolume) {
            LOG_ERROR("Incorrect parameters for audio fade in!");
            return;
        }
        this->targetVolume = targetVolume;
        this->minVolume    = minVolume;
        this->maxVolume    = maxVolume;
        currentVolume      = minVolume + fadeStep;
        Restart();
        timerHandle.start();
    }

    void VolumeFadeIn::Restart()
    {
        if (setVolumeCallback != nullptr) {
            setVolumeCallback(currentVolume);
        }
    }

    void VolumeFadeIn::Stop()
    {
        timerHandle.stop();
    }

    bool VolumeFadeIn::IsActive()
    {
        return timerHandle.isActive();
    }

    void VolumeFadeIn::PerformNextFadeStep()
    {
        if (currentVolume < targetVolume) {
            currentVolume = std::clamp(std::min(currentVolume + fadeStep, targetVolume), minVolume, maxVolume);
            if (setVolumeCallback != nullptr) {
                setVolumeCallback(currentVolume);
            }
        }
        else {
            Stop();
        }
    }
} // namespace audio
