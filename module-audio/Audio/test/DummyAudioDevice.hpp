// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Audio/Audio.hpp"

class DummyAudioDevice : public audio::AudioDevice
{
  public:
    auto Start() -> RetCode override;
    auto Stop() -> RetCode override;

    auto setOutputVolume(float vol) -> RetCode override;

    auto setInputGain(float gain) -> RetCode override;

    auto getTraits() const -> Traits override;
    auto getSupportedFormats() -> std::vector<audio::AudioFormat> override;
    auto getSourceFormat() -> audio::AudioFormat override;

    // Endpoint control methods
    void onDataSend() override;
    void onDataReceive() override;
    void enableInput() override;
    void enableOutput() override;
    void disableInput() override;
    void disableOutput() override;
};
