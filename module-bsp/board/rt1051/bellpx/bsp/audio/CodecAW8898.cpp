// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "CodecAW8898.hpp"
#include "AW8898_regs.hpp"
#include "board/BoardDefinitions.hpp"
#include <log.hpp>
extern "C"
{
#include "FreeRTOS.h"
#include "task.h"
}

using namespace drivers;
using namespace bsp::bell_audio;

inline void FlipBytes(uint16_t *val)
{
    uint16_t tmp = *val << 8;
    *val         = (*val >> 8) | tmp;
}

constexpr auto ReadStatusRetries        = 5;
constexpr auto OneByteAddressing        = 1;
constexpr auto PositiveLogic            = 0;
constexpr auto LogicLow                 = 0;
constexpr auto LogicHigh                = 1;
constexpr auto ReadWriteTwoBytes        = 2;
constexpr auto TwoMilisecondWait        = 2;
constexpr auto TenMilisecondWait        = 10;
constexpr auto OneOnBinaryPositionZero  = 0x0001;
constexpr auto OneOnBinaryPositionNine  = 0x0200;
constexpr auto OneOnBinaryPositionEight = 0x0100;

CodecAW8898::CodecAW8898() : i2cAddr{}
{
    LOG_INFO("Initializing AW8898 audio codec");
    i2cAddr.deviceAddress  = AW8898_I2C_ADDR;
    i2cAddr.subAddressSize = OneByteAddressing; // AW8898 uses 1byte addressing
    i2c                    = DriverI2C::Create(
        static_cast<I2CInstances>(BoardDefinitions::AUDIOCODEC_I2C),
        DriverI2CParams{.baudrate = static_cast<uint32_t>(BoardDefinitions::AUDIOCODEC_I2C_BAUDRATE)});

    gpio = DriverGPIO::Create(static_cast<GPIOInstances>(BoardDefinitions::BELL_AUDIOCODEC_GPIO), DriverGPIOParams{});

    gpio->ConfPin(DriverGPIOPinParams{.dir      = DriverGPIOPinParams::Direction::Output,
                                      .irqMode  = DriverGPIOPinParams::InterruptMode::NoIntmode,
                                      .defLogic = PositiveLogic,
                                      .pin = static_cast<uint32_t>(BoardDefinitions::BELL_AUDIOCODEC_RSTN_PA_PIN)});

    gpio->ConfPin(DriverGPIOPinParams{.dir      = DriverGPIOPinParams::Direction::Input,
                                      .irqMode  = DriverGPIOPinParams::InterruptMode::IntFallingEdge,
                                      .defLogic = PositiveLogic,
                                      .pin = static_cast<uint32_t>(BoardDefinitions::BELL_AUDIOCODEC_INTN_PA_PIN)});

    gpio->ClearPortInterrupts(1 << static_cast<uint32_t>(BoardDefinitions::BELL_AUDIOCODEC_INTN_PA_PIN));
    gpio->DisableInterrupt(1 << static_cast<uint32_t>(BoardDefinitions::BELL_AUDIOCODEC_INTN_PA_PIN));

    gpio->WritePin(static_cast<std::uint32_t>(BoardDefinitions::BELL_AUDIOCODEC_RSTN_PA_PIN), LogicLow); // reset chip
    vTaskDelay(pdMS_TO_TICKS(TwoMilisecondWait));
    gpio->WritePin(static_cast<std::uint32_t>(BoardDefinitions::BELL_AUDIOCODEC_RSTN_PA_PIN), LogicHigh); // clear reset
    vTaskDelay(pdMS_TO_TICKS(TwoMilisecondWait));

    auto ret = Probe();
    LOG_DEBUG("AW8898 Probe: 0x%04lX", ret.value());
}

CodecAW8898::~CodecAW8898()
{
    Reset();
}

CodecRetCode CodecAW8898::Start(const CodecParams &param)
{

    const CodecParamsAW8898 &params = static_cast<const CodecParamsAW8898 &>(param);

    // Software reset - p.19
    i2cAddr.subAddress         = AW8898_REG_ID;
    aw8898_reg_idcode_t dev_id = {.idcode = AW8898_SW_RESET_MAGIC};
    FlipBytes(reinterpret_cast<uint16_t *>(&dev_id));
    i2c->Write(i2cAddr, reinterpret_cast<uint8_t *>(&dev_id), ReadWriteTwoBytes);

    // Power up sequence - table 2, p.18
    /*
        1   Wait for VDD, DVDD supply power up  ;   mode: Power-Down
        2   I2S + Data Path Configuration       ;   mode: Stand-By
        3.1 Enable system (SYSCTRL.PWDN=0)      ;   mode: Configuring
        3,2 Bias, OSC, PLL active
        3.3 Waiting for PLL locked
        4.1 Enable Boost and amplifier
            (SYSCTRL.AMPPD=0)
            Boost and Amplifier boot up         ;   mode: Operating
        4.2 Wait SYSST.SWS=1                    ;   mode: Operating
        5   Release HARD-Mute
            Data Path active                    ;   mode: Operating
    */

    /* 2. I2S:
        I2SCTRL.I2SMD -> mode
        I2SCTRL.I2SSR -> samplerate
        I2SCTRL.I2SBCK -> bit clock BCK frequency = SampleRate * SlotLength * SlotNumber
            SampleRate: Sample rate for this digital audio interface; SlotLength: The length of one audio slot in unit
       of BCK clock; SlotNumber:  How  many  slots  supported  in  this  audio  interface.  For  example:  2-slot
       supported  in  I2S  mode, 4-slot supported in TDM mode.

    */

    aw8898_reg_i2sctrl_t i2s_setup = {
        .inplev = 1, // attenuate input
        .chsel  = 3, // mono; (L+R)/2
        .i2smd  = 0, // standard I2S
        .i2sfs  = 0, // 16 bit
        .i2sbck = 0  // 21*fs(16*2)
    };

    switch (params.sampleRate) {

    case CodecParamsAW8898::SampleRate::Rate8KHz:
        i2s_setup.i2ssr = static_cast<uint8_t>(AW8898_i2ssr::SampleRate8kHz); // 8 kHz
        break;

    case CodecParamsAW8898::SampleRate::Rate16KHz:
        i2s_setup.i2ssr = static_cast<uint8_t>(AW8898_i2ssr::SampleRate16kHz); // 16 kHz
        break;

    case CodecParamsAW8898::SampleRate::Rate44K1Hz:
        i2s_setup.i2ssr = static_cast<uint8_t>(AW8898_i2ssr::SampleRate44k1Hz); // 44.1 kHz
        break;

    case CodecParamsAW8898::SampleRate::Rate48KHz:
        i2s_setup.i2ssr = static_cast<uint8_t>(AW8898_i2ssr::SampleRate48kHz); // 48 kHz
        break;

    case CodecParamsAW8898::SampleRate::Rate32KHz:
        i2s_setup.i2ssr = static_cast<uint8_t>(AW8898_i2ssr::SampleRate32kHz); // 32 kHz
        break;

    case CodecParamsAW8898::SampleRate::Rate96KHz:
        i2s_setup.i2ssr = static_cast<uint8_t>(AW8898_i2ssr::SampleRate96kHz); // 96 kHz
        break;

    default:
        return CodecRetCode::InvalidSampleRate;
    }
    i2cAddr.subAddress = AW8898_REG_I2SCTRL;
    FlipBytes(reinterpret_cast<uint16_t *>(&i2s_setup));
    i2c->Write(i2cAddr, reinterpret_cast<uint8_t *>(&i2s_setup), ReadWriteTwoBytes);

    aw8898_reg_i2stxcfg_t i2s_tx_setup = {
        .fsync_type     = 0, // one slot wide sync pulse
        .slot_num       = 0, // 2 slots
        .i2s_tx_slotvld = 0, // send on slot 0
        .i2s_rx_slotvld = 3, // slot 0,1
        .drvstren       = 0, // I2S_DATAO pad drive 2mA
        .dohz           = 0  // unused channel data set to 0
    };
    i2cAddr.subAddress = AW8898_REG_I2STXCFG;
    FlipBytes(reinterpret_cast<uint16_t *>(&i2s_setup));
    i2c->Write(i2cAddr, reinterpret_cast<uint8_t *>(&i2s_setup), ReadWriteTwoBytes);

    /* 3.1 Enable system (SYSCTRL.PWDN=0) */
    aw8898_reg_sysctrl_t sys_setup;
    i2c->Read(i2cAddr, reinterpret_cast<uint8_t *>(&sys_setup), ReadWriteTwoBytes);
    FlipBytes(reinterpret_cast<uint16_t *>(&sys_setup));
    sys_setup.i2sen    = 1; // enable I2S
    sys_setup.pwdn     = 0; // power up
    sys_setup.rcv_mode = 0; // SPK mode
    sys_setup.amppd    = 1; // amplifier power down
    i2cAddr.subAddress = AW8898_REG_SYSCTRL;
    FlipBytes(reinterpret_cast<uint16_t *>(&sys_setup));
    i2c->Write(i2cAddr, reinterpret_cast<uint8_t *>(&sys_setup), ReadWriteTwoBytes);

    /* 3,2 Bias, OSC, PLL active */
    vTaskDelay(pdMS_TO_TICKS(TenMilisecondWait));

    /* 3.3 Waiting for PLL locked */
    i2cAddr.subAddress  = AW8898_REG_SYSST;
    uint16_t sys_status = 0;

    int retries = ReadStatusRetries;
    while ((sys_status & OneOnBinaryPositionZero) != 1) // wait for PLLS = 1 (locked)
    {
        i2c->Read(i2cAddr, reinterpret_cast<uint8_t *>(&sys_status), ReadWriteTwoBytes);
        FlipBytes(reinterpret_cast<uint16_t *>(&sys_status));
        vTaskDelay(pdMS_TO_TICKS(TwoMilisecondWait));

        if (retries-- == 0) {
            LOG_ERROR("Failed to start codec !");
            return CodecRetCode::InvalidOutputPath; // there isn't suitable fail return code
        }
    }

    /* 4.1 Enable Boost and amplifier
            (SYSCTRL.AMPPD=0)
    */
    i2cAddr.subAddress = AW8898_REG_SYSCTRL;
    i2c->Modify(i2cAddr, OneOnBinaryPositionNine, false, ReadWriteTwoBytes); //(SYSCTRL.AMPPD=0)

    /* 4.2 Wait SYSST.SWS=1 */
    i2cAddr.subAddress = AW8898_REG_SYSST;
    retries            = ReadStatusRetries;
    while ((sys_status & (1 << 8)) == 0) // wait for SWS = 1
    {
        i2c->Read(i2cAddr, reinterpret_cast<uint8_t *>(&sys_status), ReadWriteTwoBytes);
        FlipBytes(reinterpret_cast<uint16_t *>(&sys_status));
        vTaskDelay(pdMS_TO_TICKS(TwoMilisecondWait));

        if (retries-- == 0) {
            LOG_ERROR("Failed to start codec !");
            return CodecRetCode::InvalidOutputPath; // there isn't suitable fail return code
        }
    }

    // Store param configuration
    currentParams = params;

    auto currVol = currentParams.outVolume;

    SetOutputVolume(currVol);

    return CodecRetCode::Success;
}

CodecRetCode CodecAW8898::Pause()
{
    // Turn off device
    i2cAddr.subAddress = AW8898_REG_SYSCTRL;
    i2c->Modify(i2cAddr, OneOnBinaryPositionNine, true, ReadWriteTwoBytes);  //(SYSCTRL.AMPPD=1)
    i2c->Modify(i2cAddr, OneOnBinaryPositionEight, true, ReadWriteTwoBytes); //(SYSCTRL.PWDN=1)

    return CodecRetCode::Success;
}

CodecRetCode CodecAW8898::Resume()
{
    // Turn on device
    i2cAddr.subAddress = AW8898_REG_SYSCTRL;
    i2c->Modify(i2cAddr, OneOnBinaryPositionEight, false, ReadWriteTwoBytes); //(SYSCTRL.PWDN=0)
    /* 3.3 Waiting for PLL locked */
    i2cAddr.subAddress  = AW8898_REG_SYSST;
    uint16_t sys_status = 0;
    while ((sys_status & OneOnBinaryPositionEight) == 0) // wait for PLLS = 1 (locked)
    {
        i2c->Read(i2cAddr, reinterpret_cast<uint8_t *>(&sys_status), ReadWriteTwoBytes);
        FlipBytes(reinterpret_cast<uint16_t *>(&sys_status));
    }
    i2c->Modify(i2cAddr, OneOnBinaryPositionNine, false, ReadWriteTwoBytes); //(SYSCTRL.AMPPD=0)

    return CodecRetCode::Success;
}

CodecRetCode CodecAW8898::Stop()
{
    Pause();
    return CodecRetCode::Success;
}

CodecRetCode CodecAW8898::Ioctrl(const CodecParams &param)
{

    const CodecParamsAW8898 &params = static_cast<const CodecParamsAW8898 &>(param);

    CodecRetCode ret = CodecRetCode::Success;

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
        ret = SetMute(params.muteEnable);
        break;
    default:
        break;
    }

    return ret;
}

CodecRetCode CodecAW8898::SetOutputVolume(const float vol)
{
    uint8_t mute = 0;

    // If volume set to 0 then mute output
    i2cAddr.subAddress = AW8898_REG_PWMCTRL;
    if (vol == 0)
        i2c->Modify(i2cAddr, OneOnBinaryPositionEight, true, ReadWriteTwoBytes); //(PWMCTRL.HMUTE=1) - enable mute
    else
        i2c->Modify(i2cAddr, OneOnBinaryPositionEight, false, ReadWriteTwoBytes); //(PWMCTRL.HMUTE=0) - disable mute

    // volume is encoded with 8 bits. vol is in range 0-10
    i2cAddr.subAddress           = AW8898_REG_HAGCCFG7;
    aw8898_reg_hagccfg7_t regval = {};
    i2c->Read(i2cAddr, reinterpret_cast<uint8_t *>(&regval), ReadWriteTwoBytes);
    FlipBytes(reinterpret_cast<uint16_t *>(&regval));
    regval.volume = static_cast<uint8_t>(25.5 * vol);
    FlipBytes(reinterpret_cast<uint16_t *>(&regval));
    i2c->Write(i2cAddr, reinterpret_cast<uint8_t *>(&regval), ReadWriteTwoBytes);

    currentParams.outVolume = vol;
    return CodecRetCode::Success;
}

CodecRetCode CodecAW8898::Reset()
{
    // Software reset - p.19
    i2cAddr.subAddress         = AW8898_REG_ID;
    aw8898_reg_idcode_t dev_id = {AW8898_SW_RESET_MAGIC};
    FlipBytes(reinterpret_cast<uint16_t *>(&dev_id));
    i2c->Write(i2cAddr, reinterpret_cast<uint8_t *>(&dev_id), ReadWriteTwoBytes);

    return CodecRetCode::Success;
}

CodecRetCode CodecAW8898::SetMute(const bool enable)
{
    i2cAddr.subAddress = AW8898_REG_PWMCTRL;
    i2c->Modify(i2cAddr, OneOnBinaryPositionEight, enable ? true : false, ReadWriteTwoBytes); //(PWMCTRL.HMUTE)
    return CodecRetCode::Success;
}

std::optional<uint32_t> CodecAW8898::Probe()
{
    uint16_t id = 0;

    i2cAddr.subAddress = AW8898_REG_ID;
    i2c->Read(i2cAddr, reinterpret_cast<uint8_t*>(&id), ReadWriteTwoBytes);
    FlipBytes(reinterpret_cast<uint16_t*>(&id));
    return id;
}
