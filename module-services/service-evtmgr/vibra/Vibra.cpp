// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Vibra.hpp"
#include "SystemManager/Constants.hpp"
#include <module-sys/Timers/TimerFactory.hpp>

namespace vibra_handle
{
    Vibra::Vibra(sys::Service *parent) : parent{parent}
    {
    }

    void Vibra::intPulse(bool repetitive)
    {
        if (repetitive) {
            vibratorTimer =
                sys::TimerFactory::createSingleShotTimer(parent,
                                                         "VibraOneshotTimer",
                                                         std::chrono::milliseconds{bsp::vibrator::defaultVibraPulseMs},
                                                         [this](sys::Timer & /*timer*/) {
                                                             bsp::vibrator::disable();
                                                             vibratorTimerPause.start();
                                                         });
        }
        else {
            vibratorTimer =
                sys::TimerFactory::createSingleShotTimer(parent,
                                                         "VibraOneshotTimer",
                                                         std::chrono::milliseconds{bsp::vibrator::defaultVibraPulseMs},
                                                         [](sys::Timer & /*timer*/) { bsp::vibrator::disable(); });
        }

        bsp::vibrator::enable();
        vibratorTimer.start();
    }

    void Vibra::Pulse()
    {
        intPulse(false);
    }

    void Vibra::PulseRepeat(std::chrono::milliseconds time)
    {
        repetitions = static_cast<int>(time.count()) / (static_cast<int>(bsp::vibrator::defaultVibraPulseMs) +
                                                        static_cast<int>(bsp::vibrator::defaultVibraPauseMs));

        vibratorTimerPause = sys::TimerFactory::createSingleShotTimer(
            parent,
            "VibraPauseTimer",
            std::chrono::milliseconds{bsp::vibrator::defaultVibraPauseMs},
            [this](sys::Timer & /*timer*/) {
                if (repetitions > 0) // call itself for calculated number of repetitions
                {
                    repetitions--;
                    intPulse(true);
                }
            });
        intPulse(true);
    }

    void Vibra::PulseRepeat()
    {
        vibratorTimerPause =
            sys::TimerFactory::createSingleShotTimer(parent,
                                                     "VibraPauseTimer",
                                                     std::chrono::milliseconds{bsp::vibrator::defaultVibraPauseMs},
                                                     [&](sys::Timer & /*timer*/) { intPulse(true); });
        intPulse(true);
    }

    void Vibra::PulseRepeatStop()
    {
        repetitions = 1;
        bsp::vibrator::disable();
        vibratorTimerPause.stop();
        vibratorTimer.stop();
    }

} // namespace vibra_handle
