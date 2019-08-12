/*
 *  @file RT1051DriverDMAMux.hpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 09.08.19
 *  @brief  
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */



#ifndef PUREPHONE_RT1051DRIVERDMAMUX_HPP
#define PUREPHONE_RT1051DRIVERDMAMUX_HPP

#include <vector>
#include "drivers/dmamux/DriverDMAMux.hpp"
#include "../fsl_drivers/fsl_dmamux.h"
#include "mutex.hpp"

namespace drivers {

    class RT1051DriverDMAMux : public DriverDMAMux {
    public:

        RT1051DriverDMAMux(const DMAMuxInstances &inst, const DriverDMAMuxParams& params);

        ~RT1051DriverDMAMux();

        void Enable(const uint32_t channel,const uint32_t source) override final;

        void Disable(const uint32_t channel) override final;

    private:
        cpp_freertos::MutexStandard mutex;
        DMAMuxInstances instance;
        DMAMUX_Type *base;
        std::vector<uint32_t > channels;
    };

}


#endif //PUREPHONE_RT1051DRIVERDMAMUX_HPP
