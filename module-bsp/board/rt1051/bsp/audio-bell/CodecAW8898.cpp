// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "CodecAW8898.hpp"
#include "AW8898_regs.hpp"
#include "bsp/BoardDefinitions.hpp"
#include "qfilter.h"

using namespace drivers;

CodecAW8898::CodecAW8898() : i2cAddr{}
{

    i2cAddr.deviceAddress  = AW8898_I2C_ADDR;
    i2cAddr.subAddressSize = 1; // AW8898 uses 1byte addressing
    i2c                    = DriverI2C::Create(
        static_cast<I2CInstances>(BoardDefinitions::AUDIOCODEC_I2C),
        DriverI2CParams{.baudrate = static_cast<uint32_t>(BoardDefinitions::AUDIOCODEC_I2C_BAUDRATE)});
    Reset();
}

CodecAW8898::~CodecAW8898()
{
    Reset();
}

CodecRetCode CodecAW8898::Start(const CodecParams &param)
{

    const CodecParamsAW8898 &params = static_cast<const CodecParamsAW8898 &>(param);

    // Software reset - p.19
    i2cAddr.subAddress                   = AW8898_REG_ID;
    AW8898_reg_shutdown_t dev_shutdown = {};
    AW8898_reg_idcode_t dev_id = { AW8898_SW_RESET_MAGIC };
    i2c->Write(i2cAddr, (uint8_t *)&dev_id, 2);

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
            SampleRate: Sample rate for this digital audio interface; SlotLength: The length of one audio slot in unit of BCK clock; SlotNumber:  How  many  slots  supported  in  this  audio  interface.  For  example:  2-slot  supported  in  I2S  mode, 
            4-slot supported in TDM mode.
        
    */

    aw8898_reg_i2sctrl_t i2s_setup = 
    {
        .inplev = 1;    //attenuate input
        .chsel = 3;     //mono; (L+R)/2
        .i2smd = 0;     //standard I2S
        .i2sfs = 0;     //16 bit
        .i2sbck = 0;    //21*fs(16*2)
    };

    switch (params.sampleRate) {

    case CodecParamsAW8898::SampleRate::Rate8KHz:
        i2s_setup.i2ssr = 0;     //8 kHz
        break;

    case CodecParamsAW8898::SampleRate::Rate16KHz:
        i2s_setup.i2ssr = 3;     //16 kHz
        break;

    case CodecParamsAW8898::SampleRate::Rate44K1Hz:
        i2s_setup.i2ssr = 7;     //44.1 kHz
        break;

    case CodecParamsAW8898::SampleRate::Rate48KHz:
        i2s_setup.i2ssr = 8;     //48 kHz
        break;

    case CodecParamsAW8898::SampleRate::Rate32KHz:
        i2s_setup.i2ssr = 6;     //32 kHz
        break;

    case CodecParamsAW8898::SampleRate::Rate96KHz:
        i2s_setup.i2ssr = 9;     //96 kHz
        break;

    default:
        return CodecRetCode::InvalidSampleRate;
    }
    i2cAddr.subAddress = AW8898_REG_I2SCTRL;
    i2c->Write(i2cAddr, (uint8_t *)&i2s_setup, 2);

    aw8898_reg_i2stxcfg_t i2s_tx_setup = 
    {
        .fsync_type = 0;    //one slot wide sync pulse
        .slot_num = 0;      // 2 slots
        .tx_slot_vld = 0;   //send on slot 0
        .rx_slot_vld = 3;   //slot 0,1
        .drvstren = 0;      //I2S_DATAO pad drive 2mA
        .dohz = 0;          //unused channel data set to 0
    };
    i2cAddr.subAddress = AW8898_REG_I2STXCTRL;
    i2c->Write(i2cAddr, (uint8_t *)&i2s_setup, 2);

    /* 3.1 Enable system (SYSCTRL.PWDN=0) */
    aw8898_reg_sysctrl_t sys_setup = 
    {
        // all values default to 0 except:
        .i2sen = 1;     //enable I2S
        .pwdn = 0;      //power up
    };
    i2cAddr.subAddress = AW8898_REG_SYSCTRL;
    i2c->Write(i2cAddr, (uint8_t *)&sys_setup, 2);

    /* 3,2 Bias, OSC, PLL active */
    
    /* 3.3 Waiting for PLL locked */
    i2cAddr.subAddress = AW8898_REG_SYSST;
    uint8_t sys_status = 0;
    while ((sys_status & 0x0001) != 1) //wait for PLLS = 1 (locked)
    {
        i2c->Read(i2cAddr, (uint8_t *)&sys_status, 2);
    }

    /* 4.1 Enable Boost and amplifier
            (SYSCTRL.AMPPD=0)
    */
    i2cAddr.subAddress = AW8898_REG_SYSCTRL;
    i2c->Modify(i2cAddr, 0x0002, disable, 2);   //(SYSCTRL.AMPPD=0)

    /* 4.2 Wait SYSST.SWS=1 */
    i2cAddr.subAddress = AW8898_REG_SYSST;
    while ((sys_status & (1 << 8))) != 1) //wait for SWS = 1 
    {
        i2c->Read(i2cAddr, (uint8_t *)&sys_status, 2);
    }

    // Store param configuration
    currentParams = params;

    // Set volume to 0 before enabling codec to avoid pops/clicks
    auto currVol = currentParams.outVolume;

    SetOutputVolume(currVol);

    return CodecRetCode::Success;
}

CodecRetCode CodecAW8898::Pause()
{
    // Turn off device
    i2cAddr.subAddress = AW8898_REG_SYSCTRL;
    i2c->Modify(i2cAddr, 0x0002, enable, 2);   //(SYSCTRL.AMPPD=1)
    i2c->Modify(i2cAddr, 0x0001, enable, 2);   //(SYSCTRL.PWDN=1)

    return CodecRetCode::Success;
}

CodecRetCode CodecAW8898::Resume()
{
    // Turn on device
    i2cAddr.subAddress = AW8898_REG_SYSCTRL;
    i2c->Modify(i2cAddr, 0x0001, disable, 2);   //(SYSCTRL.PWDN=0)
    i2c->Modify(i2cAddr, 0x0002, disable, 2);   //(SYSCTRL.AMPPD=0)
    
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
        ret = SetOutputPath(params.outputPath);
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
    if (vol == 0) {
        mute = 1;
    }

    

    currentParams.outVolume = vol;
    return CodecRetCode::Success;
}

CodecRetCode CodecAW8898::SetInputGain(const float gain)
{
    constexpr float scaleFactor = .1f;
    float gainToSet             = gain * scaleFactor;

    if (gain > 10) {
        gainToSet = 10;
    }

    AW8898_reg_lrec_dig_gain_t lgain = {};
    lgain.avl  = static_cast<uint8_t>(CodecParamsAW8898::RecordPathDigitalFineGain::Gain_p3dB); // fine gain
    lgain.avlg = gainToSet * 0.7; // coarse gain (0.7 used as scaling factor)

    i2cAddr.subAddress = AW8898_REG_LREC_DIG_GAIN;
    i2c->Write(i2cAddr, (uint8_t *)&lgain, 1);

    // coarse gain - 18dB, fine gain - 0dB
    AW8898_reg_rrec_dig_gain_t rgain = {};
    rgain.avr  = static_cast<uint8_t>(CodecParamsAW8898::RecordPathDigitalFineGain::Gain_p3dB); // fine gain
    rgain.avrg = gainToSet * 0.7; // coarse gain (0.7 used as scaling factor)

    i2cAddr.subAddress = AW8898_REG_RREC_DIG_GAIN;
    i2c->Write(i2cAddr, (uint8_t *)&rgain, 1);

    return CodecRetCode::Success;
}

// This two links are very helpful for calculating proper data format:
// https://www.mathsisfun.com/binary-decimal-hexadecimal-converter.html
// http://faculty.weber.edu/fonbrown/EE1000/Chapter%202.pdf
CodecRetCode CodecAW8898::WriteFilterCoeff(const float coeff, const uint8_t basereg)
{
    int32_t decimal = coeff * (float)(1 << 20); // multiply by 2^20

    volatile uint32_t fractional = decimal & 0xFFFFF;
    decimal >>= 20;

    uint8_t byte1 = (decimal << 4) | (fractional >> 16);

    uint8_t byte2 = fractional >> 8;
    uint8_t byte3 = fractional & 0xFF;

    i2cAddr.subAddress = basereg;
    i2c->Write(i2cAddr, (uint8_t *)&byte1, 1);
    i2cAddr.subAddress = basereg + 1;
    i2c->Write(i2cAddr, (uint8_t *)&byte2, 1);
    i2cAddr.subAddress = basereg + 2;
    i2c->Write(i2cAddr, (uint8_t *)&byte3, 1);

    return CodecRetCode::Success;
}

CodecRetCode CodecAW8898::MicBias(const bool enable)
{
    AW8898_reg_bias_mode_t biasmode = {};
    // BIAS created by bandgap reference
    biasmode.biasmode = 1;

    i2cAddr.subAddress = AW8898_REG_BIAS_CTRL;
    i2c->Write(i2cAddr, (uint8_t *)&biasmode, 1);

    // Turn on microphone bias(needed for jack detection and powering external analog headphones mic)
    uint8_t mask = 0x10; // Set 4th bit (mic bias enable/disable)

    i2cAddr.subAddress = AW8898_REG_INPUT_ENABLE;
    i2c->Modify(i2cAddr, mask, enable, 1);

    return CodecRetCode::Success;
}

CodecRetCode CodecAW8898::SetupEarspeakerEqualizer()
{
    qfilter_coefficients_t band1_filter = {};
    qfilter_coefficients_t band2_filter = {};
    qfilter_coefficients_t band3_filter = {};

    // Highpass,lowpass & flat filters don't use Gain parameter
    qfilter_CalculateCoeffs(FilterHighPass, 800, currentParams.GetSampleRateVal(), 0.707, 1, &band1_filter);
    qfilter_CalculateCoeffs(FilterLowPass, 6000, currentParams.GetSampleRateVal(), 0.707, 1, &band2_filter);
    qfilter_CalculateCoeffs(FilterFlat, 0, currentParams.GetSampleRateVal(), 0.707, 1, &band3_filter);

    // BAND1
    WriteFilterCoeff(band1_filter.b0, 0x46);
    WriteFilterCoeff(band1_filter.b1, 0x49);
    WriteFilterCoeff(band1_filter.b2, 0x4C);
    WriteFilterCoeff(band1_filter.a1, 0x4F);
    WriteFilterCoeff(band1_filter.a2, 0x52);

    // BAND2
    WriteFilterCoeff(band2_filter.b0, 0x55);
    WriteFilterCoeff(band2_filter.b1, 0x58);
    WriteFilterCoeff(band2_filter.b2, 0x5B);
    WriteFilterCoeff(band2_filter.a1, 0x5E);
    WriteFilterCoeff(band2_filter.a2, 0x61);

    // BAND3
    WriteFilterCoeff(band3_filter.b0, 0x64);
    WriteFilterCoeff(band3_filter.b1, 0x67);
    WriteFilterCoeff(band3_filter.b2, 0x6A);
    WriteFilterCoeff(band3_filter.a1, 0x6D);
    WriteFilterCoeff(band3_filter.a2, 0x70);

    // Enable 3-band filter
    AW8898_reg_dsp_biquadfilter_enable_t filter = {};
    filter.eq3banden                              = 1;
    i2cAddr.subAddress                            = AW8898_REG_DSP_BIQUAD_FILTER_ENABLE;
    i2c->Write(i2cAddr, (uint8_t *)&filter, 1);

    return CodecRetCode::Success;
}

CodecRetCode CodecAW8898::SetupLoudspeakerEqualizer()
{
    qfilter_coefficients_t band1_filter = {};
    qfilter_coefficients_t band2_filter = {};
    qfilter_coefficients_t band3_filter = {};

    // Highpass,lowpass & flat filters don't use Gain parameter
    qfilter_CalculateCoeffs(FilterHighPass, 500, currentParams.GetSampleRateVal(), 0.707, 1, &band1_filter);
    qfilter_CalculateCoeffs(FilterFlat, 0, currentParams.GetSampleRateVal(), 0.707, 1, &band2_filter);
    qfilter_CalculateCoeffs(FilterFlat, 0, currentParams.GetSampleRateVal(), 0.707, 1, &band3_filter);

    // BAND1
    WriteFilterCoeff(band1_filter.b0, 0x46);
    WriteFilterCoeff(band1_filter.b1, 0x49);
    WriteFilterCoeff(band1_filter.b2, 0x4C);
    WriteFilterCoeff(band1_filter.a1, 0x4F);
    WriteFilterCoeff(band1_filter.a2, 0x52);

    // BAND2
    WriteFilterCoeff(band2_filter.b0, 0x55);
    WriteFilterCoeff(band2_filter.b1, 0x58);
    WriteFilterCoeff(band2_filter.b2, 0x5B);
    WriteFilterCoeff(band2_filter.a1, 0x5E);
    WriteFilterCoeff(band2_filter.a2, 0x61);

    // BAND3
    WriteFilterCoeff(band3_filter.b0, 0x64);
    WriteFilterCoeff(band3_filter.b1, 0x67);
    WriteFilterCoeff(band3_filter.b2, 0x6A);
    WriteFilterCoeff(band3_filter.a1, 0x6D);
    WriteFilterCoeff(band3_filter.a2, 0x70);

    // Enable 3-band filter
    AW8898_reg_dsp_biquadfilter_enable_t filter = {};
    filter.eq3banden                              = 1;
    i2cAddr.subAddress                            = AW8898_REG_DSP_BIQUAD_FILTER_ENABLE;
    i2c->Write(i2cAddr, (uint8_t *)&filter, 1);

    return CodecRetCode::Success;
}

CodecRetCode CodecAW8898::Reset()
{

    // Turn off device
    AW8898_reg_shutdown_t dev_shutdown = {.unused = 0, .shdn = 0};

    i2cAddr.subAddress = AW8898_REG_DEVICE_SHUTDOWN;
    i2c->Write(i2cAddr, (uint8_t *)&dev_shutdown, 1);

    // Set all registers to default state
    AW8898_reg_swreset_t reset = {};
    reset.swreset                = 1;
    i2cAddr.subAddress           = AW8898_REG_SWRESET;
    i2c->Write(i2cAddr, (uint8_t *)&reset, 1);

    return CodecRetCode::Success;
}

CodecRetCode CodecAW8898::SetOutputPath(const CodecParamsAW8898::OutputPath path)
{
    Reset();
    currentParams.outputPath = path;
    Start(currentParams);

    return CodecRetCode::Success;
}

CodecRetCode CodecAW8898::SetInputPath(const CodecParamsAW8898::InputPath path)
{
    Reset();
    currentParams.inputPath = path;
    Start(currentParams);

    return CodecRetCode::Success;
}

CodecRetCode CodecAW8898::SetMute(const bool enable)
{
    // Set/clr 7th bit in register which controls mute/unmute
    uint8_t mask = 0x80;
    uint8_t regl, regr = 0;

    switch (currentParams.outputPath) {
    case CodecParamsAW8898::OutputPath::Headphones: {
        regl = AW8898_REG_LHP_VOL_CTRL;
        regr = AW8898_REG_RHP_VOL_CTRL;
    } break;

    case CodecParamsAW8898::OutputPath::Earspeaker: {
        regl = AW8898_REG_RECV_VOL_CTRL;
        regr = 0;
    } break;

    case CodecParamsAW8898::OutputPath::Loudspeaker: {
        regl = AW8898_REG_LSPK_VOL_CTRL;
        regr = AW8898_REG_RSPK_VOL_CTRL;
    } break;

    default:
        return CodecRetCode::InvalidArgument;
    }

    if (regl) {
        i2cAddr.subAddress = regl;
        i2c->Modify(i2cAddr, mask, enable, 1);
    }
    if (regr) {
        i2cAddr.subAddress = regr;
        i2c->Modify(i2cAddr, mask, enable, 1);
    }
    return CodecRetCode::Success;
}

std::optional<uint32_t> CodecAW8898::Probe()
{
    uint8_t id = 0;

    i2cAddr.subAddress = AW8898_REG_REVID;
    i2c->Write(i2cAddr, (uint8_t *)&id, 1);
    return id;
}
