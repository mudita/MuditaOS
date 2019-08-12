/*
 *  @file RT1051DriverSAI.hpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 09.08.19
 *  @brief  
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */



#ifndef PUREPHONE_RT1051DRIVERSAI_HPP
#define PUREPHONE_RT1051DRIVERSAI_HPP

#include "drivers/sai/DriverSAI.hpp"

namespace drivers {
    class RT1051DriverSAI : public DriverSAI {

        RT1051DriverSAI(const SAIInstances &inst, const DriverSAIParams &params);

        ~RT1051DriverSAI();

        int32_t StartOutTransfer(const TransferParams &tx) override final;

        int32_t StartInTransfer(const TransferParams &rx) override final;

    };
}


#endif //PUREPHONE_RT1051DRIVERSAI_HPP
