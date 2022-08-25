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

        /// returns new data readout if it is available
        std::optional<Measurements> getMeasurement();

        bsp::KeyCodes parse(const Measurements &measurements);
        std::optional<bsp::KeyCodes> WorkerEventHandler();
        void resetCurrentParsedValue();
    } // namespace magnetometer
} // namespace bsp
