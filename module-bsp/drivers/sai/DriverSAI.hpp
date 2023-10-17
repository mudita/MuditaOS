// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#ifndef PUREPHONE_DRIVERSAI_HPP
#define PUREPHONE_DRIVERSAI_HPP

#include <memory>
#include <functional>
#include "drivers/dma/DriverDMA.hpp"
#include "drivers/dmamux/DriverDMAMux.hpp"
#include "drivers/pll/DriverPLL.hpp"

namespace drivers
{

    enum class SAIInstances
    {
        SAI_1,
        SAI_2,
        SAI_3
    };

    struct DriverSAIParams
    {

        enum class Protocol
        {
            LeftJustified,
            RightJusstified,
            I2S,
            PCMA,
            PCMB
        };

        enum class MonoStereo
        {
            Stereo = 0,
            MonoRight,
            MonoLeft
        };

        enum class MasterSlave
        {
            Master,
            Slave
        };

        uint32_t bitWidth;
        uint32_t channel = 0;
        uint32_t sampleRate;
        uint32_t masterClock;
        Protocol protocol;
        MonoStereo monostereo;
        MasterSlave masterslave;
        uint32_t rxDMAChannel;
        uint32_t txDMAChannel;
    };

    struct TransferParams
    {
        uint8_t *data;
        size_t size;
    };

    struct TransferFormat
    {};

    class DriverSAI
    {
      public:
        static std::shared_ptr<DriverSAI> Create(const SAIInstances inst, const DriverSAIParams &params);

        DriverSAI(const DriverSAIParams &params) : parameters(params)
        {}

        virtual ~DriverSAI()
        {}

        virtual int32_t StartOutTransfer(const TransferParams &tx) = 0;

        virtual int32_t StartInTransfer(const TransferParams &rx) = 0;

        virtual int32_t StopOutTransfer() = 0;

        virtual int32_t StopInTransfer() = 0;

        virtual int32_t SetOutTransferEndCallback(std::function<int32_t()> callback) = 0;

        virtual int32_t SetInTransferEndCallback(std::function<int32_t()> callback) = 0;

      protected:
        const DriverSAIParams parameters;
    };

} // namespace drivers

#endif // PUREPHONE_DRIVERSAI_HPP
