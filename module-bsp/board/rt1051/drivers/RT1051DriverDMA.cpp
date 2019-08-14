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
#include "log/log.hpp"


namespace drivers {

    RT1051DriverDMA::RT1051DriverDMA(const drivers::DMAInstances &inst, const drivers::DriverDMAParams &params)
            : DriverDMA(params),instance(inst) {
        switch (instance) {
            case DMAInstances::DMA_0: {
                base = DMA0;
                edma_config_t dmaConfig = {0};

                EDMA_GetDefaultConfig(&dmaConfig);
                EDMA_Init(base, &dmaConfig);
                LOG_DEBUG("Init: DMA0");
            }
                break;
        }
    }

    RT1051DriverDMA::~RT1051DriverDMA() {
        for (auto &w : channelHandles) {
            EDMA_AbortTransfer(&w.second.handle);
        }
        EDMA_Deinit(base);
        LOG_DEBUG("Deinit: DMA0");
    }

    int32_t RT1051DriverDMA::CreateHandle(const uint32_t channel,std::function<void()> callback) {

        cpp_freertos::LockGuard lock(mutex);

        /* Create EDMA handle */
        /*
         * dmaConfig.enableRoundRobinArbitration = false;
         * dmaConfig.enableHaltOnError = true;
         * dmaConfig.enableContinuousLinkMode = false;
         * dmaConfig.enableDebugMode = false;
         */

        if (channelHandles.find(channel) == channelHandles.end()) {
            EDMA_CreateHandle(&channelHandles[channel].handle, base, channel);
            if(callback){
                EDMA_SetCallback(&channelHandles[channel].handle, edmaCallback, &channelHandles[channel]);
                channelHandles[channel].callback = callback;
            }
        } else {
            // dma already exists
            LOG_ERROR("DMA channel handle exists already");
        }


    }

    int32_t RT1051DriverDMA::RemoveHandle(const uint32_t channel) {
        cpp_freertos::LockGuard lock(mutex);
        if (channelHandles.find(channel) != channelHandles.end()) {
            EDMA_AbortTransfer(&channelHandles[channel].handle);
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

    void edmaCallback(struct _edma_handle *handle, void *userData, bool transferDone, uint32_t tcds){
        RT1051DriverDMA::DMAEntry* entry = reinterpret_cast<RT1051DriverDMA::DMAEntry*>(userData);
        entry->callback();
    }


}