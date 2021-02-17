// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "VibraService.hpp"
#include "service-evtmgr/BatteryMessages.hpp"
#include "SystemManager/Constants.hpp"
#include <Service/Timer.hpp>

#include <common_data/EventStore.hpp>

namespace service_vibra
{
    VibraService::VibraService(sys::Service *parent)
    {
        vibratorTimerOneshot =
            std::make_unique<sys::Timer>("VibraOneshotTimer", parent, bsp::vibrator::default_vibra_pulse);
        vibratorTimerPause =
            std::make_unique<sys::Timer>("VibraPauseTimer", parent, bsp::vibrator::default_vibra_pause);

        vibratorTimerOneshot->setInterval(bsp::vibrator::default_vibra_pulse);
        vibratorTimerPause->setInterval(bsp::vibrator::default_vibra_pause);
    }

    VibraService::~VibraService()
    {}

    void VibraService::intPulse(bool repetitive)
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

    void VibraService::Pulse()
    {
        intPulse(false);
    }

    void VibraService::PulseRepeat(sys::ms time)
    {
        _repetitions = static_cast<int>(time) / (static_cast<int>(bsp::vibrator::default_vibra_pulse) +
                                                 static_cast<int>(bsp::vibrator::default_vibra_pause));

        vibratorTimerPause->connect([&](sys::Timer &) {
            if ((_repetitions) && (_repetitions--)) /// call itself for calculated number of repetitions
            {
                intPulse(true);
            }
        });
        intPulse(true);
    }

    void VibraService::PulseRepeatStop()
    {
        _repetitions = 1;
        bsp::vibrator::disable();
        vibratorTimerPause->stop();
        vibratorTimerOneshot->stop();
    }
    bool VibraService::checkState()
    {
        if (_repetitions > 1) {
            return true;
        }
        else {
            return false;
        }
    }

} // namespace service_vibra
