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

#include "drivers/dmamux/DriverDMAMux.hpp"
#include "../fsl_drivers/fsl_dmamux.h"

namespace drivers {

    class RT1051DriverDMAMux : public DriverDMAMux {
    public:

        RT1051DriverDMAMux(const DMAMuxInstances &inst, const DriverDMAMuxParams& params);

        ~RT1051DriverDMAMux();

        void Enable() override final;

        void Disable() override final;

    private:
        DMAMuxInstances instance;
        DMAMUX_Type *base;
    };

}


#endif //PUREPHONE_RT1051DRIVERDMAMUX_HPP
