// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "RT1051DriverDMAMux.hpp"
#include <algorithm>

#include <log/log.hpp>

namespace drivers
{

    RT1051DriverDMAMux::RT1051DriverDMAMux(const drivers::DMAMuxInstances &inst,
                                           const drivers::DriverDMAMuxParams &params)
        : DriverDMAMux(params), instance(inst)
    {
        switch (instance) {
        case DMAMuxInstances::DMAMUX0: {
            base = DMAMUX;
            DMAMUX_Init(base);
            LOG_DEBUG("Init: DMAMUX0");
        } break;
        default:
            break;
        }
    }

    RT1051DriverDMAMux::~RT1051DriverDMAMux()
    {
        switch (instance) {
        case DMAMuxInstances::DMAMUX0: {
            for (auto &w : channels) {
                DMAMUX_DisableChannel(base, w);
            }
            DMAMUX_Deinit(DMAMUX);
            LOG_DEBUG("Deinit DMAMUX0");
        } break;
        default:
            break;
        }
    }

    void RT1051DriverDMAMux::Enable(const uint32_t channel, const uint32_t source)
    {
        cpp_freertos::LockGuard lock(mutex);
        if (std::find(std::begin(channels), std::end(channels), channel) == std::end(channels)) {
            if (source != UINT32_MAX) {
                DMAMUX_SetSource(base, channel, (uint8_t)source);
            }

            DMAMUX_EnableChannel(base, channel);
            channels.push_back(channel);
        }
        else {
            LOG_ERROR("Trying to enable channel that is already enabled");
        }
    }

    void RT1051DriverDMAMux::Disable(const uint32_t channel)
    {
        cpp_freertos::LockGuard lock(mutex);
        auto ret = std::find(std::begin(channels), std::end(channels), channel);
        if (ret != std::end(channels)) {
            DMAMUX_DisableChannel(base, channel);
            channels.erase(ret);
        }
        else {
            LOG_ERROR("Trying to disable channel that doesn't exist");
        }
    }

} // namespace drivers
