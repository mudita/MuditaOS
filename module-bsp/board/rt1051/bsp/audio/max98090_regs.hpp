// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#ifndef BSP_AUDIOCODEC_MAX98090_H_
#define BSP_AUDIOCODEC_MAX98090_H_

#include <memory>

// Device revision
#define MAX98090_REG_REVID 0xFF
typedef struct
{
    uint8_t revid;
} max98090_reg_revid_t;

// Software reset register
#define MAX98090_REG_SWRESET 0x00
typedef struct
{
    uint8_t unused : 7;
    uint8_t swreset : 1;
} max98090_reg_swreset_t;

// Device Status Interrupt Register
// Check Table 85. for more info.
#define MAX98090_REG_DEVICE_STATUS 0x01
typedef struct
{
    uint8_t drcclp : 1;
    uint8_t drcact : 1;
    uint8_t jdet : 1;
    uint8_t unused : 2;
    uint8_t ulk : 1;
    uint8_t sld : 1;
    uint8_t cld : 1;
} max98090_reg_devstat_t;

// Device Status Interrupt Mask Register
// Check Table 86. for more info.
#define MAX98090_REG_DEVICE_STATUS_MASK 0x03
typedef struct
{
    uint8_t idrcclp : 1;
    uint8_t idrcact : 1;
    uint8_t ijdet : 1;
    uint8_t unused : 2;
    uint8_t iulk : 1;
    uint8_t isld : 1;
    uint8_t icld : 1;
} max98090_reg_devstat_mask_t;

// Device shutdown register
// Check Table 6. for more info.
#define MAX98090_REG_DEVICE_SHUTDOWN 0x45
typedef struct
{
    uint8_t unused : 7;
    uint8_t shdn : 1;
} max98090_reg_shutdown_t;

// Input enable register
#define MAX98090_REG_INPUT_ENABLE 0x3E
typedef struct
{
    uint8_t adlen : 1;
    uint8_t adren : 1;
    uint8_t lineben : 1;
    uint8_t lineaen : 1;
    uint8_t mben : 1;
    uint8_t unused : 3;
} max98090_reg_inputenable_t;

// Output enable register
#define MAX98090_REG_OUTPUT_ENABLE 0x3F
typedef struct
{
    uint8_t dalen : 1;
    uint8_t daren : 1;
    uint8_t rcvren : 1;
    uint8_t rcvlen : 1;
    uint8_t splen : 1;
    uint8_t spren : 1;
    uint8_t hplen : 1;
    uint8_t hpren : 1;
} max98090_reg_outputenable_t;

// Line Input Level Configuration Register
#define MAX98090_REG_LINE_INPUT_LEVEL_CONF 0x0E
typedef struct
{
    uint8_t linbpga : 3;
    uint8_t linapga : 3;
    uint8_t mixg246 : 1;
    uint8_t mixg135 : 1;
} max98090_reg_line_inp_lvl_t;

// Microphone 1 enable and level configuration register
#define MAX98090_REG_MIC1_ENABLE_LEVEL_CONF 0x10
typedef struct
{
    uint8_t pgam : 5;
    uint8_t pa1en : 2;
    uint8_t unused : 1;
} max98090_reg_mic1ena_lvlctl_t;

// Microphone 2 enable and level configuration register
#define MAX98090_REG_MIC2_ENABLE_LEVEL_CONF 0x11
typedef struct
{
    uint8_t pgam : 5;
    uint8_t pa2en : 2;
    uint8_t unused : 1;
} max98090_reg_mic2ena_lvlctl_t;

// Microphone Bias Level Configuration Register
#define MAX98090_REG_MIC_BIAS 0x12
typedef struct
{
    uint8_t mbvsel : 2;
    uint8_t unused : 6;
} max98090_reg_micbias_t;

// Digital Microphone Enable
// For more info check Table 13.
#define MAX98090_REG_DIG_MIC_ENABLE 0x13
typedef struct
{
    uint8_t digmicl : 1;
    uint8_t digmicr : 1;
    uint8_t unused : 2;
    uint8_t dmicclk : 3;
    uint8_t unused2 : 1;
} max98090_reg_digmic_enable_t;

// Digital Microphone Configuration
// Check Table 14. for more info.
// Check MAX98090 datasheet Table 15-16 for recommended filter configuration
#define MAX98090_REG_DIG_MIC_CONF 0x14
typedef struct
{
    uint8_t dmicfreq : 2;
    uint8_t unused : 2;
    uint8_t dmiccomp : 4;

} max98090_reg_digmic_conf_t;

/*  Left ADC Mixer Input Configuration Register
    6 R/W 0 Selects microphone input 2 to left ADC mixer.
    5 R/W 0 Selects microphone input 1 to left ADC mixer.
    4 R/W 0 Selects line input B to left ADC mixer.
    3 R/W 0 Selects line input A to left ADC mixer.
    2 R/W 0 Selects IN5/IN6 differential input direct to left ADC mixer (WLP only).
    1 R/W 0 Selects IN3/IN4 differential input direct to left ADC mixer.
    0 R/W 0 Selects IN1/IN2 differential input direct to left ADC mixer
*/
#define MAX98090_REG_LADC_MIXER_INPUT 0x15
typedef struct
{
    uint8_t mixadl : 7;
    uint8_t unused : 1;
} max98090_reg_ladc_mix_input_t;

/*  Right ADC Mixer Input Configuration Register
    6 R/W 0 Selects microphone input 2 to right ADC mixer.
    5 R/W 0 Selects microphone input 1 to right ADC mixer.
    4 R/W 0 Selects line input B to right ADC mixer.
    3 R/W 0 Selects line input A to right ADC mixer.
    2 R/W 0 Selects IN5/IN6 differential input direct to right ADC mixer (WLP only).
    1 R/W 0 Selects IN3/IN4 differential input direct to right ADC mixer.
    0 R/W 0 Selects IN1/IN2 differential input direct to right ADC mixer
*/
#define MAX98090_REG_RADC_MIXER_INPUT 0x16
typedef struct
{
    uint8_t mixadr : 7;
    uint8_t unused : 1;
} max98090_reg_radc_mix_input_t;

/*  Left Record Path Digital Gain Configuration Register
    Table 32.
*/
#define MAX98090_REG_LREC_DIG_GAIN 0x17
typedef struct
{
    uint8_t avl : 4;
    uint8_t avlg : 3;
    uint8_t unused : 1;
} max98090_reg_lrec_dig_gain_t;

/*  Right Record Path Digital Gain Configuration Register
    Table 33.
*/
#define MAX98090_REG_RREC_DIG_GAIN 0x18
typedef struct
{
    uint8_t avr : 4;
    uint8_t avrg : 3;
    uint8_t unused : 1;
} max98090_reg_rrec_dig_gain_t;

// System Master Clock (MCLK) Prescaler Configuration Register
// Check MAX98090 datasheet Table 34 for available configurations
#define MAX98090_REG_SYSTEM_MASTER_CLOCK 0x1B
typedef struct
{
    uint8_t unused : 4;
    uint8_t psclk : 2;
    uint8_t unused2 : 2;

} max98090_reg_system_masterclock_conf_t;

// Master Mode Clock Configuration Register
// Check MAX98090 datasheet Table 35 for available configurations
#define MAX98090_REG_SYSTEM_MASTER_MODE 0x21
typedef struct
{
    uint8_t bsel : 3;
    uint8_t unused : 4;
    uint8_t mas : 1;

} max98090_reg_master_mode_conf_t;

// Clock Mode Configuration Register
// Check MAX98090 datasheet Table 39 for available configurations
#define MAX98090_REG_CLOCK_MODE 0x1C
typedef struct
{
    uint8_t usemi : 1;
    uint8_t unused : 3;
    uint8_t freq : 4;
} max98090_reg_clock_mode_t;

// Manual Clock Ratio Configuration Register (NI MSB)
// Check MAX98090 datasheet Table 40 for available configurations
#define MAX98090_REG_NI_MSB 0x1D
typedef struct
{
    uint8_t ni : 7;
    uint8_t unused : 1;
} max98090_reg_manual_clock_ratio_NI_MSB_t;

// Manual Clock Ratio Configuration Register (NI LSB)
// Check MAX98090 datasheet Table 41 for available configurations
#define MAX98090_REG_NI_LSB 0x1E
typedef struct
{
    uint8_t ni;
} max98090_reg_manual_clock_ratio_NI_LSB_t;

// Manual Clock Ratio Configuration Register (MI MSB)
// Check MAX98090 datasheet Table 42 for available configurations
#define MAX98090_REG_MI_MSB 0x1F
typedef struct
{
    uint8_t mi;
} max98090_reg_manual_clock_ratio_MI_MSB_t;

// Manual Clock Ratio Configuration Register (MI LSB)
// Check MAX98090 datasheet Table 43 for available configurations
#define MAX98090_REG_MI_LSB 0x20
typedef struct
{
    uint8_t mi;
} max98090_reg_manual_clock_ratio_MI_LSB_t;

// Left Headphone Mixer Configuration Register
// Check MAX98090 datasheet Table 69 for available path configurations
#define MAX98090_REG_LHP_MIXER_CONF 0x29
typedef struct
{
    uint8_t mixhpl : 6;
    uint8_t unused : 2;

} max98090_reg_lhp_mixer_t;

// Right Headphone Mixer Configuration Register
// Check MAX98090 datasheet Table 69 for available path configurations
#define MAX98090_REG_RHP_MIXER_CONF 0x2A
typedef struct
{
    uint8_t mixhpr : 6;
    uint8_t unused : 2;

} max98090_reg_rhp_mixer_t;

// Headphone Mixer Control and Gain Register
// Check MAX98090 datasheet Table 71 for available path configurations
#define MAX98090_REG_HP_MIX_CONF 0x2B
typedef struct
{
    uint8_t mixhplg : 2;
    uint8_t mixhprg : 2;
    uint8_t mixhplsel : 1;
    uint8_t mixhprsel : 1;
    uint8_t unused : 2;

} max98090_reg_hpmix_conf_t;

// Digital Audio Interface (DAI) Input/Output Configuration Register
// Check MAX98090 datasheet Table 44 for available path configurations
#define MAX98090_REG_INOUT_PATH_CONF 0x25
typedef struct
{
    uint8_t sdien : 1;
    uint8_t sdoen : 1;
    uint8_t hizoff : 1;
    uint8_t dmono : 1;
    uint8_t lben : 1;
    uint8_t lten : 1;
    uint8_t unused : 2;

} max98090_reg_inoutpath_conf_t;

// Digital Audio Interface (DAI) Format Configuration Register
// Check MAX98090 datasheet Table 46 for available configurations
#define MAX98090_REG_DAI_FORMAT_CONF 0x22
typedef struct
{
    uint8_t ws : 2;
    uint8_t dly : 1;
    uint8_t bci : 1;
    uint8_t wci : 1;
    uint8_t rj : 1;
    uint8_t unused : 2;

} max98090_reg_dai_format_conf_t;

// Playback Gain and Level Configuration Register
// Check MAX98090 datasheet Table 49 for available configurations
#define MAX98090_REG_PLAYBACK_GAIN_LEVEL_CONF 0x27
typedef struct
{
    uint8_t dv : 4;
    uint8_t dvg : 2;
    uint8_t unused : 1;
    uint8_t dvm : 1;

} max98090_reg_playback_gainlevel_conf_t;

// DSP Biquad Filter Enable Register
// Check MAX98090 datasheet Table 50 for available configurations
#define MAX98090_REG_DSP_BIQUAD_FILTER_ENABLE 0x41
typedef struct
{
    uint8_t eq7banden : 1;
    uint8_t eq5banden : 1;
    uint8_t eq3banden : 1;
    uint8_t recbqen : 1;
    uint8_t unused : 4;

} max98090_reg_dsp_biquadfilter_enable_t;

// Parametric Equalizer Playback Level Configuration Register
// Check MAX98090 datasheet Table 51 for available configurations
#define MAX98090_REG_EQ_PLAYBACK_LEVEL_CONF 0x28
typedef struct
{
    uint8_t dveq : 4;
    uint8_t eqclp : 1;
    uint8_t unused : 3;

} max98090_reg_eqplayback_level_conf_t;

// Playback DSP Filter Configuration Register
// Check MAX98090 datasheet Table 57 for available configurations
#define MAX98090_REG_PLAYBACK_DSP_FILTER_CONF 0x26
typedef struct
{
    uint8_t unused : 4;
    uint8_t dhf : 1;
    uint8_t dhpf : 1;
    uint8_t ahpf : 1;
    uint8_t mode : 1;
} max98090_reg_playback_dspfilter_conf_t;

// Master Clock Quick Setup Register
// Check MAX98090 datasheet Table 36 for available configurations
#define MAX98090_REG_MASTER_CLOCK_QUICK_SETUP 0x04
typedef struct
{
    uint8_t FS256 : 1;
    uint8_t unused : 1;
    uint8_t M11P2896 : 1;
    uint8_t M12 : 1;
    uint8_t M12P288 : 1;
    uint8_t M13 : 1;
    uint8_t M19P2 : 1;
    uint8_t M26 : 1;

} max98090_reg_masterclock_quick_setup_t;

// Sample Rate Quick Setup Register
// Check MAX98090 datasheet Table 37 for available configurations
#define MAX98090_REG_MASTER_SAMPLE_RATE_QUICK_SETUP 0x05
typedef struct
{
    uint8_t SR_8K : 1;
    uint8_t SR_16K : 1;
    uint8_t SR_44K1 : 1;
    uint8_t SR_48K : 1;
    uint8_t SR_32K : 1;
    uint8_t SR_96K : 1;
    uint8_t unused : 2;

} max98090_reg_master_samplerate_quick_setup_t;

// Digital Audio Interface (DAI) Quick Setup Register
// Check MAX98090 datasheet Table 79 for available configurations
#define MAX98090_REG_DAI_QUICK_SETUP 0x06
typedef struct
{
    uint8_t i2ss : 1;
    uint8_t i2sm : 1;
    uint8_t ljs : 1;
    uint8_t ljm : 1;
    uint8_t rjs : 1;
    uint8_t rjm : 1;
    uint8_t unused : 2;

} max98090_reg_dai_quick_setup_t;

// Playback Path Quick Setup Register
// Check MAX98090 datasheet Table 80 for available configurations
#define MAX98090_REG_PLAYBACK_QUICK_SETUP 0x07
typedef struct
{
    uint8_t unused : 4;
    uint8_t dig2lout : 1;
    uint8_t dig2spk : 1;
    uint8_t dig2ear : 1;
    uint8_t dig2hp : 1;

} max98090_reg_playback_quick_setup_t;

// Analog Microphone/Direct Input to Record Path Quick Setup Register
// Check MAX98090 datasheet Table 81 for available configurations
#define MAX98090_REG_ANALOG_MIC_TO_RECORD_QUICK 0x08
typedef struct
{
    uint8_t unused : 1;
    uint8_t in56_dadc : 1;
    uint8_t in34_dadc : 1;
    uint8_t in12_dadc : 1;
    uint8_t unused2 : 2;
    uint8_t in34mic2 : 1;
    uint8_t in12mic1 : 1;

} max98090_reg_analog_to_record_quick_t;

// Line Input to Record Path Quick Setup Register
// Check MAX98090 datasheet Table 82 for available configurations
#define MAX98090_REG_LINE_INPUT_TO_RECORD_QUICK 0x09
typedef struct
{
    uint8_t unused : 3;
    uint8_t in65db : 1;
    uint8_t in34dan : 1;
    uint8_t in56sab : 1;
    uint8_t in34sab : 1;
    uint8_t in12sab : 1;

} max98090_reg_input_to_record_quick_t;

// Left Headphone Amplifier Volume Control Register
// Check MAX98090 datasheet Table 72 for available configurations
#define MAX98090_REG_LHP_VOL_CTRL 0x2C
typedef struct
{
    uint8_t hpvoll : 5;
    uint8_t unused : 2;
    uint8_t hplm : 1;

} max98090_reg_lhp_vol_ctrl_t;

// Right Headphone Amplifier Volume Control Register
// Check MAX98090 datasheet Table 73 for available configurations
#define MAX98090_REG_RHP_VOL_CTRL 0x2D
typedef struct
{
    uint8_t hpvolr : 5;
    uint8_t unused : 2;
    uint8_t hprm : 1;

} max98090_reg_rhp_vol_ctrl_t;

// Left Speaker Amplifier Volume Control Register
// Check MAX98090 datasheet Table 67 for available configurations
#define MAX98090_REG_LSPK_VOL_CTRL 0x31
typedef struct
{
    uint8_t spvoll : 6;
    uint8_t unused : 1;
    uint8_t splm : 1;

} max98090_reg_lspk_vol_ctrl_t;

// Right Speaker Amplifier Volume Control Register
// Check MAX98090 datasheet Table 68 for available configurations
#define MAX98090_REG_RSPK_VOL_CTRL 0x32
typedef struct
{
    uint8_t spvolr : 6;
    uint8_t unused : 1;
    uint8_t sprm : 1;

} max98090_reg_rspk_vol_ctrl_t;

// Receiver and Left Line Output Volume Control Register
// Check MAX98090 datasheet Table 60 for available configurations
#define MAX98090_REG_RECV_VOL_CTRL 0x39
typedef struct
{
    uint8_t rcvlvol : 5;
    uint8_t unused : 2;
    uint8_t rcvlm : 1;

} max98090_reg_recv_vol_ctrl_t;

// Bias Control Register
// Check MAX98090 datasheet Table 3 for available configurations
#define MAX98090_REG_BIAS_CTRL 0x42
typedef struct
{
    uint8_t biasmode : 1;
    uint8_t unused : 7;

} max98090_reg_bias_mode_t;

// DAC and Headphone Performance Mode Control Register
// Check MAX98090 datasheet Table 4 for available configurations
#define MAX98090_REG_DACHP_PERF_MODE 0x43
typedef struct
{
    uint8_t dachp : 1;
    uint8_t perfmode : 1;
    uint8_t unused : 6;

} max98090_reg_dachp_perfmode_t;

// ADC Performance Mode Control Register
// Check MAX98090 datasheet Table 5 for available configurations
#define MAX98090_REG_ADC_PERF_MODE 0x44
typedef struct
{
    uint8_t adchp : 1;
    uint8_t adcdither : 1;
    uint8_t osr128 : 1;
    uint8_t unused : 5;
} max98090_reg_adcperf_mode_t;

#define MAX98090_I2C_ADDR 0x10

#endif /* BSP_AUDIOCODEC_MAX98090_H_ */
