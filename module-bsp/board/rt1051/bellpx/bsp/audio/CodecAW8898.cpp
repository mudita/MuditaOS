// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "CodecAW8898.hpp"
#include "AW8898regs.hpp"
#include "AW8898driver.hpp"
#include "board/BoardDefinitions.hpp"

#include <log/log.hpp>

extern "C"
{
#include "FreeRTOS.h"
#include "task.h"
}

#include <cstdint>

using namespace drivers;
using namespace bsp::audio;

namespace
{
    constexpr auto ReadStatusRetries = 5;
    constexpr auto OneByteAddressing = 1;
    constexpr auto PositiveLogic     = 0;
    constexpr auto maxInVolume       = 10;
    constexpr auto minInVolume       = 0;

    /// Higher layers operate using 0-10 range. Here we are transforming it into more usable one.
    /// Anything above 9 is going to distort the speaker and values below 5 are too quiet.
    constexpr float transformVolumeLvl(float value)
    {
        constexpr auto maxOutVolume = 9;
        constexpr auto minOutVolume = 5;
        constexpr auto inputRange   = std::make_pair(minInVolume, maxInVolume);
        constexpr auto outputRange  = std::make_pair(minOutVolume, maxOutVolume);
        constexpr float slope = 1.0 * (outputRange.second - outputRange.first) / (inputRange.second - inputRange.first);
        return outputRange.first + slope * (value - inputRange.first);
    }
} // namespace

CodecAW8898::CodecAW8898()
{
    LOG_INFO("Initializing AW8898 audio codec");
    AW8898::i2cAddr.deviceAddress  = AW8898_I2C_ADDR;
    AW8898::i2cAddr.subAddressSize = OneByteAddressing; // AW8898 uses 1byte addressing
    AW8898::i2c                    = DriverI2C::Create(
        static_cast<I2CInstances>(BoardDefinitions::AUDIOCODEC_I2C),
        DriverI2CParams{.baudrate = static_cast<uint32_t>(BoardDefinitions::AUDIOCODEC_I2C_BAUDRATE)});

    AW8898::gpio =
        DriverGPIO::Create(static_cast<GPIOInstances>(BoardDefinitions::BELL_AUDIOCODEC_GPIO), DriverGPIOParams{});

    AW8898::gpio->ConfPin(
        DriverGPIOPinParams{.dir      = DriverGPIOPinParams::Direction::Output,
                            .irqMode  = DriverGPIOPinParams::InterruptMode::NoIntmode,
                            .defLogic = PositiveLogic,
                            .pin      = static_cast<uint32_t>(BoardDefinitions::BELL_AUDIOCODEC_RSTN_PA_PIN)});

    AW8898::gpio->ConfPin(
        DriverGPIOPinParams{.dir      = DriverGPIOPinParams::Direction::Input,
                            .irqMode  = DriverGPIOPinParams::InterruptMode::IntFallingEdge,
                            .defLogic = PositiveLogic,
                            .pin      = static_cast<uint32_t>(BoardDefinitions::BELL_AUDIOCODEC_INTN_PA_PIN)});

    AW8898::gpio->ClearPortInterrupts(1 << static_cast<uint32_t>(BoardDefinitions::BELL_AUDIOCODEC_INTN_PA_PIN));
    AW8898::gpio->DisableInterrupt(1 << static_cast<uint32_t>(BoardDefinitions::BELL_AUDIOCODEC_INTN_PA_PIN));

    AW8898::gpio->WritePin(static_cast<std::uint32_t>(BoardDefinitions::BELL_AUDIOCODEC_RSTN_PA_PIN),
                           AW8898::LogicLow); // reset chip
    vTaskDelay(pdMS_TO_TICKS(AW8898::OperationWaitTimeMS));
    AW8898::gpio->WritePin(static_cast<std::uint32_t>(BoardDefinitions::BELL_AUDIOCODEC_RSTN_PA_PIN),
                           AW8898::LogicHigh); // clear reset
    vTaskDelay(pdMS_TO_TICKS(AW8898::OperationWaitTimeMS));

    LOG_DEBUG("Probing AW8898 ...");
    auto ret = Probe();
    LOG_DEBUG("AW8898 Probe: 0x%04lX", ret.value());
}

CodecAW8898::~CodecAW8898()
{
    Reset();
}

CodecRetCode CodecAW8898::Start(const CodecParams &param)
{
    CodecParamsAW8898 params;
    params.opCmd      = static_cast<CodecParamsAW8898::Cmd>(param.opCmd);
    params.outVolume  = param.outVolume;
    params.sampleRate = static_cast<CodecParamsAW8898::SampleRate>(param.sampleRate);

    AW8898::Init(params.MonoStereoToCodecChsel(), params.SampleRateToCodecFreq());

    // Store param configuration
    currentParams = params;

    SetOutputVolume(currentParams.outVolume);

    AW8898::ReadAllReg();

    return CodecRetCode::Success;
}

CodecRetCode CodecAW8898::Pause()
{
    AW8898::Stop();

    return CodecRetCode::Success;
}

CodecRetCode CodecAW8898::Resume()
{
    AW8898::Start();

    return CodecRetCode::Success;
}

CodecRetCode CodecAW8898::Stop()
{
    AW8898::Stop();
    return CodecRetCode::Success;
}

CodecRetCode CodecAW8898::Ioctrl(const CodecParams &param)
{
    const CodecParamsAW8898 &params = static_cast<const CodecParamsAW8898 &>(param);
    CodecRetCode ret                = CodecRetCode::Success;

    switch (params.opCmd) {
    case CodecParamsAW8898::Cmd::SetOutVolume:
        ret = SetOutputVolume(params.outVolume);
        break;

    case CodecParamsAW8898::Cmd::SetInGain:
        ret = CodecRetCode::Success;
        break;
    case CodecParamsAW8898::Cmd::SetInput:
        ret = CodecRetCode::Success;
        break;
    case CodecParamsAW8898::Cmd::SetOutput:
        ret = CodecRetCode::Success;
        break;
    case CodecParamsAW8898::Cmd::MicBiasCtrl:
        ret = CodecRetCode::Success;
        break;
    case CodecParamsAW8898::Cmd::Reset:
        ret = Reset();
        break;
    case CodecParamsAW8898::Cmd::SetMute:
        ret = SetMute(true);
        break;
    default:
        break;
    }

    return ret;
}

std::uint8_t CodecAW8898::VolumeTo8Bit(const float vol)
{
    static constexpr auto conversionRatio = static_cast<float>(UINT8_MAX) / maxInVolume;

    if (vol < 0) {
        return 0;
    }

    if (vol > maxInVolume) {
        return UINT8_MAX;
    }

    return static_cast<uint8_t>(conversionRatio * vol);
}

CodecRetCode CodecAW8898::SetOutputVolume(const float vol)
{
    // If volume set to 0 then mute output
    AW8898::RunMute(vol == 0); //(PWMCTRL.HMUTE=0) - disable mute
    // AW8898 has "negative" gain control - higher the absolute value, lower the volume
    AW8898::SetVolume(UINT8_MAX - VolumeTo8Bit(transformVolumeLvl(vol)));
    currentParams.outVolume = vol;
    return CodecRetCode::Success;
}

CodecRetCode CodecAW8898::Reset()
{
    AW8898::HwReset();
    return CodecRetCode::Success;
}

CodecRetCode CodecAW8898::SetMute(const bool enable)
{
    AW8898::RunMute(enable);
    return CodecRetCode::Success;
}

std::optional<uint32_t> CodecAW8898::Probe()
{
    if (AW8898::ReadChipid() == kStatus_Success) {
        return AW8898::AW8898_CHIP_ID;
    }
    else {
        return std::nullopt;
    }
}
