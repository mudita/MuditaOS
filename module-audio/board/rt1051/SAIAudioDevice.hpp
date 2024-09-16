// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include <Audio/AudioDevice.hpp>
#include <Audio/Stream.hpp>

#include "fsl_sai_edma.h"

namespace audio
{

    class SAIAudioDevice : public AudioDevice
    {
      public:
        SAIAudioDevice(I2S_Type *base, sai_edma_handle_t *rxHandle, sai_edma_handle_t *txHandle);

        void onDataSend() override;
        void onDataReceive() override;
        void enableInput() override;
        void enableOutput() override;
        void disableInput() override;
        void disableOutput() override;

      protected:
        void initiateRxTransfer();
        void initiateTxTransfer();
        RetCode setOutputVolume(float vol) override;
        void scaleOutputVolume(audio::Stream::Span &span);

        I2S_Type *_base;
        sai_edma_handle_t *rx = nullptr;
        sai_edma_handle_t *tx = nullptr;
        bool txEnabled        = false;
        bool rxEnabled        = false;
        float volumeFactor{1.0};
    };

} // namespace audio
