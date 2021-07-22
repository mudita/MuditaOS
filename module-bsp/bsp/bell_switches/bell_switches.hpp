#pragma once

#include <bsp/keyboard/key_codes.hpp>

#include <cstdint>
#include <optional>

extern "C"
{
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
}

#include "../common.hpp"

namespace bsp
{

    namespace bell_switches
    {

        int32_t init(xQueueHandle qHandle);
        void deinit(void);

        void enableIRQ();
        void disableIRQ();

        BaseType_t bell_switches_Cent_Right_Latch_IRQHandler(uint32_t mask);
        BaseType_t bell_switches_Left_IRQHandler();
        BaseType_t bell_wakeup_IRQHandler();

        void bell_get_key(const uint8_t &notification, uint8_t &event, uint8_t &code);
    } // namespace bell_switches

} // namespace bsp
