/*
 *  @file RT1051DriverDMAMux.cpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 09.08.19
 *  @brief  
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */



#include "RT1051DriverDMAMux.hpp"
#include <algorithm>


namespace drivers {

    RT1051DriverDMAMux::RT1051DriverDMAMux(const drivers::DMAMuxInstances &inst,
                                           const drivers::DriverDMAMuxParams &params) : DriverDMAMux(params),
                                                                                        instance(inst) {
        switch (instance) {
            case DMAMuxInstances::DMAMUX0: {
                base = DMAMUX;
                DMAMUX_Init(base);
                LOG_DEBUG("Init: DMAMUX0");
            }
                break;
        }
    }

    RT1051DriverDMAMux::~RT1051DriverDMAMux() {
        switch (instance) {
            case DMAMuxInstances::DMAMUX0: {
                for (auto &w : channels) {
                    DMAMUX_DisableChannel(base, w);
                }
                DMAMUX_Deinit(DMAMUX);
                LOG_DEBUG("Deinit DMAMUX0");
            }
                break;
        }
    }

    void RT1051DriverDMAMux::Enable(const uint32_t channel, const uint32_t source) {
        cpp_freertos::LockGuard lock(mutex);
        if (std::find(std::begin(channels), std::end(channels), channel) == std::end(channels)) {
            DMAMUX_SetSource(base, channel,
                             (uint8_t) source);
            DMAMUX_EnableChannel(base, channel);
        } else {
            LOG_ERROR("Trying to enable channel that is already enabled");
        }
    }

    void RT1051DriverDMAMux::Disable(const uint32_t channel) {
        cpp_freertos::LockGuard lock(mutex);
        auto ret = std::find(std::begin(channels), std::end(channels), channel);
        if (ret != std::end(channels)) {
            DMAMUX_DisableChannel(base, channel);
            channels.erase(ret);
        } else {
            LOG_ERROR("Trying to disable channel that doesn't exist");
        }
    }

}