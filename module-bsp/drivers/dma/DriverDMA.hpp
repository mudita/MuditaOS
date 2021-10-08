// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#ifndef PUREPHONE_DRIVERDMA_HPP
#define PUREPHONE_DRIVERDMA_HPP

#include <functional>
#include <memory>

namespace drivers
{

    enum class DMAInstances
    {
        DMA_0,
        COUNT
    };

    struct DriverDMAParams
    {};

    class DriverDMAHandle
    {
      public:
        virtual void *GetHandle() = 0;
    };

    class DriverDMA
    {
      public:
        static std::shared_ptr<DriverDMA> Create(const DMAInstances inst, const DriverDMAParams &params);

        DriverDMA(const DriverDMAParams &params) : parameters(params)
        {}

        virtual ~DriverDMA()
        {}

        virtual std::unique_ptr<DriverDMAHandle> CreateHandle(const uint32_t channel,
                                                              std::function<void()> callback = nullptr) = 0;

      protected:
        const DriverDMAParams parameters;

      private:
        static std::weak_ptr<DriverDMA> singleton[static_cast<uint32_t>(DMAInstances ::COUNT)];
    };

} // namespace drivers

#endif // PUREPHONE_DRIVERDMA_HPP
