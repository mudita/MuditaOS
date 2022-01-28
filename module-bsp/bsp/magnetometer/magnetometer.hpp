#pragma once

#include <hal/key_input/KeyEventDefinitions.hpp>

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

    namespace magnetometer
    {

        int32_t init(xQueueHandle qHandle);
        void deinit();

        bool isPresent(void);

        /// unit: 4 LSB/Gauss
        struct Measurements
        {
            int16_t X;
            int16_t Y;
            int16_t Z;
        };

        /// returns a pair of <new_data_read?, values>
        std::pair<bool, Measurements> getMeasurement();

        bsp::KeyCodes parse(const Measurements &measurements);
        std::optional<bsp::KeyCodes> WorkerEventHandler();
        void resetCurrentParsedValue();

        BaseType_t IRQHandler();
        void enableIRQ();
    } // namespace magnetometer

} // namespace bsp
