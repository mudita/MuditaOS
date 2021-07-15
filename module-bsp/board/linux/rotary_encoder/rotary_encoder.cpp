// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "bsp/rotary_encoder/rotary_encoder.hpp"

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "timers.h"

#include <sys/stat.h>
#include <fcntl.h>

namespace bsp
{
    namespace rotary_encoder
    {

        int32_t init(xQueueHandle qHandle)
        {
            return 1;
        }

        void deinit()
        {

        }

        bool isPresent(void)
        {
            return true;
        }

        std::optional<bsp::KeyCodes> WorkerEventHandler()
        {
            return std::nullopt;
        }

    } // namespace rotary-encoder
} // namespace bsp
