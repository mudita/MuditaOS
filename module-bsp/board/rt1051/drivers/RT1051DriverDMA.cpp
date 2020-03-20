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
#include "segger/log/log.hpp"

namespace drivers
{

    RT1051DriverDMAHandle::RT1051DriverDMAHandle(const DMA_Type *base,
                                                 const uint32_t channel,
                                                 std::function<void()> callback)
    {
        EDMA_CreateHandle(&handle, const_cast<DMA_Type *>(base), channel);
        if (callback) {
            EDMA_SetCallback(&handle, edmaCallback, this);
        }
    }

    RT1051DriverDMAHandle::~RT1051DriverDMAHandle()
    {
        EDMA_AbortTransfer(&handle);
    }

    RT1051DriverDMA::RT1051DriverDMA(const drivers::DMAInstances &inst, const drivers::DriverDMAParams &params)
        : DriverDMA(params), instance(inst)
    {
        switch (instance) {
        case DMAInstances::DMA_0: {
            base                    = DMA0;
            edma_config_t dmaConfig = {0};

            EDMA_GetDefaultConfig(&dmaConfig);
            EDMA_Init(base, &dmaConfig);
            LOG_DEBUG("Init: DMA_0");
        } break;
        }
    }

    RT1051DriverDMA::~RT1051DriverDMA()
    {
        EDMA_Deinit(base);
        LOG_DEBUG("Deinit: DMA_0");
    }

    std::unique_ptr<DriverDMAHandle> RT1051DriverDMA::CreateHandle(const uint32_t channel,
                                                                   std::function<void()> callback)
    {

        cpp_freertos::LockGuard lock(mutex);

        /* Create EDMA handle */
        /*
         * dmaConfig.enableRoundRobinArbitration = false;
         * dmaConfig.enableHaltOnError = true;
         * dmaConfig.enableContinuousLinkMode = false;
         * dmaConfig.enableDebugMode = false;
         */
        return std::make_unique<RT1051DriverDMAHandle>(base, channel, callback);
    }

    void edmaCallback(struct _edma_handle *handle, void *userData, bool transferDone, uint32_t tcds)
    {
        RT1051DriverDMAHandle *entry = reinterpret_cast<RT1051DriverDMAHandle *>(userData);
        entry->callback();
    }

} // namespace drivers
