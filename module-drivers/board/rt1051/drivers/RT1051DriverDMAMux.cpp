/*
 *  @file RT1051DriverDMAMux.cpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 09.08.19
 *  @brief  
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */



#include "RT1051DriverDMAMux.hpp"




namespace drivers {

    RT1051DriverDMAMux::RT1051DriverDMAMux(const drivers::DMAMuxInstances &inst,
                                           const drivers::DriverDMAMuxParams &params) : DriverDMAMux(params),
                                                                                        instance(inst){
        switch (instance) {
            case DMAMuxInstances::DMAMUX0: {
                base = DMAMUX;
                DMAMUX_Init(base);
            }
                break;
        }
    }

    RT1051DriverDMAMux::~RT1051DriverDMAMux() {
        switch (instance) {
            case DMAMuxInstances::DMAMUX0: {
                Disable();
                DMAMUX_Deinit(DMAMUX);
            }
                break;
        }
    }

    void RT1051DriverDMAMux::Enable() {
            if(parameters.rxChannel != UINT32_MAX){
                DMAMUX_SetSource(base, parameters.rxChannel,
                                 (uint8_t) parameters.rxSource);
                DMAMUX_EnableChannel(base, parameters.rxChannel);


            }

            if(parameters.txChannel != UINT32_MAX){
                DMAMUX_SetSource(base, parameters.txChannel,
                                 (uint8_t) parameters.txSource);
                DMAMUX_EnableChannel(base, parameters.txChannel);
            }

    }

    void RT1051DriverDMAMux::Disable() {
        DMAMUX_DisableChannel(base,parameters.txChannel);
        DMAMUX_DisableChannel(base,parameters.rxChannel);
    }

}