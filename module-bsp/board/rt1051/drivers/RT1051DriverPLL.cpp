/*
 *  @file RT1051DriverPLL.cpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 09.08.19
 *  @brief  
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */



#include "RT1051DriverPLL.hpp"
#include "log/log.hpp"
#include "menums/magic_enum.hpp"
#include "../common/clock_config.h"

namespace drivers {

    using namespace magic_enum;

    RT1051DriverPLL::RT1051DriverPLL(const drivers::PLLInstances &inst, const drivers::DriverPLLParams &params)
            : DriverPLL(params), instance(inst) {

        switch (instance) {
            case PLLInstances::PLL4_Audio: {
                clkPLL4setup(CLK_ENABLE);
            }
                break;
            case PLLInstances::PLL3: {
                clkPLL3setup(CLK_ENABLE);
            }
                break;
            case PLLInstances::PLL3_PFD0: {
                clkPLL3_PFD0setup(CLK_ENABLE);
            }
                break;
            case PLLInstances::PLL3_PFD1: {
                clkPLL3_PFD1setup(CLK_ENABLE);
            }
                break;
            case PLLInstances::PLL3_PFD2: {
                clkPLL3_PFD2setup(CLK_ENABLE);
            }
                break;
            case PLLInstances::PLL2_PFD2:
                clkPLL2_PFD2setup(CLK_ENABLE);
                break;
            case PLLInstances::PLL1_ARM: {
                clkPLL1setup(CLK_ENABLE);
            }
                break;
            case PLLInstances::OSC_24M: {
                //not used
            }
                break;
        }
        LOG_DEBUG("Init %s",std::string(magic_enum::enum_name(instance)).c_str());

    }


    RT1051DriverPLL::~RT1051DriverPLL() {
        switch (instance) {
            case PLLInstances::PLL4_Audio: {
                clkPLL4setup(CLK_DISABLE);
            }
                break;
            case PLLInstances::PLL3: {
                clkPLL3setup(CLK_DISABLE);
            }
                break;
            case PLLInstances::PLL3_PFD0: {
                clkPLL3_PFD0setup(CLK_DISABLE);
            }
                break;
            case PLLInstances::PLL3_PFD1: {
                clkPLL3_PFD1setup(CLK_DISABLE);
            }
                break;
            case PLLInstances::PLL3_PFD2: {
                clkPLL3_PFD2setup(CLK_DISABLE);
            }
                break;
            case PLLInstances::PLL2_PFD2:
                clkPLL2_PFD2setup(CLK_DISABLE);
                break;
            case PLLInstances::PLL1_ARM: {
                clkPLL1setup(CLK_DISABLE);
            }
                break;
            case PLLInstances::OSC_24M: {
                //not used
            }
                break;
        }

        LOG_DEBUG("Deinit %s",std::string(magic_enum::enum_name(instance)).c_str());
    }

}