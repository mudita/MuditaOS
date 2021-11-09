// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AW8898driver.hpp"
#include "board/BoardDefinitions.hpp"
#include <log/log.hpp>

extern "C"
{
#include "FreeRTOS.h"
#include "task.h"
}

#include <memory>

namespace
{
    constexpr auto softResetCommand = 0x55aa;
    constexpr auto readRetries      = 5;
    constexpr auto expectedReadSize = sizeof(std::uint16_t);
} // namespace

namespace bsp::audio::AW8898
{
    std::shared_ptr<drivers::DriverI2C> i2c;
    std::shared_ptr<drivers::DriverGPIO> gpio;
    drivers::I2CAddress i2cAddr;

    inline void FlipBytes(uint16_t *val)
    {
        std::uint16_t tmp = *val << 8;
        *val              = (*val >> 8) | tmp;
    }

    // internal typedefs & variables

    struct aw8898_reg_cfg
    {
        std::uint8_t addr;
        std::uint16_t data;
    };

    enum class aw8898_init
    {
        AW8898_INIT_ST = 0,
        AW8898_INIT_OK = 1,
        AW8898_INIT_NG = 2,
    };

    struct aw8898
    {
        aw8898_init init;
        aw_sel_mode mode;
    };

    struct aw8898 g_aw8898 = {
        .init = aw8898_init::AW8898_INIT_ST,
        .mode = aw_sel_mode::OFF_MODE,
    };

    namespace
    {
        constexpr std::size_t configurationSize                                      = 16;
        constexpr std::array<aw8898_reg_cfg, configurationSize> initialConfiguration = {{
            {0x06, 0x0330},
            {0x08, 0xa00e},
            {0x09, 0x424a},
            {0x0a, 0x03c2},
            {0x0b, 0x03c2},
            {0x0c, 0x3007},
            {0x0d, 0x011b},
            {0x0e, 0x0329},
            {0x20, 0x0001},
            {0x60, 0x1cbc},
            {0x61, 0x0f0e},
            {0x62, 0xf5b6},
            {0x63, 0x307f},
            {0x67, 0x007c},
            {0x69, 0x0245},
            {0x04, 0x0044},
        }};

        constexpr std::uint8_t availableRegisters[] = {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0f, 0x21, 0x60, 0x61, 0x62};
    } // namespace

    /*******************************************************************************
     * Code
     ******************************************************************************/
    status_t WriteReg(std::uint8_t reg, std::uint16_t val)
    {
        i2cAddr.subAddress = reg;
        std::uint16_t tval = val;
        FlipBytes(reinterpret_cast<std::uint16_t *>(&tval));
        auto sent = i2c->Write(i2cAddr, reinterpret_cast<std::uint8_t *>(&tval), expectedReadSize);
        if (sent == expectedReadSize)
            return kStatus_Success;
        else
            return kStatus_Fail;
    }

    status_t ReadReg(std::uint8_t reg, std::uint16_t *val)
    {
        std::uint16_t tval;

        i2cAddr.subAddress = reg;

        auto received = i2c->Read(i2cAddr, reinterpret_cast<std::uint8_t *>(&tval), expectedReadSize);
        if (received != expectedReadSize)
            return kStatus_Fail;

        FlipBytes(reinterpret_cast<std::uint16_t *>(&tval));
        *val = tval;

        return kStatus_Success;
    }

    status_t ModifyReg(std::uint8_t reg, std::uint16_t mask, std::uint16_t val)
    {
        status_t retval       = 0;
        std::uint16_t reg_val = 0;
        retval                = ReadReg(reg, &reg_val);
        if (retval != kStatus_Success) {
            return kStatus_Fail;
        }
        reg_val &= mask;
        reg_val |= val;
        retval = WriteReg(reg, reg_val);
        if (retval != kStatus_Success) {
            return kStatus_Fail;
        }
        return kStatus_Success;
    }

    status_t HwReset(void)
    {
        gpio->WritePin(static_cast<std::uint32_t>(BoardDefinitions::BELL_AUDIOCODEC_RSTN_PA_PIN),
                       LogicLow); // reset chip
        HAL_Delay(OperationWaitTimeMS);
        gpio->WritePin(static_cast<std::uint32_t>(BoardDefinitions::BELL_AUDIOCODEC_RSTN_PA_PIN),
                       LogicHigh); // clear reset
        HAL_Delay(OperationWaitTimeMS);

        g_aw8898.init = aw8898_init::AW8898_INIT_ST;

        return kStatus_Success;
    }

    void SoftReset(void)
    {
        WriteReg(AW8898_REG_ID, softResetCommand);

        LOG_DEBUG("enter %s end", __func__);
    }

    status_t ReadChipid(void)
    {
        std::uint8_t cnt      = 0;
        std::uint16_t reg_val = 0;

        while (cnt < readRetries) {
            ReadReg(AW8898_REG_ID, &reg_val);
            if (reg_val == AW8898_CHIP_ID) {
                LOG_DEBUG("this chip is Aw8898 chipid=0x%x", reg_val);
                return kStatus_Success;
            }
            LOG_DEBUG("%s: aw8898 chipid=0x%x error\n", __func__, reg_val);
            cnt++;
            HAL_Delay(OperationWaitTimeMS);
        }

        return kStatus_Fail;
    }

    status_t Init(const aw_i2s_channel chsel, aw_i2s_frequency rate)
    {
        status_t ret = kStatus_Success;

        HwReset();
        HAL_Delay(OperationWaitTimeMS);

        ret = ReadChipid();
        if (ret == kStatus_Fail) {
            LOG_DEBUG("please check hardware ad_pin && i2c config!!!");
            return ret;
        }

        RunPwd(false);
        HAL_Delay(OperationWaitTimeMS);

        SetMode(aw_sel_mode::SPK_MODE);

        HwParams(chsel, rate, aw_i2s_width::WIDTH_16BITS, aw_i2s_fs::I2SBCK_32FS);
        HAL_Delay(OperationWaitTimeMS);

        CtrlState(aw_codec_mode::AW_DECODE, aw_ctrl::START);

        Start();

        ReadAllReg();

        return ret;
    }

    void setChannelSelection(aw_i2s_channel chsel)
    {
        std::uint16_t reg_value;

        switch (chsel) {
        case aw_i2s_channel::CHSEL_LEFT:
            reg_value = AW8898_BIT_I2SCTRL_CHS_LEFT;
            break;
        case aw_i2s_channel::CHSEL_RIGHT:
            reg_value = AW8898_BIT_I2SCTRL_CHS_RIGHT;
            break;
        case aw_i2s_channel::CHSEL_MONO:
            reg_value = AW8898_BIT_I2SCTRL_CHS_MONO;
            break;
        default:
            reg_value = AW8898_BIT_I2SCTRL_CHS_MONO;
            LOG_DEBUG("%s: chsel can not support ", __func__);
            break;
        }

        // set chsel
        ModifyReg(AW8898_REG_I2SCTRL, AW8898_BIT_I2SCTRL_CHS_MASK, reg_value);
    }

    void setSampleRate(aw_i2s_frequency rate)
    {
        std::uint16_t reg_value;

        switch (rate) {
        case aw_i2s_frequency::FREQUENCY_08K:
            reg_value = AW8898_BIT_I2SCTRL_SR_8K;
            break;
        case aw_i2s_frequency::FREQUENCY_11K:
            reg_value = AW8898_BIT_I2SCTRL_SR_11K;
            break;
        case aw_i2s_frequency::FREQUENCY_16K:
            reg_value = AW8898_BIT_I2SCTRL_SR_16K;
            break;
        case aw_i2s_frequency::FREQUENCY_22K:
            reg_value = AW8898_BIT_I2SCTRL_SR_22K;
            break;
        case aw_i2s_frequency::FREQUENCY_24K:
            reg_value = AW8898_BIT_I2SCTRL_SR_24K;
            break;
        case aw_i2s_frequency::FREQUENCY_32K:
            reg_value = AW8898_BIT_I2SCTRL_SR_32K;
            break;
        case aw_i2s_frequency::FREQUENCY_44K:
            reg_value = AW8898_BIT_I2SCTRL_SR_44P1K;
            break;
        case aw_i2s_frequency::FREQUENCY_48K:
            reg_value = AW8898_BIT_I2SCTRL_SR_48K;
            break;
        default:
            reg_value = AW8898_BIT_I2SCTRL_SR_48K;
            LOG_DEBUG("%s: rate can not support", __func__);
            break;
        }

        // set rate
        ModifyReg(AW8898_REG_I2SCTRL, AW8898_BIT_I2SCTRL_SR_MASK, reg_value);
    }

    void setBitWidth(aw_i2s_width width)
    {
        std::uint16_t reg_value;

        switch (width) {
        case aw_i2s_width::WIDTH_16BITS:
            reg_value = AW8898_BIT_I2SCTRL_FMS_16BIT;
            break;
        case aw_i2s_width::WIDTH_24BITS:
            reg_value = AW8898_BIT_I2SCTRL_FMS_24BIT;
            break;
        case aw_i2s_width::WIDTH_32BITS:
            reg_value = AW8898_BIT_I2SCTRL_FMS_32BIT;
            break;
        default:
            reg_value = AW8898_BIT_I2SCTRL_FMS_16BIT;
            LOG_DEBUG("%s: width can not support ", __func__);
            break;
        }
        // set width
        ModifyReg(AW8898_REG_I2SCTRL, AW8898_BIT_I2SCTRL_FMS_MASK, reg_value);
    }

    void setFs(aw_i2s_fs fs)
    {
        std::uint16_t reg_value;

        switch (fs) {
        case aw_i2s_fs::I2SBCK_32FS:
            reg_value = AW8898_BIT_I2SCTRL_BCK_32FS;
            break;
        case aw_i2s_fs::I2SBCK_48FS:
            reg_value = AW8898_BIT_I2SCTRL_BCK_48FS;
            break;
        case aw_i2s_fs::I2SBCK_64FS:
            reg_value = AW8898_BIT_I2SCTRL_BCK_64FS;
            break;
        default:
            reg_value = AW8898_BIT_I2SCTRL_BCK_64FS;
            LOG_DEBUG("%s: fs can not support ", __func__);
            break;
        }

        // set fs
        ModifyReg(AW8898_REG_I2SCTRL, AW8898_BIT_I2SCTRL_BCK_MASK, reg_value);
    }

    status_t HwParams(aw_i2s_channel chsel, aw_i2s_frequency rate, aw_i2s_width width, aw_i2s_fs fs)
    {
        LOG_DEBUG("[chsel]=%i, [rate]=%i", static_cast<int>(chsel), static_cast<int>(rate));

        setChannelSelection(chsel);
        setSampleRate(rate);
        setBitWidth(width);
        setFs(fs);

        return kStatus_Success;
    }

    void LoadRegCfg(void)
    {
        for (unsigned int i = 0; i < configurationSize; i++) {
            WriteReg(initialConfiguration[i].addr, initialConfiguration[i].data);
        }
    }

    status_t RunPwd(bool pwd)
    {
        if (pwd) {
            ModifyReg(AW8898_REG_SYSCTRL, AW8898_BIT_SYSCTRL_PW_MASK, AW8898_BIT_SYSCTRL_PW_PDN);
        }
        else {
            ModifyReg(AW8898_REG_SYSCTRL, AW8898_BIT_SYSCTRL_PW_MASK, AW8898_BIT_SYSCTRL_PW_ACTIVE);
            ModifyReg(AW8898_REG_SYSCTRL, AW8898_BIT_SYSCTRL_I2SEN_MASK, AW8898_BIT_SYSCTRL_I2S_ENABLE);
        }

        return kStatus_Success;
    }

    status_t RunMute(bool mute)
    {
        if (mute) {
            ModifyReg(AW8898_REG_PWMCTRL, AW8898_BIT_PWMCTRL_HMUTE_MASK, AW8898_BIT_PWMCTRL_HMUTE_ENABLE);
        }
        else {
            ModifyReg(AW8898_REG_PWMCTRL, AW8898_BIT_PWMCTRL_HMUTE_MASK, AW8898_BIT_PWMCTRL_HMUTE_DISABLE);
        }

        return kStatus_Success;
    }

    void ColdStart(void)
    {
        LoadRegCfg();
        g_aw8898.init = aw8898_init::AW8898_INIT_OK;

        if ((g_aw8898.mode == aw_sel_mode::SPK_MODE) || (g_aw8898.mode == aw_sel_mode::MUSIC_MODE)) {
            LOG_DEBUG("SPK MODE");
            ModifyReg(AW8898_REG_SYSCTRL, AW8898_BIT_SYSCTRL_MODE_MASK, AW8898_BIT_SYSCTRL_SPK_MODE);
        }
        else {
            LOG_DEBUG("RCV MODE");
            ModifyReg(AW8898_REG_SYSCTRL, AW8898_BIT_SYSCTRL_MODE_MASK, AW8898_BIT_SYSCTRL_RCV_MODE);
        }
    }

    status_t Start(void)
    {
        std::uint16_t reg_val = 0;

        RunPwd(false);
        HAL_Delay(OperationWaitTimeMS);

        for (unsigned int i = 0; i < readRetries; i++) {
            ReadReg(AW8898_REG_SYSST, &reg_val);
            if ((reg_val & AW8898_BIT_SYSST_PLLS)) {
                RunMute(false);
                LOG_DEBUG("%s iis signal check pass!", __func__);
                return kStatus_Success;
            }

            HAL_Delay(OperationWaitTimeMS);
        }
        RunPwd(true);

        LOG_DEBUG("%s: iis signal check error[0x%04X]", __func__, reg_val);

        return kStatus_Fail;
    }

    status_t Stop(void)
    {
        RunMute(true);
        RunPwd(true);

        return kStatus_Success;
    }

    status_t SmartpaCfg(bool play_flag)
    {
        int ret = kStatus_Fail;

        LOG_DEBUG("%s: flag = %d", __func__, play_flag);

        if (play_flag == true && g_aw8898.mode != aw_sel_mode::OFF_MODE) {
            if ((g_aw8898.init == aw8898_init::AW8898_INIT_ST) || (g_aw8898.init == aw8898_init::AW8898_INIT_NG)) {
                LOG_DEBUG("%s: init = %d", __func__, static_cast<int>(g_aw8898.init));
                ColdStart();
            }
            else {
                ret = Start();
                LOG_DEBUG("%s: init = %d", __func__, static_cast<int>(g_aw8898.init));
                if (ret < 0) {
                    LOG_DEBUG("%s: start fail, ret=%d\n", __func__, ret);
                }
                else {
                    LOG_DEBUG("%s: start success", __func__);
                }
            }
        }
        else {
            Stop();
        }

        return kStatus_Success;
    }

    status_t CtrlState(aw_codec_mode mode, aw_ctrl aw_ctrl)
    {
        switch (mode) {
        case aw_codec_mode::AW_ENCODE:
            break;
        case aw_codec_mode::AW_DECODE:
            SmartpaCfg(aw_ctrl != aw_ctrl::STOP);
            break;
        case aw_codec_mode::AW_MODE_BOTH:
            break;
        case aw_codec_mode::AW_MODE_LINE_IN:
            break;
        default:
            break;
        }

        return kStatus_Success;
    }

    status_t SetVolume(std::uint8_t gain)
    {
        status_t res      = kStatus_Success;
        std::uint16_t reg = 0, reg_val = 0;

        res = ReadReg(AW8898_REG_HAGCCFG7, &reg);
        if (res != 0) {
            LOG_DEBUG("reg read err(%ld)", res);
            return res;
        }

        reg_val = (gain << 8) | (reg & 0x00ff);
        res     = WriteReg(AW8898_REG_HAGCCFG7, reg_val);
        LOG_DEBUG("Vol 0x%04X", reg_val);

        return res;
    }

    status_t GetVolume(std::uint8_t *gain)
    {
        status_t res      = kStatus_Success;
        std::uint16_t reg = 0;

        res = ReadReg(AW8898_REG_HAGCCFG7, &reg);
        if (res != kStatus_Success) {
            LOG_DEBUG("reg read err(%ld)", res);
            return res;
        }

        *gain = reg >> 8;
        return res;
    }

    int SetMode(aw_sel_mode mode)
    {
        if ((mode == aw_sel_mode::SPK_MODE) && (g_aw8898.mode != mode)) {
            g_aw8898.mode = aw_sel_mode::SPK_MODE;
            g_aw8898.init = aw8898_init::AW8898_INIT_ST;
        }
        else if ((mode == aw_sel_mode::VOICE_MODE) && (g_aw8898.mode != mode)) {
            g_aw8898.mode = aw_sel_mode::VOICE_MODE;
            g_aw8898.init = aw8898_init::AW8898_INIT_ST;
        }
        else if ((mode == aw_sel_mode::MUSIC_MODE) && (g_aw8898.mode != mode)) {
            g_aw8898.mode = aw_sel_mode::MUSIC_MODE;
            g_aw8898.init = aw8898_init::AW8898_INIT_ST;
        }
        else if (mode == aw_sel_mode::OFF_MODE) {
            g_aw8898.mode = aw_sel_mode::OFF_MODE;
        }
        else {
            LOG_DEBUG("Mode not changed!");
        }
        return 0;
    }

    bool CheckPllStatus(void)
    {
        std::uint16_t reg_val = 0;

        ReadReg(AW8898_REG_SYSST, &reg_val);

        return (reg_val & AW8898_BIT_SYSST_PLLS) != 0;
    }

    void HAL_Delay(std::uint32_t count)
    {
        extern std::uint32_t SystemCoreClock;
        vTaskDelay(pdMS_TO_TICKS(count));
    }

    void ReadAllReg(void)
    {
        std::uint16_t val = 0;
        for (std::uint8_t i = 0; i < sizeof(availableRegisters); i++) {
            ReadReg(availableRegisters[i], &val);
            LOG_DEBUG("reg[%x] = 0x%x", availableRegisters[i], val);
        }
    }

}; // namespace bsp::audio::AW8898
