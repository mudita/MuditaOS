// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AVDTP.hpp"

namespace bluetooth
{
    AVDTP::SbcConfiguration AVDTP::sbcConfig;
    btstack_sbc_encoder_state_t AVDTP::sbcEncoderState;
    std::array<uint8_t, 4> AVDTP::sbcCodecConfiguration;
    int AVDTP::sampleRate = AVDTP::defaultSampleRate;

    void AVDTP::dumpSbcConfiguration()
    {
        LOG_INFO("Received media codec configuration:");
        LOG_INFO("    - numChannels: %d", sbcConfig.numChannels);
        LOG_INFO("    - samplingFrequency: %d", sbcConfig.samplingFrequency);
        LOG_INFO("    - channelMode: %d", sbcConfig.channelMode);
        LOG_INFO("    - blockLength: %d", sbcConfig.blockLength);
        LOG_INFO("    - subbands: %d", sbcConfig.subbands);
        LOG_INFO("    - allocationMethod: %d", sbcConfig.allocationMethod);
        LOG_INFO("    - bitpool_value [%d, %d] ", sbcConfig.minBitpoolValue, sbcConfig.maxBitpoolValue);
    }
} // namespace bluetooth
