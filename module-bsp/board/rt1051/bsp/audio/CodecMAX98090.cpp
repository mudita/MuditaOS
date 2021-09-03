// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "CodecMAX98090.hpp"
#include "max98090_regs.hpp"
#include "board/BoardDefinitions.hpp"

#include <module-utils/Utils.hpp>

#include <map>

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
    max98090_reg_shutdown_t dev_shutdown = {};
    i2c->Write(i2cAddr, (uint8_t *)&dev_shutdown, 1);

    max98090_reg_masterclock_quick_setup_t masterclock_setup = {};
    masterclock_setup.M12P288                                = 1;
    i2cAddr.subAddress                                       = MAX98090_REG_MASTER_CLOCK_QUICK_SETUP;
    i2c->Write(i2cAddr, (uint8_t *)&masterclock_setup, 1);

    max98090_reg_master_samplerate_quick_setup_t samplerate_setup = {};
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
    max98090_reg_dai_quick_setup_t q_dai_setup = {};
    q_dai_setup.i2sm                           = 1;
    i2cAddr.subAddress                         = MAX98090_REG_DAI_QUICK_SETUP;
    i2c->Write(i2cAddr, (uint8_t *)&q_dai_setup, 1);

    // OUT configuration
    if (params.outputPath != CodecParamsMAX98090::OutputPath::None) {

        // Control HP performance
        max98090_reg_dachp_perfmode_t dacperf = {};
        dacperf.dachp                         = 1;
        dacperf.perfmode                      = 0;
        i2cAddr.subAddress                    = MAX98090_REG_DACHP_PERF_MODE;
        i2c->Write(i2cAddr, (uint8_t *)&dacperf, 1);

        switch (params.outputPath) {

        case CodecParamsMAX98090::OutputPath::HeadphonesMono: {
            // Set output route
            max98090_reg_playback_quick_setup_t q_playback_setup = {};
            q_playback_setup.dig2hp                              = 1;
            i2cAddr.subAddress                                   = MAX98090_REG_PLAYBACK_QUICK_SETUP;
            i2c->Write(i2cAddr, (uint8_t *)&q_playback_setup, 1);

            // Mix left DAC channel to left&right HP output
            max98090_reg_lhp_mixer_t lmixconf = {};
            lmixconf.mixhpl                   = 1;
            i2cAddr.subAddress                = MAX98090_REG_LHP_MIXER_CONF;
            i2c->Write(i2cAddr, (uint8_t *)&lmixconf, 1);

            max98090_reg_rhp_mixer_t rmixconf = {};
            rmixconf.mixhpr                   = 1;
            i2cAddr.subAddress                = MAX98090_REG_RHP_MIXER_CONF;
            i2c->Write(i2cAddr, (uint8_t *)&rmixconf, 1);

            // Use mixer outputs instead of direct DAC outputs
            max98090_reg_hpmix_conf_t mixconf = {};
            mixconf.mixhplsel                 = 1;
            mixconf.mixhprsel                 = 1;
            i2cAddr.subAddress                = MAX98090_REG_HP_MIX_CONF;
            i2c->Write(i2cAddr, (uint8_t *)&mixconf, 1);
            SetFilterMode(FilterMode::Voice);
        } break;

        case CodecParamsMAX98090::OutputPath::Headphones: {
            max98090_reg_playback_quick_setup_t q_playback_setup = {};
            q_playback_setup.dig2hp                              = 1;
            i2cAddr.subAddress                                   = MAX98090_REG_PLAYBACK_QUICK_SETUP;
            i2c->Write(i2cAddr, (uint8_t *)&q_playback_setup, 1);

        } break;

        case CodecParamsMAX98090::OutputPath::Earspeaker: {
            max98090_reg_playback_quick_setup_t q_playback_setup = {};
            q_playback_setup.dig2ear                             = 1;
            i2cAddr.subAddress                                   = MAX98090_REG_PLAYBACK_QUICK_SETUP;
            i2c->Write(i2cAddr, (uint8_t *)&q_playback_setup, 1);

            constexpr uint8_t coarse = 2;
            constexpr uint8_t fine   = 2;
            SetPlaybackPath(coarse, fine);
            SetFilterMode(FilterMode::Voice);
        } break;

        case CodecParamsMAX98090::OutputPath::Loudspeaker: {
            max98090_reg_playback_quick_setup_t q_playback_setup = {};
            q_playback_setup.dig2spk                             = 1;
            i2cAddr.subAddress                                   = MAX98090_REG_PLAYBACK_QUICK_SETUP;
            i2c->Write(i2cAddr, (uint8_t *)&q_playback_setup, 1);

            uint8_t mask       = 0x08; // Set 3th bit (dmono on)
            i2cAddr.subAddress = MAX98090_REG_INOUT_PATH_CONF;
            i2c->Modify(i2cAddr, mask, true, 1);

            // Turn off right speaker path
            max98090_reg_outputenable_t outputenable = {};
            outputenable.dalen                       = 1;
            outputenable.splen                       = 1;
            i2cAddr.subAddress                       = MAX98090_REG_OUTPUT_ENABLE;
            i2c->Write(i2cAddr, (uint8_t *)&outputenable, 1);

            constexpr uint8_t coarse = 0;
            constexpr uint8_t fine   = 2;
            SetPlaybackPath(coarse, fine);
        } break;

        case CodecParamsMAX98090::OutputPath::LoudspeakerMono: {
            max98090_reg_playback_quick_setup_t q_playback_setup = {};
            q_playback_setup.dig2spk                             = 1;
            i2cAddr.subAddress                                   = MAX98090_REG_PLAYBACK_QUICK_SETUP;
            i2c->Write(i2cAddr, (uint8_t *)&q_playback_setup, 1);

            // Turn off right speaker path
            max98090_reg_outputenable_t outputenable = {};
            outputenable.dalen                       = 1;
            outputenable.splen                       = 1;
            i2cAddr.subAddress                       = MAX98090_REG_OUTPUT_ENABLE;
            i2c->Write(i2cAddr, (uint8_t *)&outputenable, 1);

            constexpr uint8_t coarse = 1;
            constexpr uint8_t fine   = 0;
            SetPlaybackPath(coarse, fine);
            SetFilterMode(FilterMode::Voice);
        } break;

        default:
            return CodecRetCode::InvalidOutputPath;
        }
    }

    // IN configuration
    if (params.inputPath != CodecParamsMAX98090::InputPath::None) {
        // Set input path
        switch (params.inputPath) {

        case CodecParamsMAX98090::InputPath::Headphones: {
            max98090_reg_input_to_record_quick_t q_input_setup = {};
            q_input_setup.in34dan                              = 1;
            i2cAddr.subAddress                                 = MAX98090_REG_LINE_INPUT_TO_RECORD_QUICK;
            i2c->Write(i2cAddr, (uint8_t *)&q_input_setup, 1);
        } break;

        case CodecParamsMAX98090::InputPath::Microphone: {
            max98090_reg_input_to_record_quick_t q_input_setup = {};
            max98090_reg_digmic_enable_t digena                = {};
            max98090_reg_digmic_conf_t digconf                 = {};

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
    max98090_reg_shutdown_t dev_shutdown = {};
    i2cAddr.subAddress                   = MAX98090_REG_DEVICE_SHUTDOWN;
    i2c->Write(i2cAddr, (uint8_t *)&dev_shutdown, 1);

    return CodecRetCode::Success;
}

CodecRetCode CodecMAX98090::Resume()
{
    // Turn on device
    max98090_reg_shutdown_t dev_shutdown = {.unused = 0, .shdn = 1};
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

void CodecMAX98090::SetEqualizerParameters(float b0, float b1, float b2, float a1, float a2, std::size_t band)
{
    std::map<std::size_t, std::array<uint8_t, 5>> filterRegs = {
        {0, {0x46, 0x49, 0x4C, 0x4F, 0x52}},
        {1, {0x55, 0x58, 0x5B, 0x5E, 0x61}},
        {2, {0x64, 0x67, 0x6A, 0x6D, 0x70}},
        {3, {0x73, 0x76, 0x79, 0x7C, 0x7F}},
        {4, {0x82, 0x85, 0x88, 0x8B, 0x8E}},
        {5, {0x91, 0x94, 0x97, 0x9A, 0x9D}},
        {6, {0xA0, 0xA3, 0xA6, 0xA9, 0xAC}},
    };
    WriteFilterCoeff(b0, filterRegs[band].at(0));
    WriteFilterCoeff(b1, filterRegs[band].at(1));
    WriteFilterCoeff(b2, filterRegs[band].at(2));
    WriteFilterCoeff(a1, filterRegs[band].at(3));
    WriteFilterCoeff(a2, filterRegs[band].at(4));
}

CodecRetCode CodecMAX98090::SetOutputVolume(const float vol)
{
    uint8_t mute = 0;

    // If volume set to 0 then mute output
    if (vol == 0) {
        mute = 1;
    }

    switch (currentParams.outputPath) {
    case CodecParamsMAX98090::OutputPath::Headphones:
    case CodecParamsMAX98090::OutputPath::HeadphonesMono: {
        // Scale input volume(range 0 - 10) to MAX98090 range(decibels hardcoded as specific hex values)
        // @note:
        // In order to pass certification max value is limited and taken from measurements 0x0B: -28dB
        // Be carefull when changing it !!!
        constexpr auto scale_factor      = 1.24f;
        uint8_t volume                   = static_cast<float>(vol * scale_factor);
        max98090_reg_lhp_vol_ctrl_t lvol = {};
        max98090_reg_rhp_vol_ctrl_t rvol = {};

        lvol.hplm   = mute;
        rvol.hprm   = mute;
        lvol.hpvoll = volume;
        rvol.hpvolr = volume;

        i2cAddr.subAddress = MAX98090_REG_LHP_VOL_CTRL;
        i2c->Write(i2cAddr, (uint8_t *)&lvol, 1);
        i2cAddr.subAddress = MAX98090_REG_RHP_VOL_CTRL;
        i2c->Write(i2cAddr, (uint8_t *)&rvol, 1);

    } break;

    case CodecParamsMAX98090::OutputPath::Earspeaker: {
        // Scale input volume(range 0 - 10) to MAX98090 range(decibels hardcoded as specific hex values)
        constexpr auto scale_factor      = 3.1f; // take the whole scale
        uint8_t volume                   = static_cast<float>(vol * scale_factor);
        max98090_reg_recv_vol_ctrl_t vol = {};

        vol.rcvlm   = mute;
        vol.rcvlvol = volume;

        i2cAddr.subAddress = MAX98090_REG_RECV_VOL_CTRL;
        i2c->Write(i2cAddr, (uint8_t *)&vol, 1);
    } break;

    case CodecParamsMAX98090::OutputPath::Loudspeaker:
    case CodecParamsMAX98090::OutputPath::LoudspeakerMono: {
        // Scale input volume(range 0 - 10) to MAX98090 range(decibels hardcoded as specific hex values)
        constexpr auto scale_factor = 3.9f;
        uint8_t volume              = static_cast<float>(vol * scale_factor) + 0x18;

        max98090_reg_lspk_vol_ctrl_t lvol = {};
        max98090_reg_rspk_vol_ctrl_t rvol = {};

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

    max98090_reg_lrec_dig_gain_t lgain = {};
    lgain.avl  = static_cast<uint8_t>(CodecParamsMAX98090::RecordPathDigitalFineGain::Gain_p3dB); // fine gain
    lgain.avlg = gainToSet * 0.7; // coarse gain (0.7 used as scaling factor)

    i2cAddr.subAddress = MAX98090_REG_LREC_DIG_GAIN;
    i2c->Write(i2cAddr, (uint8_t *)&lgain, 1);

    // coarse gain - 18dB, fine gain - 0dB
    max98090_reg_rrec_dig_gain_t rgain = {};
    rgain.avr  = static_cast<uint8_t>(CodecParamsMAX98090::RecordPathDigitalFineGain::Gain_p3dB); // fine gain
    rgain.avrg = gainToSet * 0.7; // coarse gain (0.7 used as scaling factor)

    i2cAddr.subAddress = MAX98090_REG_RREC_DIG_GAIN;
    i2c->Write(i2cAddr, (uint8_t *)&rgain, 1);

    return CodecRetCode::Success;
}

CodecRetCode CodecMAX98090::WriteFilterCoeff(const float coeff, const uint8_t basereg)
{
    const auto [byte1, byte2, byte3] = utils::floatingPointConverter(coeff);

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
    max98090_reg_bias_mode_t biasmode = {};
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

CodecRetCode CodecMAX98090::Reset()
{

    // Turn off device
    max98090_reg_shutdown_t dev_shutdown = {.unused = 0, .shdn = 0};

    i2cAddr.subAddress = MAX98090_REG_DEVICE_SHUTDOWN;
    i2c->Write(i2cAddr, (uint8_t *)&dev_shutdown, 1);

    // Set all registers to default state
    max98090_reg_swreset_t reset = {};
    reset.swreset                = 1;
    i2cAddr.subAddress           = MAX98090_REG_SWRESET;
    i2c->Write(i2cAddr, (uint8_t *)&reset, 1);

    return CodecRetCode::Success;
}

void CodecMAX98090::SetPlaybackPath(uint8_t coarse, uint8_t fine)
{
    max98090_reg_playback_gainlevel_conf_t gain = {};
    gain.dvg                                    = coarse;
    gain.dv                                     = fine;
    i2cAddr.subAddress                          = MAX98090_REG_PLAYBACK_GAIN_LEVEL_CONF;
    i2c->Write(i2cAddr, (uint8_t *)&gain, 1);
}

void CodecMAX98090::SetFilterMode(FilterMode mode)
{
    constexpr uint8_t mask = (1 << 7);
    i2cAddr.subAddress     = MAX98090_REG_PLAYBACK_DSP_FILTER_CONF;
    i2c->Modify(i2cAddr, mask, static_cast<bool>(mode), 1);
}

void CodecMAX98090::EnableFilterBands(std::size_t bandsCount)
{
    max98090_reg_dsp_biquadfilter_enable_t filter = {};
    switch (bandsCount) {
    case 3:
        filter.eq3banden = 1;
        break;
    case 5:
        filter.eq5banden = 1;
        break;
    case 7:
        filter.eq7banden = 1;
        break;
    default:
        return;
    }
    i2cAddr.subAddress = MAX98090_REG_DSP_BIQUAD_FILTER_ENABLE;
    i2c->Write(i2cAddr, (uint8_t *)&filter, 1);
}

CodecRetCode CodecMAX98090::SetOutputPath(const CodecParamsMAX98090::OutputPath path)
{
    Reset();
    currentParams.outputPath = path;
    Start(currentParams);

    return CodecRetCode::Success;
}

CodecRetCode CodecMAX98090::SetInputPath(const CodecParamsMAX98090::InputPath path)
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
    case CodecParamsMAX98090::OutputPath::Headphones: {
        regl = MAX98090_REG_LHP_VOL_CTRL;
        regr = MAX98090_REG_RHP_VOL_CTRL;
    } break;

    case CodecParamsMAX98090::OutputPath::Earspeaker: {
        regl = MAX98090_REG_RECV_VOL_CTRL;
        regr = 0;
    } break;

    case CodecParamsMAX98090::OutputPath::Loudspeaker: {
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
