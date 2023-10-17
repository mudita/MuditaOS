#pragma once

#include <hal/key_input/KeyEventDefinitions.hpp>

#include <cstdint>
#include <optional>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "../common.hpp"

namespace bsp::magnetometer
{
    /// unit: 4 LSB/Gauss
    struct Measurements
    {
        std::int16_t X;
        std::int16_t Y;
        std::int16_t Z;
    };

    std::int32_t init(xQueueHandle qHandle);
    void deinit();

    bool isPresent();

    /// returns new data readout if it is available
    std::optional<Measurements> getMeasurement();

    bsp::KeyCodes parse(const Measurements &measurements);
    void resetCurrentParsedValue();

    std::optional<bsp::KeyCodes> WorkerEventHandler();
}
