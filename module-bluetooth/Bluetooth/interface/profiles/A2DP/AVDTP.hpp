// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "A2DPImpl.hpp"
extern "C"
{
#include <btstack.h>
#include <btstack_defines.h>
#include <classic/avdtp.h>
}
namespace bluetooth
{

    class AVDTP
    {
      public:
        struct SbcConfiguration
        {
            int reconfigure;
            int numChannels;
            int samplingFrequency;
            int channelMode;
            int blockLength;
            int subbands;
            int allocationMethod;
            int minBitpoolValue;
            int maxBitpoolValue;
            int framesPerBuffer;
        };
        static SbcConfiguration sbcConfig;
        static btstack_sbc_encoder_state_t sbcEncoderState;
        static std::array<uint8_t, 4> sbcCodecConfiguration;
        static constexpr int defaultSampleRate = 44100;
        static int sampleRate;

        static void dumpSbcConfiguration();
    };
} // namespace Bt
