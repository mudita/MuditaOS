// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Vibra.hpp"
#include "SystemManager/Constants.hpp"
#include <Service/Timer.hpp>

#include <common_data/EventStore.hpp>

namespace vibra_handle
{
    Vibra::Vibra(sys::Service *parent)
    {
        vibratorTimerOneshot =
            std::make_unique<sys::Timer>("VibraOneshotTimer", parent, bsp::vibrator::defaultVibraPulseMs);
        vibratorTimerPause =
            std::make_unique<sys::Timer>("VibraPauseTimer", parent, bsp::vibrator::defaultVibraPauseMs);

        vibratorTimerOneshot->setInterval(bsp::vibrator::defaultVibraPulseMs);
        vibratorTimerPause->setInterval(bsp::vibrator::defaultVibraPauseMs);
    }

    void Vibra::intPulse(bool repetitive)
    {
        if (repetitive) {
            vibratorTimerOneshot->connect([&](sys::Timer &) {
                bsp::vibrator::disable();
                vibratorTimerPause->start();
            });
        }
        else {
            vibratorTimerOneshot->connect([&](sys::Timer &) { bsp::vibrator::disable(); });
        }
        bsp::vibrator::enable();
        vibratorTimerOneshot->start();
    }

    void Vibra::Pulse()
    {
        intPulse(false);
    }

    void Vibra::PulseRepeat(sys::ms time)
    {
        repetitions = static_cast<int>(time) / (static_cast<int>(bsp::vibrator::defaultVibraPulseMs) +
                                                static_cast<int>(bsp::vibrator::defaultVibraPauseMs));

        vibratorTimerPause->connect([&](sys::Timer &) {
            if (repetitions) /// call itself for calculated number of repetitions
            {
                repetitions--;
                intPulse(true);
            }
        });
        intPulse(true);
    }

    void Vibra::PulseRepeat()
    {
        vibratorTimerPause->connect([&](sys::Timer &) { intPulse(true); });
        intPulse(true);
    }

    void Vibra::PulseRepeatStop()
    {
        repetitions = 1;
        bsp::vibrator::disable();
        vibratorTimerPause->stop();
        vibratorTimerOneshot->stop();
    }

} // namespace vibra_handle
