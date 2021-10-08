// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#ifndef PUREPHONE_RT1051DRIVERSAI_HPP
#define PUREPHONE_RT1051DRIVERSAI_HPP

#include "drivers/sai/DriverSAI.hpp"

#include <functional>
#include <magic_enum.hpp>
#include "../fsl_drivers/fsl_sai.h"
#include "../fsl_drivers/fsl_sai_edma.h"

namespace drivers
{

    void txAudioCallback(I2S_Type *base, sai_edma_handle_t *handle, status_t status, void *userData);

    void rxAudioCallback(I2S_Type *base, sai_edma_handle_t *handle, status_t status, void *userData);

    class RT1051DriverSAI : public DriverSAI
    {

      public:
        friend void txAudioCallback(I2S_Type *base, sai_edma_handle_t *handle, status_t status, void *userData);

        friend void rxAudioCallback(I2S_Type *base, sai_edma_handle_t *handle, status_t status, void *userData);

        RT1051DriverSAI(const SAIInstances &inst, const DriverSAIParams &params);

        ~RT1051DriverSAI();

        int32_t StartOutTransfer(const TransferParams &tx) override final;

        int32_t StartInTransfer(const TransferParams &rx) override final;

        int32_t StopOutTransfer() override final;

        int32_t StopInTransfer() override final;

        int32_t SetOutTransferEndCallback(std::function<int32_t()> callback) override final;

        int32_t SetInTransferEndCallback(std::function<int32_t()> callback) override final;

      private:
        enum class TransferState
        {
            HalfTransfer,
            FullTransfer
        };

        I2S_Type *base = nullptr;
        SAIInstances instances;

        // Order here is important
        std::shared_ptr<drivers::DriverDMA> dma;
        std::shared_ptr<drivers::DriverDMAMux> dmamux;
        std::shared_ptr<drivers::DriverPLL> pll;

        std::function<int32_t()> rxTransferEndCallback;
        std::function<int32_t()> txTransferEndCallback;

        static AT_NONCACHEABLE_SECTION_INIT(sai_edma_handle_t txHandle[magic_enum::enum_count<SAIInstances>()]);
        static AT_NONCACHEABLE_SECTION_INIT(sai_edma_handle_t rxHandle[magic_enum::enum_count<SAIInstances>()]);

        TransferState txTransferState = TransferState::HalfTransfer;
        TransferState rxTransferState = TransferState::HalfTransfer;

        TransferParams txParams;
        TransferParams rxParams;

        uint32_t txDMASource = 0;
        uint32_t rxDMASource = 0;
    };
} // namespace drivers

#endif // PUREPHONE_RT1051DRIVERSAI_HPP
