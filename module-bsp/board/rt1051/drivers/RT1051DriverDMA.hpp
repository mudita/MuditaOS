// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#ifndef PUREPHONE_RT1051DRIVERDMA_HPP
#define PUREPHONE_RT1051DRIVERDMA_HPP

#include "drivers/dma/DriverDMA.hpp"
#include "../fsl_drivers/fsl_edma.h"
#include <map>
#include "mutex.hpp"

namespace drivers
{

    void edmaCallback(struct _edma_handle *handle, void *userData, bool transferDone, uint32_t tcds);

    class RT1051DriverDMAHandle : public DriverDMAHandle
    {
        friend void edmaCallback(struct _edma_handle *handle, void *userData, bool transferDone, uint32_t tcds);

      public:
        RT1051DriverDMAHandle(const DMA_Type *base, const uint32_t channel, std::function<void()> callback);
        ~RT1051DriverDMAHandle();

        void *GetHandle()
        {
            return &handle;
        }

      private:
        edma_handle_t handle;
        std::function<void()> callback;
    };

    class RT1051DriverDMA : public DriverDMA
    {

        friend void edmaCallback(struct _edma_handle *handle, void *userData, bool transferDone, uint32_t tcds);

      public:
        RT1051DriverDMA(const DMAInstances &inst, const DriverDMAParams &params);

        ~RT1051DriverDMA();

        std::unique_ptr<DriverDMAHandle> CreateHandle(const uint32_t channel,
                                                      std::function<void()> callback = nullptr) override final;

      private:
        cpp_freertos::MutexStandard mutex;
        DMAInstances instance;
        DMA_Type *base;
    };

} // namespace drivers

#endif // PUREPHONE_RT1051DRIVERDMA_HPP
