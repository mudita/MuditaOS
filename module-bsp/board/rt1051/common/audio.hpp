// Copyright (c) 2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "drivers/pll/DriverPLL.hpp"
#include "drivers/dmamux/DriverDMAMux.hpp"
#include "drivers/dma/DriverDMA.hpp"

#include "fsl_sai_edma.h"

#include <memory>

namespace bsp
{
    void audioInit();
    void audioDeinit();

    struct AudioConfig
    {
        sai_config_t config;
        std::uint32_t mclkSourceClockHz;

        std::shared_ptr<drivers::DriverPLL> pllAudio;
        std::shared_ptr<drivers::DriverDMAMux> dmamux;
        std::shared_ptr<drivers::DriverDMA> dma;
        std::unique_ptr<drivers::DriverDMAHandle> rxDMAHandle;
        std::unique_ptr<drivers::DriverDMAHandle> txDMAHandle;

        static AudioConfig *get();
    };
}; // namespace bsp
