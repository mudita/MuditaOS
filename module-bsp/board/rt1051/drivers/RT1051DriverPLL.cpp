// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "RT1051DriverPLL.hpp"
#include <log/log.hpp>
#include "board/clock_config.h"

namespace drivers
{

    RT1051DriverPLL::RT1051DriverPLL(const drivers::PLLInstances &inst, const drivers::DriverPLLParams &params)
        : DriverPLL(params), instance(inst)
    {

        switch (instance) {
        case PLLInstances::PLL4_Audio: {
            clkPLL4setup(CLK_ENABLE);
            name = "PLL4_Audio";
        } break;
        case PLLInstances::PLL3: {
            clkPLL3setup(CLK_ENABLE);
            name = "PLL3";
        } break;
        case PLLInstances::PLL3_PFD0: {
            clkPLL3_PFD0setup(CLK_ENABLE);
            name = "PLL3_PFD0";
        } break;
        case PLLInstances::PLL3_PFD1: {
            clkPLL3_PFD1setup(CLK_ENABLE);
            name = "PLL3_PFD1";
        } break;
        case PLLInstances::PLL3_PFD2: {
            clkPLL3_PFD2setup(CLK_ENABLE);
            name = "PLL3_PFD2";
        } break;
        case PLLInstances::PLL2_PFD2:
            clkPLL2_PFD2setup(CLK_ENABLE);
            name = "PLL2_PFD2";
            break;
        case PLLInstances::PLL1_ARM: {
            clkPLL1setup(CLK_ENABLE);
            name = "PLL1_ARM";
        } break;
        case PLLInstances::OSC_24M: {
            // not used
        } break;
        default:
            break;
        }
        LOG_DEBUG("Init: %s", name.c_str());
    }

    RT1051DriverPLL::~RT1051DriverPLL()
    {
        switch (instance) {
        case PLLInstances::PLL4_Audio: {
            clkPLL4setup(CLK_DISABLE);
        } break;
        case PLLInstances::PLL3: {
            clkPLL3setup(CLK_DISABLE);
        } break;
        case PLLInstances::PLL3_PFD0: {
            clkPLL3_PFD0setup(CLK_DISABLE);
        } break;
        case PLLInstances::PLL3_PFD1: {
            clkPLL3_PFD1setup(CLK_DISABLE);
        } break;
        case PLLInstances::PLL3_PFD2: {
            clkPLL3_PFD2setup(CLK_DISABLE);
        } break;
        case PLLInstances::PLL2_PFD2:
            clkPLL2_PFD2setup(CLK_DISABLE);
            break;
        case PLLInstances::PLL1_ARM: {
            clkPLL1setup(CLK_DISABLE);
        } break;
        case PLLInstances::OSC_24M: {
            // not used
        } break;
        default:
            break;
        }

        LOG_DEBUG("Deinit: %s", name.c_str());
    }

} // namespace drivers
