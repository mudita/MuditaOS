/*
 *  @file MAX98090.cpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 30.07.19
 *  @brief  
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */



#include "CodecMAX98090.hpp"
#include "max98090_regs.hpp"
#include "qfilter.h"


CodecMAX98090::CodecMAX98090() : i2CInst(BOARD_GetI2CInstance()) {
    Reset();
}

CodecMAX98090::~CodecMAX98090() {
    max98090_reg_swreset_t reset = {0};
    reset.swreset = 1;
    bsp_i2c_Send(i2CInst, MAX98090_I2C_ADDR, MAX98090_REG_SWRESET, 1, (uint8_t *) &reset, 1);
}

CodecRetCode CodecMAX98090::Start(const CodecParams &param) {

    const CodecParamsMAX98090 &params = static_cast<const CodecParamsMAX98090 &>(param);

    // Turn off device
    max98090_reg_shutdown_t dev_shutdown = {.shdn=0};
    bsp_i2c_Send(i2CInst, MAX98090_I2C_ADDR, MAX98090_REG_DEVICE_SHUTDOWN, 1, (uint8_t *) &dev_shutdown,
                 1);

    max98090_reg_masterclock_quick_setup_t masterclock_setup = {0};
    masterclock_setup.M12P288 = 1;
    bsp_i2c_Send(i2CInst, MAX98090_I2C_ADDR, MAX98090_REG_MASTER_CLOCK_QUICK_SETUP, 1,
                 (uint8_t *) &masterclock_setup, 1);

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
    bsp_i2c_Send(i2CInst, MAX98090_I2C_ADDR, MAX98090_REG_MASTER_SAMPLE_RATE_QUICK_SETUP, 1,
                 (uint8_t *) &samplerate_setup, 1);

    // Sets up DAI for I2S master mode operation.
    max98090_reg_dai_quick_setup_t q_dai_setup = {0};
    q_dai_setup.i2sm = 1;
    bsp_i2c_Send(i2CInst, MAX98090_I2C_ADDR, MAX98090_REG_DAI_QUICK_SETUP, 1, (uint8_t *) &q_dai_setup,
                 1);

    // OUT configuration
    if (params.outputPath != CodecParamsMAX98090::OutputPath::None) {
        // Set output route
        max98090_reg_playback_quick_setup_t q_playback_setup = {0};

        // Control HP performance
        max98090_reg_dachp_perfmode_t dacperf = {0};

        switch (params.outputPath) {

            case CodecParamsMAX98090::OutputPath::HeadphonesMono: {

                // Mix left DAC channel to left&right HP output
                max98090_reg_lhp_mixer_t lmixconf = {0};
                max98090_reg_rhp_mixer_t rmixconf = {0};
                lmixconf.mixhpl = 1;
                rmixconf.mixhpr = 1;
                bsp_i2c_Send(i2CInst, MAX98090_I2C_ADDR, MAX98090_REG_LHP_MIXER_CONF, 1,
                             (uint8_t *) &lmixconf, 1);
                bsp_i2c_Send(i2CInst, MAX98090_I2C_ADDR, MAX98090_REG_RHP_MIXER_CONF, 1,
                             (uint8_t *) &rmixconf, 1);

                q_playback_setup.dig2hp = 1;
                bsp_i2c_Send(i2CInst, MAX98090_I2C_ADDR, MAX98090_REG_PLAYBACK_QUICK_SETUP, 1,
                             (uint8_t *) &q_playback_setup, 1);

                max98090_reg_hpmix_conf_t mixconf = {0};
                mixconf.mixhplsel = 1;
                mixconf.mixhprsel = 1;
                bsp_i2c_Send(i2CInst, MAX98090_I2C_ADDR, MAX98090_REG_HP_MIX_CONF, 1,
                             (uint8_t *) &mixconf, 1);

            }
                break;

            case CodecParamsMAX98090::OutputPath::Headphones: {
                // Set DAC headphones output to high performance mode, increasing power consumption but providing the highest quality
                dacperf.dachp = 1;
                dacperf.perfmode = 1;
                q_playback_setup.dig2hp = 1;
                bsp_i2c_Send(i2CInst, MAX98090_I2C_ADDR, MAX98090_REG_PLAYBACK_QUICK_SETUP, 1,
                             (uint8_t *) &q_playback_setup, 1);
                bsp_i2c_Send(i2CInst, MAX98090_I2C_ADDR, MAX98090_REG_DACHP_PERF_MODE, 1,
                             (uint8_t *) &dacperf, 1);
            }
                break;

            case CodecParamsMAX98090::OutputPath::Earspeaker: {
                q_playback_setup.dig2ear = 1;
                bsp_i2c_Send(i2CInst, MAX98090_I2C_ADDR, MAX98090_REG_PLAYBACK_QUICK_SETUP, 1,
                             (uint8_t *) &q_playback_setup, 1);

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
                filter.eq3banden = 1;
                bsp_i2c_Send(i2CInst, MAX98090_I2C_ADDR, MAX98090_REG_DSP_BIQUAD_FILTER_ENABLE, 1,
                             (uint8_t *) &filter, 1);
            }
                break;

            case CodecParamsMAX98090::OutputPath::Loudspeaker: {
                q_playback_setup.dig2spk = 1;

                uint8_t mask = 0x08; // Set 3th bit (dmono on)
                bsp_i2c_ModifyReg(i2CInst, MAX98090_I2C_ADDR, MAX98090_REG_INOUT_PATH_CONF, 1, mask, 1,
                                  1);

                //TODO: Turn off/mute right speaker

                bsp_i2c_Send(i2CInst, MAX98090_I2C_ADDR, MAX98090_REG_PLAYBACK_QUICK_SETUP, 1,
                             (uint8_t *) &q_playback_setup, 1);

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
                filter.eq3banden = 1;
                bsp_i2c_Send(i2CInst, MAX98090_I2C_ADDR, MAX98090_REG_DSP_BIQUAD_FILTER_ENABLE, 1,
                             (uint8_t *) &filter, 1);


            }
                break;


            default:
                return CodecRetCode::InvalidOutputPath;

        }

    }



    // IN configuration
    if (params.inputPath != CodecParamsMAX98090::InputPath::None) {
        // Set input path
        switch (params.inputPath) {
            case CodecParamsMAX98090::InputPath::Headphones: {
                /* 			max98090_reg_analog_to_record_quick_t q_analog_setup = {0};

                            q_analog_setup.in34mic2 = 1;
                            bsp_i2c_Send(i2CInst,DeviceAddr, MAX98090_REG_ANALOG_MIC_TO_RECORD_QUICK,(uint8_t*)&q_analog_setup,1);

                            max98090_reg_radc_mix_input_t radcmix = {0};
                            bsp_i2c_Send(i2CInst,DeviceAddr, MAX98090_REG_RADC_MIXER_INPUT,(uint8_t*)&radcmix);

                            max98090_reg_ladc_mix_input_t ladcmix = {0};
                            ladcmix.mixadl = 0x40;
                            bsp_i2c_Send(i2CInst,DeviceAddr, MAX98090_REG_LADC_MIXER_INPUT,(uint8_t*)&ladcmix); */
                max98090_reg_input_to_record_quick_t q_input_setup = {0};
                q_input_setup.in34dan = 1;
                bsp_i2c_Send(i2CInst, MAX98090_I2C_ADDR, MAX98090_REG_LINE_INPUT_TO_RECORD_QUICK, 1,
                             (uint8_t *) &q_input_setup, 1);

                uint8_t mask = 0x10; // Set 4th bit (mic bias enable)
                bsp_i2c_ModifyReg(i2CInst, MAX98090_I2C_ADDR, MAX98090_REG_INPUT_ENABLE, 1, mask, 1, 1);

            }
                break;

            case CodecParamsMAX98090::InputPath::Microphone: {
                max98090_reg_input_to_record_quick_t q_input_setup = {0};
                max98090_reg_analog_to_record_quick_t q_analog_setup = {0};
                max98090_reg_digmic_enable_t digena = {0};

                // Enable left and right digital mic interface
                digena.digmicl = 1;
                digena.digmicr = 1;
                // Harman Kardon dig microphones specify valid clock range as 1.024MHz - 4.8MHz, typical ~2.4MHz
                digena.dmicclk = 3; // fDMC = fPCLK/5 - > fPCLK=12.288MHz fDMC = 2.458MHz
                bsp_i2c_Send(i2CInst, MAX98090_I2C_ADDR, MAX98090_REG_DIG_MIC_ENABLE, 1,
                             (uint8_t *) &digena, 1);


                // It seems that for digital mics it doesn't matter if digitial or analog input is chosen
                q_input_setup.in12sab = 1;
                bsp_i2c_Send(i2CInst, MAX98090_I2C_ADDR, MAX98090_REG_LINE_INPUT_TO_RECORD_QUICK, 1,
                             (uint8_t *) &q_input_setup, 1);
                //q_analog_setup.in12mic1 = 1;
                //bsp_i2c_Send(i2CInst,DeviceAddr, MAX98090_REG_ANALOG_MIC_TO_RECORD_QUICK,(uint8_t*)&q_analog_setup,1);


                uint8_t mask = 0x10; // Clr 4th bit (mic bias disable)
                bsp_i2c_ModifyReg(i2CInst, MAX98090_I2C_ADDR, MAX98090_REG_INPUT_ENABLE, 1, mask, 0, 1);

            }
                break;

            default:
                return CodecRetCode::InvalidInputPath;

        }

        // Turn on DC blocking filters
        uint8_t mask = (1 << 6) | (1 << 5); // set 6th and 7th bit (AHPF and DHPF)
        bsp_i2c_ModifyReg(i2CInst, MAX98090_I2C_ADDR, MAX98090_REG_PLAYBACK_DSP_FILTER_CONF, 1, mask, 1,
                          1);
    }

    // Store param configuration
    currentParams = params;

    // Set volume to 0 before enabling codec to avoid pops/clicks
    SetOutputVolume(0);

    // Turn on device
    dev_shutdown.shdn = 1;
    bsp_i2c_Send(i2CInst, MAX98090_I2C_ADDR, MAX98090_REG_DEVICE_SHUTDOWN, 1, (uint8_t *) &dev_shutdown,
                 1);

    SetOutputVolume(100);

    return CodecRetCode::Success;
}

CodecRetCode CodecMAX98090::Pause() {
    // Turn off device
    max98090_reg_shutdown_t dev_shutdown = {.shdn=0};
    bsp_i2c_Send(i2CInst, MAX98090_I2C_ADDR, MAX98090_REG_DEVICE_SHUTDOWN, 1, (uint8_t *) &dev_shutdown, 1);
    return CodecRetCode::Success;
}

CodecRetCode CodecMAX98090::Resume() {
    // Turn on device
    max98090_reg_shutdown_t dev_shutdown = {.shdn=1};
    bsp_i2c_Send(i2CInst, MAX98090_I2C_ADDR, MAX98090_REG_DEVICE_SHUTDOWN, 1, (uint8_t *) &dev_shutdown,
                 1);
    return CodecRetCode::Success;
}

CodecRetCode CodecMAX98090::Stop() {
    Reset();
    return CodecRetCode::Success;
}

CodecRetCode CodecMAX98090::Ioctrl(const CodecParams &param) {

    const CodecParamsMAX98090 &params = static_cast<const CodecParamsMAX98090 &>(param);

    CodecRetCode ret = CodecRetCode ::Success;

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


CodecRetCode CodecMAX98090::SetOutputVolume(const float vol) {
    uint8_t mute = 0;


    // If volume set to 0 then mute output
    if (vol == 0) {
        mute = 1;
    }

    switch (currentParams.outputPath) {
        case CodecParamsMAX98090::OutputPath::Headphones:
        case CodecParamsMAX98090::OutputPath::HeadphonesMono: {
            // Scale input volume(range 0 - 100) to MAX98090 range(decibels hardcoded as specific hex values)
            float scale_factor = 0.31;
            uint8_t volume = (float) (vol * scale_factor);
            max98090_reg_lhp_vol_ctrl_t lvol = {0};
            max98090_reg_rhp_vol_ctrl_t rvol = {0};

            lvol.hplm = mute;
            rvol.hprm = mute;
            lvol.hpvoll = volume;
            rvol.hpvolr = volume;

            bsp_i2c_Send(i2CInst, MAX98090_I2C_ADDR, MAX98090_REG_LHP_VOL_CTRL, 1, (uint8_t *) &lvol, 1);
            bsp_i2c_Send(i2CInst, MAX98090_I2C_ADDR, MAX98090_REG_RHP_VOL_CTRL, 1, (uint8_t *) &rvol, 1);

        }
            break;

        case CodecParamsMAX98090::OutputPath::Earspeaker: {
            // Scale input volume(range 0 - 100) to MAX98090 range(decibels hardcoded as specific hex values)
            float scale_factor = 0.31;
            uint8_t volume = (float) (vol * 100 * scale_factor);
            max98090_reg_recv_vol_ctrl_t vol = {0};

            vol.rcvlm = mute;
            vol.rcvlvol = volume;

            bsp_i2c_Send(i2CInst, MAX98090_I2C_ADDR, MAX98090_REG_RECV_VOL_CTRL, 1, (uint8_t *) &vol, 1);
        }
            break;

        case CodecParamsMAX98090::OutputPath::Loudspeaker: {
            // Scale input volume(range 0 - 100) to MAX98090 range(decibels hardcoded as specific hex values)
            float scale_factor = 0.39;
            uint8_t volume = (float) (vol * 100 * scale_factor) + 0x18;

            max98090_reg_lspk_vol_ctrl_t lvol = {0};
            max98090_reg_rspk_vol_ctrl_t rvol = {0};

            lvol.splm = mute;
            rvol.sprm = mute;
            lvol.spvoll = volume;
            rvol.spvolr = volume;

            bsp_i2c_Send(i2CInst, MAX98090_I2C_ADDR, MAX98090_REG_LSPK_VOL_CTRL, 1, (uint8_t *) &lvol,
                         1);
            bsp_i2c_Send(i2CInst, MAX98090_I2C_ADDR, MAX98090_REG_RSPK_VOL_CTRL, 1, (uint8_t *) &rvol,
                         1);
        }
            break;

        default:
            return CodecRetCode::InvalidArgument;
    }

    currentParams.outVolume = vol;
    return CodecRetCode::Success;
}

CodecRetCode CodecMAX98090::SetInputGain(const float gain) {
    if (gain > 10) {
        return CodecRetCode::InvalidArgument;
    }

    max98090_reg_lrec_dig_gain_t lgain = {0};
    lgain.avl = 3; // fine gain - 0dB
    lgain.avlg = gain; // coarse gain
    bsp_i2c_Send(i2CInst, MAX98090_I2C_ADDR, MAX98090_REG_LREC_DIG_GAIN, 1, (uint8_t *) &lgain, 1);

    //coarse gain - 18dB, fine gain - 0dB
    max98090_reg_rrec_dig_gain_t rgain = {0};
    rgain.avr = 3; // fine gain - 0dB
    rgain.avrg = gain; // coarse gain
    bsp_i2c_Send(i2CInst, MAX98090_I2C_ADDR, MAX98090_REG_RREC_DIG_GAIN, 1, (uint8_t *) &rgain, 1);

    return CodecRetCode::Success;
}

// This two links are very helpful for calculating proper data format:
// https://www.mathsisfun.com/binary-decimal-hexadecimal-converter.html
// http://faculty.weber.edu/fonbrown/EE1000/Chapter%202.pdf
CodecRetCode CodecMAX98090::WriteFilterCoeff(const float coeff, const uint8_t basereg) {
    int32_t decimal = coeff * (float) (1 << 20); // multiply by 2^20

    volatile uint32_t fractional = decimal & 0xFFFFF;
    decimal >>= 20;

    uint8_t byte1 = (decimal << 4) | (fractional >> 16);

    uint8_t byte2 = fractional >> 8;
    uint8_t byte3 = fractional & 0xFF;

    bsp_i2c_Send(i2CInst, MAX98090_I2C_ADDR, basereg, 1, &byte1, 1);
    bsp_i2c_Send(i2CInst, MAX98090_I2C_ADDR, basereg + 1, 1, &byte2, 1);
    bsp_i2c_Send(i2CInst, MAX98090_I2C_ADDR, basereg + 2, 1, &byte3, 1);

    return CodecRetCode::Success;
}

CodecRetCode CodecMAX98090::MicBias(const bool enable) {
    max98090_reg_bias_mode_t biasmode = {0};
    // BIAS created by bandgap reference
    biasmode.biasmode = 1;
    bsp_i2c_Send(i2CInst, MAX98090_I2C_ADDR, MAX98090_REG_BIAS_CTRL, 1, (uint8_t *) &biasmode, 1);

    // Turn on microphone bias(needed for jack detection and powering external analog headphones mic)
    uint8_t mask = 0x10; // Set 4th bit (mic bias enable/disable)
    bsp_i2c_ModifyReg(i2CInst, MAX98090_I2C_ADDR, MAX98090_REG_INPUT_ENABLE, 1, mask, enable, 1);

    return CodecRetCode::Success;
}

CodecRetCode CodecMAX98090::Reset() {
    max98090_reg_swreset_t reset = {0};
    reset.swreset = 1;
    bsp_i2c_Send(i2CInst, MAX98090_I2C_ADDR, MAX98090_REG_SWRESET, 1, (uint8_t *) &reset, 1);
    return CodecRetCode::Success;
}

CodecRetCode CodecMAX98090::SetOutputPath(const CodecParamsMAX98090::OutputPath path) {
    Reset();
    currentParams.outputPath = path;
    Start(currentParams);

    return CodecRetCode::Success;
}

CodecRetCode CodecMAX98090::SetInputPath(const CodecParamsMAX98090::InputPath path) {
    Reset();
    currentParams.inputPath = path;
    Start(currentParams);

    return CodecRetCode::Success;
}

CodecRetCode CodecMAX98090::SetMute(const bool enable) {
    // Set/clr 7th bit in register which controls mute/unmute
    uint8_t mask = 0x80;
    uint8_t regl,regr = 0;

    switch(currentParams.outputPath)
    {
        case CodecParamsMAX98090::OutputPath ::Headphones:
        {
            regl = MAX98090_REG_LHP_VOL_CTRL;
            regr = MAX98090_REG_RHP_VOL_CTRL;
        }
            break;

        case CodecParamsMAX98090::OutputPath ::Earspeaker:
        {
            regl = MAX98090_REG_RECV_VOL_CTRL;
            regr = 0;
        }
            break;

        case CodecParamsMAX98090::OutputPath ::Loudspeaker:
        {
            regl = MAX98090_REG_LSPK_VOL_CTRL;
            regr = MAX98090_REG_RSPK_VOL_CTRL;
        }
            break;

        default:
            return CodecRetCode ::InvalidArgument;
    }

    if(regl){
        bsp_i2c_ModifyReg(i2CInst,MAX98090_I2C_ADDR,regl,1,mask,enable,1);
    }
    if(regr){
        bsp_i2c_ModifyReg(i2CInst,MAX98090_I2C_ADDR,regr,1,mask,enable ,1);
    }
    return CodecRetCode ::Success;
}

std::optional<uint32_t> CodecMAX98090::Probe() {
    uint8_t id = 0;
    bsp_i2c_Send(BOARD_GetI2CInstance(),MAX98090_I2C_ADDR, MAX98090_REG_REVID,1,&id,1);
    return id;
}