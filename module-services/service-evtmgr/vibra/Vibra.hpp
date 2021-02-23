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

namespace vibra_handle
{
    class Vibra
    {
      public:
        explicit Vibra(sys::Service *parent);
        ~Vibra() = default;

        /// According to design : 04.06 — Apps Tools.png there is only ON/OFF setting for vibra.
        /// But there is also number of sound repetitions so we may assume that vibra should be
        /// set the same way.
        void Pulse();
        /// but we also want to repeat pulses during eg. full length of ringtone
        void PulseRepeat(sys::ms time);
        void PulseRepeat(); /// infinite repeat ... until stop summoned
        void PulseRepeatStop();

      private:
        std::unique_ptr<sys::Timer> vibratorTimerOneshot;
        std::unique_ptr<sys::Timer> vibratorTimerPause;

        int repetitions = 1;

        void intPulse(bool repetitive);
        std::function<void(sys::Timer &)> nullTimerCallback{};
    };
} // namespace vibra_handle
