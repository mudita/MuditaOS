/*
 *  @file DriverDMAMux.hpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 09.08.19
 *  @brief  
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */



#ifndef PUREPHONE_DRIVERDMAMUX_HPP
#define PUREPHONE_DRIVERDMAMUX_HPP


#include "../DriverInterface.hpp"

namespace drivers {


    enum class DMAMuxInstances {
        DMAMUX0
    };

    struct DriverDMAMuxParams {

    };

    class DriverDMAMux : public DriverInterface<DriverDMAMux> {
    public:

        static std::shared_ptr<DriverDMAMux> Create(const DMAMuxInstances inst, const DriverDMAMuxParams &params);

        DriverDMAMux(const DriverDMAMuxParams& params): parameters(params) {}

        virtual ~DriverDMAMux() {}

        virtual void Enable() = 0;
        virtual void Disable() = 0;

    protected:
        const DriverDMAMuxParams parameters;

    };

}


#endif //PUREPHONE_DRIVERDMAMUX_HPP
