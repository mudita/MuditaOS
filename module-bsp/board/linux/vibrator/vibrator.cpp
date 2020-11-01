// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "bsp/vibrator/vibrator.hpp"

#include <module-utils/log/log.hpp>

namespace bsp
{
    namespace vibrator
    {

        void enable()
        {
            LOG_DEBUG("vibration starts\t\U0001f7e2\U0001f4f3");
        }
        void disable()
        {
            LOG_DEBUG("vibration ends  \t\U0001f6d1\U0001f4f3");
        }
        void init()
        {}
        void deinit()
        {}
        void set(State state)
        {
            state == State::On ? enable() : disable();
        }
    } // namespace vibrator
} // namespace bsp
