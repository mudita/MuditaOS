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
#include "menums/magic_enum.hpp"
#include "../fsl_drivers/fsl_sai.h"
#include "../fsl_drivers/fsl_sai_edma.h"

namespace drivers {
    class RT1051DriverSAI : public DriverSAI {

    public:

        RT1051DriverSAI(const SAIInstances &inst, const DriverSAIParams &params);

        ~RT1051DriverSAI();

        int32_t StartOutTransfer(const TransferParams &tx) override final;

        int32_t StartInTransfer(const TransferParams &rx) override final;

        int32_t StopOutTransfer() override final;

        int32_t StopInTransfer() override final;

    private:
        I2S_Type *base;
        SAIInstances instances;

        static AT_NONCACHEABLE_SECTION_INIT(sai_edma_handle_t txHandle[magic_enum::enum_count<SAIInstances >()]);
        static AT_NONCACHEABLE_SECTION_INIT(sai_edma_handle_t rxHandle[magic_enum::enum_count<SAIInstances >()]);

    };
}


#endif //PUREPHONE_RT1051DRIVERSAI_HPP
