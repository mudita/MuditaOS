// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#ifndef PUREPHONE_DRIVERDMAMUX_HPP
#define PUREPHONE_DRIVERDMAMUX_HPP

#include <memory>

namespace drivers
{

    enum class DMAMuxInstances
    {
        DMAMUX0,
        COUNT
    };

    struct DriverDMAMuxParams
    {};

    class DriverDMAMux
    {
      public:
        static std::shared_ptr<DriverDMAMux> Create(const DMAMuxInstances inst, const DriverDMAMuxParams &params);

        DriverDMAMux(const DriverDMAMuxParams &params) : parameters(params)
        {}

        virtual ~DriverDMAMux()
        {}

        virtual void Enable(const uint32_t channel, const uint32_t source = UINT32_MAX) = 0;
        virtual void Disable(const uint32_t channel)                                    = 0;

      protected:
        const DriverDMAMuxParams parameters;

      private:
        static std::weak_ptr<DriverDMAMux> singleton[static_cast<uint32_t>(DMAMuxInstances ::COUNT)];
    };

} // namespace drivers

#endif // PUREPHONE_DRIVERDMAMUX_HPP
