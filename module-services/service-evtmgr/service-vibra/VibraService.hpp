// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Service/Service.hpp>
#include <bsp/vibrator/vibrator.hpp>

namespace sys
{
    class Timer;
    class Service;
} // namespace sys

namespace service_vibra
{
    class VibraService
    {
      public:
        explicit VibraService(sys::Service *parent);
        ~VibraService();

        /// According to design : 04.06 — Apps Tools.png there is only ON/OFF setting for vibra.
        /// But there is also number of sound repetitions so we may assume that vibra should be
        /// set the same way.
        void Pulse();
        /// but we also want to repeat pulses during eg. full length of ringtone
        void PulseRepeat(sys::ms time);
        void PulseRepeatStop();
        bool checkState();

        /// to use, send message:
        /// eg. vibrate 10 seconds:
        /// bus.sendUnicast(std::make_shared<sevm::VibraMessage>(bsp::vibrator::Action::pulseRepeat, 10000),
        /// service::name::evt_manager); vibrate once:
        /// bus.sendUnicast(std::make_shared<sevm::VibraMessage>(bsp::vibrator::Action::pulse),
        /// service::name::evt_manager); stop vibrating:
        /// bus.sendUnicast(std::make_shared<sevm::VibraMessage>(bsp::vibrator::Action::stop),
        /// service::name::evt_manager);

      private:
        std::unique_ptr<sys::Timer> vibratorTimerOneshot;
        std::unique_ptr<sys::Timer> vibratorTimerPause;

        int _repetitions = 1;

        void intPulse(bool repetitive);
    };
} // namespace service_vibra
