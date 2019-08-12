/*
 *  @file RT1051DriverSAI.cpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 09.08.19
 *  @brief  
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */



#include "RT1051DriverSAI.hpp"
#include "bsp/BoardDefinitions.hpp"


#include "../fsl_drivers/fsl_sai_edma.h"



namespace drivers{

    using namespace magic_enum;

     sai_edma_handle_t RT1051DriverSAI::txHandle[magic_enum::enum_count<SAIInstances >()] = {};
     sai_edma_handle_t RT1051DriverSAI::rxHandle[magic_enum::enum_count<SAIInstances >()] = {};

    RT1051DriverSAI::RT1051DriverSAI(const drivers::SAIInstances &inst, const drivers::DriverSAIParams &params) : DriverSAI(params) {
            switch(instances){
                case SAIInstances ::SAI_1:
                    base = SAI1;
                    break;
                case SAIInstances ::SAI_2:
                    base = SAI2;
                    break;
                case SAIInstances ::SAI_3:
                    base = SAI3;
                    break;
            }
    }

    RT1051DriverSAI::~RT1051DriverSAI() {

    }

    int32_t RT1051DriverSAI::StartInTransfer(const drivers::TransferParams &rx) {
        sai_transfer_t xfer = {0};

        /* Reset SAI Rx internal logic */
        SAI_RxSoftwareReset(base, kSAI_ResetTypeSoftware);

        xfer.data = rx.data;
        xfer.dataSize = rx.size;
        SAI_TransferReceiveEDMA(base, &rxHandle[enum_integer(instances)], &xfer);
    }

    int32_t RT1051DriverSAI::StopInTransfer() {
        SAI_RxDisableInterrupts(base, kSAI_FIFOErrorInterruptEnable);
        if(rxHandle[enum_integer(instances)].dmaHandle){
            SAI_TransferAbortReceiveEDMA(base,&rxHandle[enum_integer(instances)]);
        }

    }

    int32_t RT1051DriverSAI::StartOutTransfer(const drivers::TransferParams &tx) {
        sai_transfer_t xfer = {0};

        /* Reset SAI Tx internal logic */
        SAI_TxSoftwareReset(base, kSAI_ResetTypeSoftware);

        xfer.data = tx.data;
        xfer.dataSize = tx.size;
        SAI_TransferSendEDMA(base, &txHandle[enum_integer(instances)], &xfer);
    }

    int32_t RT1051DriverSAI::StopOutTransfer() {
        SAI_TxDisableInterrupts(base, kSAI_FIFOErrorInterruptEnable);
        if(txHandle[enum_integer(instances)].dmaHandle){
            SAI_TransferTerminateSendEDMA(base,&txHandle[enum_integer(instances)]);
        }

    }
}