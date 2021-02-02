// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "CodecMAX98090.hpp"
#include "max98090_regs.hpp"
#include "bsp/BoardDefinitions.hpp"
#include "qfilter.h"

using namespace drivers;

CodecMAX98090::CodecMAX98090() : i2cAddr{}
{

    i2cAddr.deviceAddress  = MAX98090_I2C_ADDR;
    i2cAddr.subAddressSize = 1; // MAX98090 uses 1byte addressing
    i2c                    = DriverI2C::Create(
        static_cast<I2CInstances>(BoardDefinitions::AUDIOCODEC_I2C),
        DriverI2CParams{.baudrate = static_cast<uint32_t>(BoardDefinitions::AUDIOCODEC_I2C_BAUDRATE)});
    Reset();
}

CodecMAX98090::~CodecMAX98090()
{
    Reset();
}

CodecRetCode CodecMAX98090::Start(const CodecParams &param)
{

    const CodecParamsMAX98090 &params = static_cast<const CodecParamsMAX98090 &>(param);

    // Turn off device
    i2cAddr.subAddress                   = MAX98090_REG_DEVICE_SHUTDOWN;
    max98090_reg_shutdown_t dev_shutdown = {.shdn = 0};
    i2c->Write(i2cAddr, (uint8_t *)&dev_shutdown, 1);

    max98090_reg_masterclock_quick_setup_t masterclock_setup = {0};
    masterclock_setup.M12P288                                = 1;
    i2cAddr.subAddress                                       = MAX98090_REG_MASTER_CLOCK_QUICK_SETUP;
    i2c->Write(i2cAddr, (uint8_t *)&masterclock_setup, 1);

    max98090_reg_master_samplerate_quick_setup_t samplerate_setup = {0};
    switch (params.sampleRate) {

    case CodecParamsMAX98090::SampleRate::Rate8KHz:
        samplerate_setup.SR_8K = 1;
        break;

    case CodecParamsMAX98090::SampleRate::Rate16KHz:
        samplerate_setup.SR_16K = 1;
        break;

    case CodecParamsMAX98090::SampleRate::Rate44K1Hz:
        samplerate_setup.SR_44K1 = 1;
        break;

    case CodecParamsMAX98090::SampleRate::Rate48KHz:
        samplerate_setup.SR_48K = 1;
        break;

    case CodecParamsMAX98090::SampleRate::Rate32KHz:
        samplerate_setup.SR_32K = 1;
        break;

    case CodecParamsMAX98090::SampleRate::Rate96KHz:
        samplerate_setup.SR_96K = 1;
        break;

    default:
        return CodecRetCode::InvalidSampleRate;
    }
    i2cAddr.subAddress = MAX98090_REG_MASTER_SAMPLE_RATE_QUICK_SETUP;
    i2c->Write(i2cAddr, (uint8_t *)&samplerate_setup, 1);

    // Sets up DAI for I2S master mode operation.
    max98090_reg_dai_quick_setup_t q_dai_setup = {0};
    q_dai_setup.i2sm                           = 1;
    i2cAddr.subAddress                         = MAX98090_REG_DAI_QUICK_SETUP;
    i2c->Write(i2cAddr, (uint8_t *)&q_dai_setup, 1);

    // OUT configuration
    if (params.outputPath != bsp::AudioDevice::OutputPath::None) {

        // Control HP performance
        max98090_reg_dachp_perfmode_t dacperf = {0};
        dacperf.dachp                         = 1;
        dacperf.perfmode                      = 0;
        i2cAddr.subAddress                    = MAX98090_REG_DACHP_PERF_MODE;
        i2c->Write(i2cAddr, (uint8_t *)&dacperf, 1);

        switch (params.outputPath) {

        case bsp::AudioDevice::OutputPath::HeadphonesMono: {
            max98090_reg_playback_quick_setup_t q_playback_setup = {0};
            q_playback_setup.dig2hp                              = 1;
            i2cAddr.subAddress                                   = MAX98090_REG_PLAYBACK_QUICK_SETUP;
            i2c->Write(i2cAddr, (uint8_t *)&q_playback_setup, 1);

            // Mix left DAC channel to left&right HP output
            max98090_reg_lhp_mixer_t lmixconf = {0};
            lmixconf.mixhpl                   = 1;
            i2cAddr.subAddress = MAX98090_REG_LHP_MIXER_CONF;
            i2c->Write(i2cAddr, (uint8_t *)&lmixconf, 1);

            max98090_reg_rhp_mixer_t rmixconf = {0};
            rmixconf.mixhpr                   = 1;
            i2cAddr.subAddress = MAX98090_REG_RHP_MIXER_CONF;
            i2c->Write(i2cAddr, (uint8_t *)&rmixconf, 1);

            // Use mixer outputs instead of direct DAC outputs
            max98090_reg_hpmix_conf_t mixconf = {0};
            mixconf.mixhplsel                 = 1;
            mixconf.mixhprsel                 = 1;
            i2cAddr.subAddress                = MAX98090_REG_HP_MIX_CONF;
            i2c->Write(i2cAddr, (uint8_t *)&mixconf, 1);

        } break;

        case bsp::AudioDevice::OutputPath::Headphones: {
            max98090_reg_playback_quick_setup_t q_playback_setup = {0};
            q_playback_setup.dig2hp                              = 1;
            i2cAddr.subAddress = MAX98090_REG_PLAYBACK_QUICK_SETUP;
            i2c->Write(i2cAddr, (uint8_t *)&q_playback_setup, 1);

        } break;

        case bsp::AudioDevice::OutputPath::Earspeaker: {
            max98090_reg_playback_quick_setup_t q_playback_setup = {0};
            q_playback_setup.dig2ear = 1;
            i2cAddr.subAddress       = MAX98090_REG_PLAYBACK_QUICK_SETUP;
            i2c->Write(i2cAddr, (uint8_t *)&q_playback_setup, 1);

            SetupEarspeakerEqualizer();

        } break;

        case bsp::AudioDevice::OutputPath::Loudspeaker: {
            max98090_reg_playback_quick_setup_t q_playback_setup = {0};
            q_playback_setup.dig2spk = 1;
            i2cAddr.subAddress                                   = MAX98090_REG_PLAYBACK_QUICK_SETUP;
            i2c->Write(i2cAddr, (uint8_t *)&q_playback_setup, 1);

            uint8_t mask       = 0x08; // Set 3th bit (dmono on)
            i2cAddr.subAddress = MAX98090_REG_INOUT_PATH_CONF;
            i2c->Modify(i2cAddr, mask, true, 1);

            // Turn off right speaker path
            max98090_reg_outputenable_t outputenable = {0};
            outputenable.dalen                       = 1;
            outputenable.splen                       = 1;
            i2cAddr.subAddress                       = MAX98090_REG_OUTPUT_ENABLE;
            i2c->Write(i2cAddr, (uint8_t *)&outputenable, 1);

            SetupLoudspeakerEqualizer();

        } break;

        case bsp::AudioDevice::OutputPath::LoudspeakerMono: {
            max98090_reg_playback_quick_setup_t q_playback_setup = {0};
            q_playback_setup.dig2spk                             = 1;
            i2cAddr.subAddress = MAX98090_REG_PLAYBACK_QUICK_SETUP;
            i2c->Write(i2cAddr, (uint8_t *)&q_playback_setup, 1);

            // Turn off right speaker path
            max98090_reg_outputenable_t outputenable = {0};
            outputenable.dalen                       = 1;
            outputenable.splen                       = 1;
            i2cAddr.subAddress                       = MAX98090_REG_OUTPUT_ENABLE;
            i2c->Write(i2cAddr, (uint8_t *)&outputenable, 1);

            SetupLoudspeakerEqualizer();
        } break;

        default:
            return CodecRetCode::InvalidOutputPath;
        }
    }

    // IN configuration
    if (params.inputPath != bsp::AudioDevice::InputPath::None) {
        // Set input path
        switch (params.inputPath) {

        case bsp::AudioDevice::InputPath::Headphones: {
            max98090_reg_input_to_record_quick_t q_input_setup = {0};
            q_input_setup.in34dan                              = 1;
            i2cAddr.subAddress                                 = MAX98090_REG_LINE_INPUT_TO_RECORD_QUICK;
            i2c->Write(i2cAddr, (uint8_t *)&q_input_setup, 1);
        } break;

        case bsp::AudioDevice::InputPath::Microphone: {
            max98090_reg_input_to_record_quick_t q_input_setup = {0};
            max98090_reg_digmic_enable_t digena                = {0};

            // Enable left and right digital mic interface
            digena.digmicl = 1;
            digena.digmicr = 1;
            // Harman Kardon dig microphones specify valid clock range as 1.024MHz - 4.8MHz, typical ~2.4MHz
            digena.dmicclk = 3; // fDMC = fPCLK/5 - > fPCLK=12.288MHz fDMC = 2.458MHz

            // Table 17 from datasheet
            digconf.dmicfreq = 0;
            switch (currentParams.GetSampleRateVal()) {
            case 8000:
                digconf.dmiccomp =
                    static_cast<uint8_t>(CodecParamsMAX98090::DigitalMicrophoneCompensationFilter::DMIC_COMP_7);
                break;
            case 16000:
                digconf.dmiccomp =
                    static_cast<uint8_t>(CodecParamsMAX98090::DigitalMicrophoneCompensationFilter::DMIC_COMP_8);
                break;
            case 32000:
            case 44100:
            case 48000:
                digconf.dmiccomp =
                    static_cast<uint8_t>(CodecParamsMAX98090::DigitalMicrophoneCompensationFilter::DMIC_COMP_6);
                break;
            case 96000:
            case 0:
            default:
                digconf.dmiccomp =
                    static_cast<uint8_t>(CodecParamsMAX98090::DigitalMicrophoneCompensationFilter::DMIC_COMP_6);
                break;
            }

            i2cAddr.subAddress = MAX98090_REG_DIG_MIC_ENABLE;
            i2c->Write(i2cAddr, (uint8_t *)&digena, 1);

            i2cAddr.subAddress = MAX98090_REG_DIG_MIC_CONF;
            i2c->Write(i2cAddr, (uint8_t *)&digconf, 1);

            q_input_setup.in12sab = 1;
            i2cAddr.subAddress    = MAX98090_REG_LINE_INPUT_TO_RECORD_QUICK;
            i2c->Write(i2cAddr, (uint8_t *)&q_input_setup, 1);

        } break;

        default:
            return CodecRetCode::InvalidInputPath;
        }
    }

    // Turn on DC blocking filters
    uint8_t mask       = (1 << 6) | (1 << 5); // set 6th and 7th bit (AHPF and DHPF)
    i2cAddr.subAddress = MAX98090_REG_PLAYBACK_DSP_FILTER_CONF;
    i2c->Modify(i2cAddr, mask, true, 1);

    // Store param configuration
    currentParams = params;

    // Set volume to 0 before enabling codec to avoid pops/clicks
    auto currVol = currentParams.outVolume;
    SetOutputVolume(0);

    SetInputGain(currentParams.inGain);

    // Turn on device
    dev_shutdown.shdn = 1;

    i2cAddr.subAddress = MAX98090_REG_DEVICE_SHUTDOWN;
    i2c->Write(i2cAddr, (uint8_t *)&dev_shutdown, 1);

    SetOutputVolume(currVol);

    return CodecRetCode::Success;
}

CodecRetCode CodecMAX98090::Pause()
{
    // Turn off device
    max98090_reg_shutdown_t dev_shutdown = {.shdn = 0};
    i2cAddr.subAddress                   = MAX98090_REG_DEVICE_SHUTDOWN;
    i2c->Write(i2cAddr, (uint8_t *)&dev_shutdown, 1);

    return CodecRetCode::Success;
}

CodecRetCode CodecMAX98090::Resume()
{
    // Turn on device
    max98090_reg_shutdown_t dev_shutdown = {.shdn = 1};
    i2cAddr.subAddress                   = MAX98090_REG_DEVICE_SHUTDOWN;
    i2c->Write(i2cAddr, (uint8_t *)&dev_shutdown, 1);

    return CodecRetCode::Success;
}

CodecRetCode CodecMAX98090::Stop()
{
    Reset();
    return CodecRetCode::Success;
}

CodecRetCode CodecMAX98090::Ioctrl(const CodecParams &param)
{

    const CodecParamsMAX98090 &params = static_cast<const CodecParamsMAX98090 &>(param);

    CodecRetCode ret = CodecRetCode::Success;

    switch (params.opCmd) {
    case CodecParamsMAX98090::Cmd::SetOutVolume:
        ret = SetOutputVolume(params.outVolume);
        break;

    case CodecParamsMAX98090::Cmd::SetInGain:
        ret = SetInputGain(params.inGain);
        break;
    case CodecParamsMAX98090::Cmd::SetInput:
        ret = SetInputPath(params.inputPath);
        break;
    case CodecParamsMAX98090::Cmd::SetOutput:
        ret = SetOutputPath(params.outputPath);
        break;
    case CodecParamsMAX98090::Cmd::MicBiasCtrl:
        ret = MicBias(params.micBiasEnable);
        break;
    case CodecParamsMAX98090::Cmd::Reset:
        ret = Reset();
        break;
    case CodecParamsMAX98090::Cmd::SetMute:
        ret = SetMute(params.muteEnable);
        break;
    default:
        break;
    }

    return ret;
}

CodecRetCode CodecMAX98090::SetOutputVolume(const float vol)
{
    uint8_t mute = 0;

    // If volume set to 0 then mute output
    if (vol == 0) {
        mute = 1;
    }

    switch (currentParams.outputPath) {
    case bsp::AudioDevice::OutputPath::Headphones:
    case bsp::AudioDevice::OutputPath::HeadphonesMono: {
        // Scale input volume(range 0 - 100) to MAX98090 range(decibels hardcoded as specific hex values)
        constexpr float scale_factor     = .31f * 10.f;
        uint8_t volume                   = static_cast<float>(vol * scale_factor);
        max98090_reg_lhp_vol_ctrl_t lvol = {0};
        max98090_reg_rhp_vol_ctrl_t rvol = {0};

        lvol.hplm   = mute;
        rvol.hprm   = mute;
        lvol.hpvoll = volume;
        rvol.hpvolr = volume;

        i2cAddr.subAddress = MAX98090_REG_LHP_VOL_CTRL;
        i2c->Write(i2cAddr, (uint8_t *)&lvol, 1);
        i2cAddr.subAddress = MAX98090_REG_RHP_VOL_CTRL;
        i2c->Write(i2cAddr, (uint8_t *)&rvol, 1);

    } break;

    case bsp::AudioDevice::OutputPath::Earspeaker: {
        // Scale input volume(range 0 - 100) to MAX98090 range(decibels hardcoded as specific hex values)
        constexpr float scale_factor     = .31f * 10.f;
        uint8_t volume                   = static_cast<float>(vol * scale_factor);
        max98090_reg_recv_vol_ctrl_t vol = {0};

        vol.rcvlm   = mute;
        vol.rcvlvol = volume;

        i2cAddr.subAddress = MAX98090_REG_RECV_VOL_CTRL;
        i2c->Write(i2cAddr, (uint8_t *)&vol, 1);
    } break;

    case bsp::AudioDevice::OutputPath::Loudspeaker:
    case bsp::AudioDevice::OutputPath::LoudspeakerMono: {
        // Scale input volume(range 0 - 100) to MAX98090 range(decibels hardcoded as specific hex values)
        constexpr float scale_factor = .39f * 10.f;
        uint8_t volume               = static_cast<float>(vol * scale_factor) + 0x18;

        max98090_reg_lspk_vol_ctrl_t lvol = {0};
        max98090_reg_rspk_vol_ctrl_t rvol = {0};

        lvol.splm   = mute;
        rvol.sprm   = mute;
        lvol.spvoll = volume;
        rvol.spvolr = volume;

        i2cAddr.subAddress = MAX98090_REG_LSPK_VOL_CTRL;
        i2c->Write(i2cAddr, (uint8_t *)&lvol, 1);

        i2cAddr.subAddress = MAX98090_REG_RSPK_VOL_CTRL;
        i2c->Write(i2cAddr, (uint8_t *)&rvol, 1);
    } break;

    default:
        return CodecRetCode::InvalidArgument;
    }

    currentParams.outVolume = vol;
    return CodecRetCode::Success;
}

CodecRetCode CodecMAX98090::SetInputGain(const float gain)
{
    constexpr float scaleFactor = .1f;
    float gainToSet             = gain * scaleFactor;

    if (gain > 10) {
        gainToSet = 10;
    }

    max98090_reg_lrec_dig_gain_t lgain = {0};
    lgain.avl = static_cast<uint8_t>(CodecParamsMAX98090::RecordPathDigitalFineGain::Gain_p3dB); // fine gain
    lgain.avlg                         = gainToSet * 0.7; // coarse gain (0.7 used as scaling factor)

    i2cAddr.subAddress = MAX98090_REG_LREC_DIG_GAIN;
    i2c->Write(i2cAddr, (uint8_t *)&lgain, 1);

    // coarse gain - 18dB, fine gain - 0dB
    max98090_reg_rrec_dig_gain_t rgain = {0};
    rgain.avr = static_cast<uint8_t>(CodecParamsMAX98090::RecordPathDigitalFineGain::Gain_p3dB); // fine gain
    rgain.avrg                         = gainToSet * 0.7; // coarse gain (0.7 used as scaling factor)

    i2cAddr.subAddress = MAX98090_REG_RREC_DIG_GAIN;
    i2c->Write(i2cAddr, (uint8_t *)&rgain, 1);

    return CodecRetCode::Success;
}

// This two links are very helpful for calculating proper data format:
// https://www.mathsisfun.com/binary-decimal-hexadecimal-converter.html
// http://faculty.weber.edu/fonbrown/EE1000/Chapter%202.pdf
CodecRetCode CodecMAX98090::WriteFilterCoeff(const float coeff, const uint8_t basereg)
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

CodecRetCode CodecMAX98090::MicBias(const bool enable)
{
    max98090_reg_bias_mode_t biasmode = {0};
    // BIAS created by bandgap reference
    biasmode.biasmode = 1;

    i2cAddr.subAddress = MAX98090_REG_BIAS_CTRL;
    i2c->Write(i2cAddr, (uint8_t *)&biasmode, 1);

    // Turn on microphone bias(needed for jack detection and powering external analog headphones mic)
    uint8_t mask = 0x10; // Set 4th bit (mic bias enable/disable)

    i2cAddr.subAddress = MAX98090_REG_INPUT_ENABLE;
    i2c->Modify(i2cAddr, mask, enable, 1);

    return CodecRetCode::Success;
}

CodecRetCode CodecMAX98090::SetupEarspeakerEqualizer()
{
    qfilter_coefficients_t band1_filter = {0};
    qfilter_coefficients_t band2_filter = {0};
    qfilter_coefficients_t band3_filter = {0};

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
    max98090_reg_dsp_biquadfilter_enable_t filter = {0};
    filter.eq3banden                              = 1;
    i2cAddr.subAddress                            = MAX98090_REG_DSP_BIQUAD_FILTER_ENABLE;
    i2c->Write(i2cAddr, (uint8_t *)&filter, 1);

    return CodecRetCode::Success;
}

CodecRetCode CodecMAX98090::SetupLoudspeakerEqualizer()
{
    qfilter_coefficients_t band1_filter = {0};
    qfilter_coefficients_t band2_filter = {0};
    qfilter_coefficients_t band3_filter = {0};

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
    max98090_reg_dsp_biquadfilter_enable_t filter = {0};
    filter.eq3banden                              = 1;
    i2cAddr.subAddress                            = MAX98090_REG_DSP_BIQUAD_FILTER_ENABLE;
    i2c->Write(i2cAddr, (uint8_t *)&filter, 1);

    return CodecRetCode::Success;
}

CodecRetCode CodecMAX98090::Reset()
{

    // Turn off device
    max98090_reg_shutdown_t dev_shutdown = {.shdn = 0};

    i2cAddr.subAddress = MAX98090_REG_DEVICE_SHUTDOWN;
    i2c->Write(i2cAddr, (uint8_t *)&dev_shutdown, 1);

    // Set all registers to default state
    max98090_reg_swreset_t reset = {0};
    reset.swreset                = 1;
    i2cAddr.subAddress           = MAX98090_REG_SWRESET;
    i2c->Write(i2cAddr, (uint8_t *)&reset, 1);

    return CodecRetCode::Success;
}

CodecRetCode CodecMAX98090::SetOutputPath(const bsp::AudioDevice::OutputPath path)
{
    Reset();
    currentParams.outputPath = path;
    Start(currentParams);

    return CodecRetCode::Success;
}

CodecRetCode CodecMAX98090::SetInputPath(const bsp::AudioDevice::InputPath path)
{
    Reset();
    currentParams.inputPath = path;
    Start(currentParams);

    return CodecRetCode::Success;
}

CodecRetCode CodecMAX98090::SetMute(const bool enable)
{
    // Set/clr 7th bit in register which controls mute/unmute
    uint8_t mask = 0x80;
    uint8_t regl, regr = 0;

    switch (currentParams.outputPath) {
    case bsp::AudioDevice::OutputPath::Headphones: {
        regl = MAX98090_REG_LHP_VOL_CTRL;
        regr = MAX98090_REG_RHP_VOL_CTRL;
    } break;

    case bsp::AudioDevice::OutputPath::Earspeaker: {
        regl = MAX98090_REG_RECV_VOL_CTRL;
        regr = 0;
    } break;

    case bsp::AudioDevice::OutputPath::Loudspeaker: {
        regl = MAX98090_REG_LSPK_VOL_CTRL;
        regr = MAX98090_REG_RSPK_VOL_CTRL;
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

std::optional<uint32_t> CodecMAX98090::Probe()
{
    uint8_t id = 0;

    i2cAddr.subAddress = MAX98090_REG_REVID;
    i2c->Write(i2cAddr, (uint8_t *)&id, 1);
    return id;
}
