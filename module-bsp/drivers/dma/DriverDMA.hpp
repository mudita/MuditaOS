/*
 *  @file DriverDMA.hpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 09.08.19
 *  @brief
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */

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
