// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#ifndef PUREPHONE_RT1051DRIVERDMAMUX_HPP
#define PUREPHONE_RT1051DRIVERDMAMUX_HPP

#include <vector>
#include "drivers/dmamux/DriverDMAMux.hpp"
#include "../fsl_drivers/fsl_dmamux.h"
#include "mutex.hpp"

namespace drivers
{

    class RT1051DriverDMAMux : public DriverDMAMux
    {
      public:
        RT1051DriverDMAMux(const DMAMuxInstances &inst, const DriverDMAMuxParams &params);

        ~RT1051DriverDMAMux();

        void Enable(const uint32_t channel, const uint32_t source = UINT32_MAX) override final;

        void Disable(const uint32_t channel) override final;

      private:
        cpp_freertos::MutexStandard mutex;
        DMAMuxInstances instance;
        DMAMUX_Type *base;
        std::vector<uint32_t> channels;
    };

} // namespace drivers

#endif // PUREPHONE_RT1051DRIVERDMAMUX_HPP
