#pragma once

#include <stdint.h>
#include <module-bsp/bsp/keyboard/key_codes.hpp>

extern "C" {
	#include "FreeRTOS.h"
	#include "task.h"
	#include "queue.h"
}

#include "../common.hpp"
#include "../../../../../../../usr/include/c++/10.2.0/utility"

namespace bsp {

namespace magnetometer{

    int32_t init(xQueueHandle qHandle);

    bool isPresent(void);

    bsp::Board GetBoard(void);

    bool setPowerState(const uint8_t pwr_value);

    struct Measurements {
        int16_t X;
        int16_t Y;
        int16_t Z; // Z is useless
        float tempC;
    } typedef Measurements;

    /// returns a pair of <new_data_read?, values>
    std::pair<bool, Measurements> getMeasurements();

    bsp::KeyCodes getDiscrete(const Measurements &measurements);

    BaseType_t IRQHandler();
    void enableIRQ();
}

}
