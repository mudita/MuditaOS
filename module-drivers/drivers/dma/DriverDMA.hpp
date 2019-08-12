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


#include "../DriverInterface.hpp"

namespace drivers {


    enum class DMAInstances {
        DMA_0
    };

    struct DriverDMAParams {

    };

    class DriverDMA : public DriverInterface<DriverDMA> {
    public:

        static std::shared_ptr<DriverDMA> Create(const DMAInstances inst, const DriverDMAParams &params);

        DriverDMA(const DriverDMAParams &params) : parameters(params) {}

        virtual ~DriverDMA() {}

        virtual int32_t CreateHandle(const uint32_t channel) = 0;

        virtual int32_t RemoveHandle(const uint32_t channel) = 0;

        virtual void* GetHandle(const uint32_t channel) = 0;

    protected:
        const DriverDMAParams parameters;

    };

}


#endif //PUREPHONE_DRIVERDMA_HPP
