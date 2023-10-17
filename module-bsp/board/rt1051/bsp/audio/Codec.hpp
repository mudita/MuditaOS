// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#ifndef PUREPHONE_CODEC_HPP
#define PUREPHONE_CODEC_HPP

#include <memory>
#include <optional>

class CodecParams
{
  public:
    enum class SampleRate
    {
        Rate8KHz   = 8000,
        Rate16KHz  = 16000,
        Rate44K1Hz = 44100,
        Rate48KHz  = 48000,
        Rate32KHz  = 32000,
        Rate96KHz  = 96000,
        Invalid
    };

    enum class Cmd
    {
        SetOutVolume,
        SetInGain,
        SetMute,
        SetOutput,
        SetInput,
        Reset,
        MicBiasCtrl,
        None
    };

    static SampleRate ValToSampleRate(uint32_t rate)
    {
        switch (rate) {
        case 8000:
            return SampleRate::Rate8KHz;
        case 16000:
            return SampleRate::Rate16KHz;
        case 32000:
            return SampleRate::Rate32KHz;
        case 44100:
            return SampleRate::Rate44K1Hz;
        case 48000:
            return SampleRate::Rate48KHz;
        case 96000:
            return SampleRate::Rate96KHz;
        default:
            return SampleRate::Invalid;
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

    Cmd opCmd             = Cmd::None;
    float outVolume       = 0;
    float inGain          = 0;
    SampleRate sampleRate = SampleRate::Rate44K1Hz;
};

enum class CodecRetCode
{
    Success,
    InvalidSampleRate,
    InvalidInputPath,
    InvalidOutputPath,
    InvalidArgument
};

class Codec
{
  public:
    virtual ~Codec() = default;

    virtual std::optional<uint32_t> Probe() = 0;

    virtual CodecRetCode Start(const CodecParams &param) = 0;

    virtual CodecRetCode Pause() = 0;

    virtual CodecRetCode Resume() = 0;

    virtual CodecRetCode Stop() = 0;

    virtual CodecRetCode Ioctrl(const CodecParams &param) = 0;
};

#endif // PUREPHONE_CODEC_HPP
