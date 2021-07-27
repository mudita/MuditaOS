// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#ifndef BSP_AUDIOCODEC_AW8898_REGS_H_
#define BSP_AUDIOCODEC_AW8898_REGS_H_

#include <memory>

// Chip ID Register
#define AW8898_REG_ID          0x00
typedef struct
{
    uint16_t idcode;
} aw8898_reg_idcode_t;

// System Status Register
#define AW8898_REG_SYSST        0x01
typedef struct
{
    uint8_t unused1 : 1;
    uint8_t uvls : 1;
    uint8_t adps : 1;
    uint8_t unused2 : 1;
    uint8_t bstocs : 1;
    uint8_t ovps : 1;
    uint8_t bsts : 1;
    uint8_t sws : 1;
    uint8_t clips : 1;
    uint8_t unused3 : 1;
    uint8_t noclks : 1;
    uint8_t clks : 1;
    uint8_t ocds : 1;
    uint8_t otls : 1;
    uint8_t oths : 1;
    uint8_t plls : 1;
} aw8898_reg_sysst_t;

// System Interrupt Register
#define AW8898_REG_SYSINT       0x02
typedef struct
{
    uint8_t unused1 : 1;
    uint8_t uvli : 1;
    uint8_t adpi : 1;
    uint8_t unused2 : 1;
    uint8_t bstoci : 1;
    uint8_t ovpi : 1;
    uint8_t bsti : 1;
    uint8_t swi : 1;
    uint8_t clipi : 1;
    uint8_t unused3 : 1;
    uint8_t noclki : 1;
    uint8_t clki : 1;
    uint8_t ocdi : 1;
    uint8_t otli : 1;
    uint8_t othi : 1;
    uint8_t plli : 1;
} aw8898_reg_sysint_t;

// System Interrupt Mask Register
#define AW8898_REG_SYSINTM      0x03
typedef struct
{
    uint8_t unused1 : 1;
    uint8_t uvlm : 1;
    uint8_t adpm : 1;
    uint8_t unused2 : 1;
    uint8_t bstocm : 1;
    uint8_t ovpm : 1;
    uint8_t bstm : 1;
    uint8_t swm : 1;
    uint8_t clipm : 1;
    uint8_t unused3 : 1;
    uint8_t noclkm : 1;
    uint8_t clkm : 1;
    uint8_t ocdm : 1;
    uint8_t otlm : 1;
    uint8_t othm : 1;
    uint8_t pllm : 1;
} aw8898_reg_sysintm_t;

// System Control Register
#define AW8898_REG_SYSCTRL      0x04
typedef struct 
{
    uint8_t unused1 : 6;
    uint8_t intmode : 2;
    uint8_t rcv_mode : 1;
    uint8_t i2sen : 1;
    uint8_t wsinv : 1;
    uint8_t bckinv : 1;
    uint8_t ipll : 1;
    uint8_t unused2 : 1;
    uint8_t amppd : 1;
    uint8_t pwdn : 1;
} aw8898_reg_sysctrl_t;

// i2S Interface Control Register
#define AW8898_REG_I2SCTRL      0x05
typedef struct
{
    uint8_t unused1 : 2;
    uint8_t inplev : 1;
    uint8_t unused2 : 1;
    uint8_t chsel : 2;
    uint8_t i2smd : 2;
    uint8_t i2sfs : 2;
    uint8_t i2sbck : 2;
    uint8_t i2ssr : 4;
} aw8898_reg_i2sctrl_t;

// I2S Tx Channel Configuration Register
#define AW8898_REG_I2STXCFG     0x06
typedef struct
{
    uint8_t fsync_type : 1;
    uint8_t slot_num : 1;
    uint8_t i2s_tx_slotvld : 2;
    uint8_t i2s_rx_slotvld : 4;
    uint8_t unused1 : 2;
    uint8_t drvstren : 1;
    uint8_t dohz : 1;
    uint8_t unused2 : 3;
    uint8_t i2stxen : 1;
} aw8898_reg_i2stxcfg_t;

// PWM Control Regoster
#define AW8898_REG_PWMCTRL      0x08
typedef struct
{
    uint16_t unused : 13;
    uint8_t hdcce : 1;
    uint8_t hmute : 1;
} aw8898_reg_pwmctrl_t;

// Hardware AGC Configuration Register 1
#define AW8898_REG_HAGCCFG1     0x09
typedef struct
{
    uint8_t rvth : 8;
    uint8_t avth : 8;
} aw8898_reg_hagccfg1_t;

// Hardware AGC Configuration Register 2
#define AW8898_REG_HAGCCFG2     0x0A
typedef struct
{
    uint16_t atth : 16;
} aw8898_reg_hagccfg2_t;

// Hardware AGC Configuration Register 3
#define AW8898_REG_HAGCCFG3     0x0B
typedef struct
{
    uint16_t rtth : 16;
} aw8898_reg_hagccfg3_t;

// Hardware AGC Configuration Register 4
#define AW8898_REG_HAGCCFG4     0x0C
typedef struct
{
    uint8_t unused : 8;
    uint8_t holdth : 8;
} aw8898_reg_hagccfg4_t;
// Hardware AGC Configuration Register 5
#define AW8898_REG_HAGCCFG5     0x0D
typedef struct
{
    uint8_t unused1 : 7;
    uint8_t hagce : 1;
    uint8_t unused2 : 8;
} aw8898_reg_hagccfg5_t;

// Hardware AGC Configuration Register 6
#define AW8898_REG_HAGCCFG6     0x0E

// Hardware AGC Configuration Register 7
#define AW8898_REG_HAGCCFG7     0x0F
typedef struct
{
    uint8_t volume : 8;
    uint8_t unused : 8;
} aw8898_reg_hagccfg7_t;

// 
#define AW8898_REG_HAGCST       0x10
typedef struct
{
    uint16_t unused : 12;
    uint8_t bstvout : 4;
} aw8898_reg_hagcst_t;

//
#define AW8898_REG_DBGCTRL      0x20

// I2S Configuration Register
#define AW8898_REG_I2SCFG       0x21
typedef struct
{
    uint16_t unused : 15;
    uint8_t i2rxen : 1;
} aw8898_reg_i2scfg_t;

//
#define AW8898_REG_I2SSTAT      0x22
typedef struct
{
    uint16_t unused : 13;
    uint8_t dpstat : 1;
    uint8_t i2srovs : 1;
    uint8_t i2stovs : 1;
} aw8898_reg_i2sstat_t;

//
#define AW8898_REG_I2SCAPCNT    0x23

// Analog General Control Register
#define AW8898_REG_GENCTRL      0x60
typedef struct
{
    uint8_t unused1 : 3;
    uint8_t bst_tdeg2 : 3;
    uint8_t unused2 : 1;
    uint8_t bst_en : 1;
    uint8_t bstilimit : 3;
    uint8_t bstvout : 4;
} aw8898_reg_genctrl_t;

// Boost Control Register 1
#define AW8898_REG_BSTCTRL1     0x61
typedef struct
{
    uint8_t unused1 : 2;
    uint8_t bst_rth : 6;
    uint8_t unused2 : 2;
    uint8_t bst_ath : 6;
} aw8898_reg_bstctrl1_t;

// Boost Control Register 2
#define AW8898_REG_BSTCTRL2     0x62
typedef struct
{
    uint16_t unused : 10;
    uint8_t bst_mode : 4;
    uint8_t bst_tdeg : 2;
} aw8898_reg_bstctrl2_t;

//
#define AW8898_REG_PLLCTRL1     0x63

//
#define AW8898_REG_PLLCTRL2     0x64

//
#define AW8898_REG_TESTCTRL     0x65

//
#define AW8898_REG_AMPDBG1      0x66

//
#define AW8898_REG_AMPDBG2      0x67

//
#define AW8898_REG_BSTDBG1      0x68

//
#define AW8898_REG_CDACTRL1     0x69

//
#define AW8898_REG_CDACTRL2     0x6A

//
#define AW8898_REG_TESTCTRL2    0x6B

//
#define AW8898_REG_MAX          0x6F


#define AW8898_SW_RESET_MAGIC   0x55AA

#define AW8898_I2C_ADDR 0x34

#endif /* BSP_AUDIOCODEC_AW8898_REGS_H_ */
