/*
 *  @file RT1051DriverDMA.cpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 09.08.19
 *  @brief  
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */



#include "RT1051DriverDMA.hpp"
#include <algorithm>


namespace drivers {

    RT1051DriverDMA::RT1051DriverDMA(const drivers::DMAInstances &inst, const drivers::DriverDMAParams &params)
            : DriverDMA(params) {
        switch (instance) {
            case DMAInstances::DMA_0: {
                base = DMA0;
                edma_config_t dmaConfig = {0};

                EDMA_GetDefaultConfig(&dmaConfig);
                EDMA_Init(base, &dmaConfig);
            }
                break;
        }
    }

    RT1051DriverDMA::~RT1051DriverDMA() {
        for (auto &w : channelHandles) {
            EDMA_AbortTransfer(&w.second);
        }
        EDMA_Deinit(base);
    }

    int32_t RT1051DriverDMA::CreateHandle(const uint32_t channel) {

        cpp_freertos::LockGuard lock(mutex);

        /* Create EDMA handle */
        /*
         * dmaConfig.enableRoundRobinArbitration = false;
         * dmaConfig.enableHaltOnError = true;
         * dmaConfig.enableContinuousLinkMode = false;
         * dmaConfig.enableDebugMode = false;
         */

        if (channelHandles.find(channel) == channelHandles.end()) {
            EDMA_CreateHandle(&channelHandles[channel], base, channel);
        } else {
            // dma already exists
            LOG_ERROR("DMA channel handle exists already");
        }


    }

    int32_t RT1051DriverDMA::RemoveHandle(const uint32_t channel) {
        cpp_freertos::LockGuard lock(mutex);
        if (channelHandles.find(channel) != channelHandles.end()) {
            EDMA_AbortTransfer(&channelHandles[channel]);
            channelHandles.erase(channel);
        } else {
            // dma already exists
            LOG_ERROR("Trying to remove non-existent dma channel handle");
        }
    }

    void *RT1051DriverDMA::GetHandle(const uint32_t channel) {
        cpp_freertos::LockGuard lock(mutex);
        if (channelHandles.find(channel) != channelHandles.end()) {
            return &channelHandles[channel];
        } else {
            // dma already exists
            LOG_ERROR("DMA channel doesn't exist");
        }
    }

}