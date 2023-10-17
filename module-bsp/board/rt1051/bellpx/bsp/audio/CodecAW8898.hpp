// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "AW8898driver.hpp"

#include <bsp/audio/Codec.hpp>

extern "C"
{
#include "fsl_common.h"
}

#include <cstdint>

class CodecParamsAW8898 : public CodecParams
{
  public:
    enum class MonoStereo
    {
        Left,
        Right,
        Mono
    };

    bsp::audio::AW8898::aw_i2s_channel MonoStereoToCodecChsel() const
    {
        switch (monoStereo) {
        case MonoStereo::Left:
            return bsp::audio::AW8898::aw_i2s_channel::CHSEL_LEFT;
        case MonoStereo::Right:
            return bsp::audio::AW8898::aw_i2s_channel::CHSEL_RIGHT;
        default:
            return bsp::audio::AW8898::aw_i2s_channel::CHSEL_MONO;
        }
    }

    bsp::audio::AW8898::aw_i2s_frequency SampleRateToCodecFreq() const
    {
        switch (sampleRate) {
        case SampleRate::Rate8KHz:
            return bsp::audio::AW8898::aw_i2s_frequency::FREQUENCY_08K;
        case SampleRate::Rate16KHz:
            return bsp::audio::AW8898::aw_i2s_frequency::FREQUENCY_16K;
        case SampleRate::Rate32KHz:
            return bsp::audio::AW8898::aw_i2s_frequency::FREQUENCY_32K;
        case SampleRate::Rate44K1Hz:
            return bsp::audio::AW8898::aw_i2s_frequency::FREQUENCY_44K;
        case SampleRate::Rate48KHz:
            return bsp::audio::AW8898::aw_i2s_frequency::FREQUENCY_48K;
        default:
            return bsp::audio::AW8898::aw_i2s_frequency::FREQUENCY_44K;
        }
    }

    uint32_t GetSampleRateVal()
    {
        switch (sampleRate) {
        case SampleRate::Rate8KHz:
            return 8000;
        case SampleRate::Rate16KHz:
            return 16000;
        case SampleRate::Rate32KHz:
            return 32000;
        case SampleRate::Rate44K1Hz:
            return 44100;
        case SampleRate::Rate48KHz:
            return 48000;
        case SampleRate::Rate96KHz:
            return 96000;
        default:
            return 0;
        }
    }

    MonoStereo monoStereo = MonoStereo::Mono;
};

class CodecAW8898 : public Codec
{
  public:
    CodecAW8898();
    ~CodecAW8898();

    std::optional<uint32_t> Probe() override final;

    CodecRetCode Start(const CodecParams &param) override final;

    CodecRetCode Pause() override final;

    CodecRetCode Resume() override final;

    CodecRetCode Stop() override final;

    CodecRetCode Ioctrl(const CodecParams &param) override final;

  private:
    CodecParamsAW8898 currentParams;

    std::uint8_t VolumeTo8Bit(const float vol);
    CodecRetCode SetOutputVolume(const float vol);
    CodecRetCode SetMute(const bool enable);
    CodecRetCode Reset();
};
