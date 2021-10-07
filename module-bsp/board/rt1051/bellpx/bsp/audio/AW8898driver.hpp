// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "AW8898regs.hpp"
#include "drivers/i2c/DriverI2C.hpp"
#include "drivers/gpio/DriverGPIO.hpp"

extern "C"
{
#include "fsl_common.h"
}

#include <memory>

#include <cstdint>

namespace bsp::audio::AW8898
{
    extern std::shared_ptr<drivers::DriverI2C> i2c;
    extern std::shared_ptr<drivers::DriverGPIO> gpio;
    extern drivers::I2CAddress i2cAddr;

    constexpr inline auto LogicLow            = 0;
    constexpr inline auto LogicHigh           = 1;
    constexpr inline auto OperationWaitTimeMS = 2;

    constexpr inline auto AW8898_CHIP_ID = 0x1702;

    enum class aw_i2s_channel
    {
        CHSEL_LEFT  = 0,
        CHSEL_RIGHT = 1,
        CHSEL_MONO  = 2,
    };

    enum class aw_i2s_frequency
    {
        FREQUENCY_08K = 0,
        FREQUENCY_11K = 1,
        FREQUENCY_16K = 2,
        FREQUENCY_22K = 3,
        FREQUENCY_24K = 4,
        FREQUENCY_32K = 5,
        FREQUENCY_44K = 6,
        FREQUENCY_48K = 7,

    };

    enum class aw_i2s_width
    {
        WIDTH_16BITS = 0,
        WIDTH_24BITS = 1,
        WIDTH_32BITS = 2,

    };

    enum class aw_i2s_fs
    {
        I2SBCK_32FS = 0,
        I2SBCK_48FS = 1,
        I2SBCK_64FS = 2,
    };

    enum class aw_sel_mode
    {
        OFF_MODE   = 0,
        SPK_MODE   = 1,
        VOICE_MODE = 2,
        MUSIC_MODE = 3,
    };

    enum class aw_codec_mode
    {
        AW_ENCODE       = 0,
        AW_DECODE       = 1,
        AW_MODE_BOTH    = 2,
        AW_MODE_LINE_IN = 3,
    };

    enum class aw_ctrl
    {
        STOP  = 0,
        START = 1,
    };

    status_t WriteReg(std::uint8_t reg, std::uint16_t val);
    status_t ReadReg(std::uint8_t reg, std::uint16_t *val);
    status_t ModifyReg(std::uint8_t reg, std::uint16_t mask, std::uint16_t val);

    status_t HwReset(void);
    void SoftReset(void);

    status_t ReadChipid(void);
    status_t Init(const aw_i2s_channel chsel, aw_i2s_frequency rate);

    status_t HwParams(aw_i2s_channel chsel, aw_i2s_frequency rate, aw_i2s_width width, aw_i2s_fs fs);
    status_t RunPwd(bool pwd);
    status_t RunMute(bool mute);
    void LoadRegCfg(void);

    void ColdStart(void);

    status_t Stop(void);

    status_t SmartpaCfg(bool play_flag);
    status_t CtrlState(aw_codec_mode mode, aw_ctrl aw_ctrl);

    int SetMode(aw_sel_mode mode);
    bool CheckPllStatus(void);

    void ReadAllReg(void);
    void HAL_Delay(std::uint32_t count);

    status_t SetVolume(std::uint8_t gain);
    status_t GetVolume(std::uint8_t *gain);

    status_t Start(void);
}; // namespace bsp::audio::AW8898
