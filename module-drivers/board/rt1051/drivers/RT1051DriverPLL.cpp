/*
 *  @file RT1051DriverPLL.cpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 09.08.19
 *  @brief  
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */



#include "RT1051DriverPLL.hpp"
#include "../fsl_drivers/fsl_clock.h"
#include "log/log.hpp"

namespace drivers{

    RT1051DriverPLL::RT1051DriverPLL(const drivers::PLLInstances &inst, const drivers::DriverPLLParams &params) : DriverPLL(params),instance(inst){

        switch(instance){
            case PLLInstances ::Audio:
            {
                /*
                 * AUDIO PLL setting: Frequency = Fref * (DIV_SELECT + NUM / DENOM)
                 *                              = 24 * (32 + 77/100)
                 *                              = 786.48 MHz
                 */
                const clock_audio_pll_config_t audioPllConfig = {
                        .loopDivider = 32,  /* PLL loop divider. Valid range for DIV_SELECT divider value: 27~54. */
                        .postDivider = 1,   /* Divider after the PLL, should only be 1, 2, 4, 8, 16. */
                        .numerator = 77,    /* 30 bit numerator of fractional loop divider. */
                        .denominator = 100, /* 30 bit denominator of fractional loop divider */
                };
                CLOCK_InitAudioPll(&audioPllConfig);
                LOG_DEBUG("Init Audio PLL: %lu",CLOCK_GetPllFreq(kCLOCK_PllAudio));
            }
                break;
            case PLLInstances ::Ethernet:
            case PLLInstances ::Video:
            case PLLInstances ::USB_1:
            case PLLInstances ::USB_2:
                // not used
                break;
        }
    }


    RT1051DriverPLL::~RT1051DriverPLL() {
        switch(instance){
            case PLLInstances ::Audio:
            {
                CLOCK_DeinitAudioPll();
                LOG_DEBUG("Deinit Audio PLL");
            }
                break;
            case PLLInstances ::Ethernet:
            case PLLInstances ::Video:
                // not used
                break;
        }
    }

}