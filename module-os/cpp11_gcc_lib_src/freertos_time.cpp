/// @file
///
/// @author: Piotr Grygorczuk grygorek@gmail.com
///
/// @copyright Copyright 2019 Piotr Grygorczuk
/// All rights reserved.
///
/// Redistribution and use in source and binary forms, with or without
/// modification, are permitted provided that the following conditions are met:
///
/// o Redistributions of source code must retain the above copyright notice,
///   this list of conditions and the following disclaimer.
///
/// o Redistributions in binary form must reproduce the above copyright notice,
///   this list of conditions and the following disclaimer in the documentation
///   and/or other materials provided with the distribution.
///
/// o My name may not be used to endorse or promote products derived from this
///   software without specific prior written permission.
///
/// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
/// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
/// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
/// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
/// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
/// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
/// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
/// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
/// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
/// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
/// POSSIBILITY OF SUCH DAMAGE.
///
#include "freertos_time.h"
#include "FreeRTOS.h"
#include "critical_section.h"
#include "task.h"
#include <chrono>
#include <sys/time.h>

namespace free_rtos_std
{

    class wall_clock
    {
      public:
        struct time_data
        {
            timeval offset;
            TickType_t ticks;
        };

        static time_data time()
        { // atomic
            critical_section critical;
            return time_data{_timeOffset, xTaskGetTickCount()};
        }

        static void time(const timeval &time)
        { // atomic
            critical_section critical;
            _timeOffset = time;
        }

      private:
        static timeval _timeOffset;
    };

    timeval wall_clock::_timeOffset;

} // namespace free_rtos_std

using namespace std::chrono;
void SetSystemClockTime(const time_point<system_clock, system_clock::duration> &time)
{
    auto delta{time - time_point<system_clock>(milliseconds(pdTICKS_TO_MS(xTaskGetTickCount())))};
    long long sec{duration_cast<seconds>(delta).count()};
    long usec = duration_cast<microseconds>(delta).count() - sec * 1000000; // narrowing type

    free_rtos_std::wall_clock::time({sec, usec});
}

static timeval operator+(const timeval &l, const timeval &r)
{
    timeval t{l.tv_sec + r.tv_sec, l.tv_usec + r.tv_usec};

    if (t.tv_usec >= 1000000)
    {
        t.tv_sec++;
        t.tv_usec -= 1000000;
    }
    return t;
}

extern "C" int _gettimeofday(timeval *tv, void *tzvp)
{
    (void)tzvp;

    auto t{free_rtos_std::wall_clock::time()};

    long long ms{pdTICKS_TO_MS(t.ticks)};
    long long sec{ms / 1000};
    long usec = (ms - sec * 1000) * 1000; // narrowing type

    *tv = t.offset + timeval{sec, usec};

    return 0; // return non-zero for error
}
