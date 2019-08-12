/*
 *  @file DriverSAI.hpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 09.08.19
 *  @brief  
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */



#ifndef PUREPHONE_DRIVERSAI_HPP
#define PUREPHONE_DRIVERSAI_HPP

#include "../DriverInterface.hpp"

namespace drivers {


    enum class SAIInstances {
        SAI_1,
        SAI_2,
        SAI_3
    };

    struct DriverSAIParams {

        enum class Protocol {
            LeftJustified,
            RightJusstified,
            I2S,
            PCMA,
            PCMB
        };

        enum class MonoStereo {
            Stereo = 0,
            MonoRight,
            MonoLeft
        };

        uint32_t bitWidth;
        uint32_t channel = 0;
        uint32_t sampleRate;
        uint32_t masterClock;
        Protocol protocol;
        MonoStereo monostereo;


    };

    struct TransferParams {
        uint8_t *data;
        size_t size;
    };

    class DriverSAI : public DriverInterface<DriverSAI> {
    public:

        static std::shared_ptr<DriverSAI> Create(const SAIInstances inst, const DriverSAIParams &params);

        DriverSAI(const DriverSAIParams &params) : parameters(params) {}

        virtual ~DriverSAI() {}

        virtual int32_t StartOutTransfer(const TransferParams &tx) = 0;

        virtual int32_t StartInTransfer(const TransferParams &rx) = 0;

        virtual int32_t StopOutTransfer() = 0;

        virtual int32_t StopInTransfer() = 0;

    protected:
        const DriverSAIParams parameters;

    };

}


#endif //PUREPHONE_DRIVERSAI_HPP
