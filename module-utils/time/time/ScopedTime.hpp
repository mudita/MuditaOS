// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <FreeRTOS.h>
#include <log.hpp>
#include <string>
#include <ticks.hpp>

namespace utils
{
    namespace time
    {
        class Scoped
        {
#if DEBUG_SCOPED_TIMINGS == 1
            TickType_t timestamp;
#endif
            std::string text;

          public:
            /// scoped timer usage:
            /// auto time = Scoped("lol");
            /// will log how much time elapsed in form 'lol time: 123'
            /// auto time is important - othervise item will be created and removed instantly
            Scoped(const std::string &text)
            {
#if DEBUG_SCOPED_TIMINGS == 1
                this->text = text;
                timestamp  = cpp_freertos::Ticks::GetTicks();
#endif
            }
            ~Scoped()
            {
#if DEBUG_SCOPED_TIMINGS == 1
                LOG_DEBUG("%s time: %" PRIu32, text.c_str(), cpp_freertos::Ticks::GetTicks() - timestamp);
#endif
            }
        };

    }; // namespace time
};     // namespace utils
