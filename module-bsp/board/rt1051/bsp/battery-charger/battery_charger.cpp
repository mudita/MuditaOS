// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

/*
 * battery_charger.cpp
 *
 *  Created on: Jun 28, 2019
 *      Author: kuba
 */

extern "C"
{
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
}

#include "bsp/battery-charger/battery_charger.hpp"
#include "vfs.hpp"

#include "bsp/BoardDefinitions.hpp"
#include "common_data/EventStore.hpp"
#include "drivers/gpio/DriverGPIO.hpp"
#include "drivers/i2c/DriverI2C.hpp"

#define BSP_BATTERY_CHARGER_I2C_ADDR (0xD2 >> 1)
#define BSP_FUEL_GAUGE_I2C_ADDR      (0x6C >> 1)
#define BSP_TOP_CONTROLLER_I2C_ADDR  (0xCC >> 1)

static const uint32_t i2cSubaddresSize = 1;

const char *battery_cfgFile     = "sys/batteryAdjustementConfig.cfg";
const char *battery_cfgFilePrev = "sys/batteryAdjustementConfig_old.cfg";

static const uint16_t BATT_SERVICE_AVG_CURRENT_PERIOD =
    0x00; //< 0.1758 ms * 2^(2 + BATT_SERVICE_AVG_CURRENT_PERIOD)         == 700ms
static const uint16_t BATT_SERVICE_AVG_CELL_VOLTAGE_PERIOD =
    0x00; //< 0.1758 ms * 2^(6 + BATT_SERVICE_AVG_CELL_VOLTAGE_PERIOD)    == 11.25 s
static const uint16_t BATT_SERVICE_AVG_MIXING_PERIOD =
    0x0D; //< 0.1758 ms * 2^(5 + BATT_SERVICE_AVG_MIXING_PERIOD)          == 12.8 h
static const uint16_t BATT_SERVICE_AVG_TEMP_PERIOD =
    0x01; //< 0.1758 ms * 2^(11 + BATT_SERVICE_AVG_TEMP_PERIOD)           == 12 min
static const uint16_t BATT_SERVICE_AVG_NEMPTY_PERIOD = 0x00;

static const uint16_t battery_nominalCapacitymAh = 3000;

static const uint8_t battery_fullyChargedPercent = 100;
static const uint8_t battery_DischargedPercent   = 15;

static const uint8_t battery_maxTemperatureDegrees = 50;
static const uint8_t battery_minTemperatureDegrees = 5;

static const uint16_t battery_maxVoltagemV = 4200;
static const uint16_t battery_minVoltagemV = 3700;

using namespace drivers;

static std::shared_ptr<drivers::DriverI2C> i2c;
static std::shared_ptr<drivers::DriverGPIO> gpio;

static bsp::batteryRetval battery_loadConfiguration(void);

static bsp::batteryRetval battery_storeConfiguration(void);

static int battery_fuelGaugeWrite(bsp::batteryChargerRegisters registerAddress, uint16_t value);

static int battery_fuelGaugeRead(bsp::batteryChargerRegisters registerAddress, uint16_t *value);

static int battery_chargerWrite(bsp::batteryChargerRegisters registerAddress, uint8_t value);

static int battery_chargerRead(bsp::batteryChargerRegisters registerAddress, uint8_t *value);

static int battery_chargerTopControllerWrite(bsp::batteryChargerRegisters registerAddress, uint8_t value);

static int battery_chargerTopControllerRead(bsp::batteryChargerRegisters registerAddress, uint8_t *value);

static bsp::batteryRetval battery_setAvgCalcPeriods(void);

static bsp::batteryRetval battery_setAvgCalcPeriods(void);

static bsp::batteryRetval battery_setNominalBatteryCapacity(uint16_t capacity);

static bsp::batteryRetval battery_setChargingDischargingThresholds(uint8_t chargedThresholdPercent,
                                                                   uint8_t dischargedThresholdPercent);

static bsp::batteryRetval battery_setTemperatureThresholds(uint8_t maxTemperatureDegrees,
                                                           uint8_t minTemperatureDegrees);

static bsp::batteryRetval battery_setServiceVoltageThresholds(uint16_t maxVoltage_mV, uint16_t minVoltage_mV);

static bsp::batteryRetval battery_enableFuelGuageIRQs(void);

static bsp::batteryRetval battery_enableTopIRQs(void);

static bsp::batteryRetval battery_disableAlerts(void);

static void s_BSP_BatteryChargerIrqPinsInit();

static xQueueHandle qHandleIrq = NULL;

namespace bsp
{
    namespace
    {
        std::uint8_t batteryLevelCritical = 5;

        void triggerCriticalLevelCheck()
        {
            if (qHandleIrq != NULL) {
                BaseType_t xHigherPriorityTaskWoken = pdFALSE;
                uint8_t val                         = static_cast<uint8_t>(bsp::batteryIRQSource::checkCriticalLevel);
                xQueueSendFromISR(qHandleIrq, &val, &xHigherPriorityTaskWoken);
            }
        }
    } // namespace

    // STATUS register bits
    enum B_STATUS
    {
        Inm    = (1 << 0),
        POR    = (1 << 1),
        SPR_2  = (1 << 2),
        BST    = (1 << 3),
        Isysmx = (1 << 4),
        SPR_5  = (1 << 5),
        ThmHot = (1 << 6),
        dSOCi  = (1 << 7),
        Vmn    = (1 << 8),
        Tmn    = (1 << 9),
        Smn    = (1 << 10),
        Bi     = (1 << 11),
        Vmx    = (1 << 12),
        Tmx    = (1 << 13),
        Smx    = (1 << 14),
        Br     = (1 << 15),
    };

    /// CHG_INT registers from documentation
    enum B_CHG_INT
    {
        BYP_I   = (1 << 0),
        RSVD    = (1 << 1),
        BATP_I  = (1 << 2),
        BAT_I   = (1 << 3),
        CHG_I   = (1 << 4),
        WCIN_I  = (1 << 5),
        CHGIN_I = (1 << 6),
        AICL_I  = (1 << 7),
    };

    uint16_t battery_get_STATUS();
    uint16_t battery_get_CHG_INT_OK();

    int battery_Init(xQueueHandle qHandle)
    {
        i2c = DriverI2C::Create(
            static_cast<I2CInstances>(BoardDefinitions::BATTERY_CHARGER_I2C),
            DriverI2CParams{.baudrate = static_cast<uint32_t>(BoardDefinitions::BATTERY_CHARGER_I2C_BAUDRATE)});

        qHandleIrq = qHandle;

        // check Power-On reset bit
        uint16_t status        = 0;
        const uint16_t porMask = 0x0002;
        battery_fuelGaugeRead(bsp::batteryChargerRegisters::STATUS_REG, &status);

        if (status & porMask) {
            LOG_INFO("Initializing battery charger");
            battery_loadConfiguration();
            battery_setAvgCalcPeriods();
            battery_setNominalBatteryCapacity(battery_nominalCapacitymAh);
            battery_setChargingDischargingThresholds(battery_fullyChargedPercent, battery_DischargedPercent);
            battery_setTemperatureThresholds(battery_maxTemperatureDegrees, battery_minTemperatureDegrees);
            battery_setServiceVoltageThresholds(battery_maxVoltagemV, battery_minVoltagemV);
        }

        battery_disableAlerts();
        battery_enableFuelGuageIRQs();

        uint8_t level = 0;
        bool charging = false;
        battery_getBatteryLevel(level);
        battery_getChargeStatus(charging);
        LOG_INFO("Phone battery start state: %d %d", level, charging);

        battery_ClearAllIRQs();
        battery_enableTopIRQs();

        s_BSP_BatteryChargerIrqPinsInit();

        triggerCriticalLevelCheck();

        return 0;
    }

    void battery_Deinit(void)
    {
        battery_storeConfiguration();

        gpio->DisableInterrupt(1 << static_cast<uint32_t>(BoardDefinitions::BATTERY_CHARGER_INTB_PIN));
        gpio->DisableInterrupt(1 << static_cast<uint32_t>(BoardDefinitions::BATTERY_CHARGER_INOKB_PIN));
        gpio->DisableInterrupt(1 << static_cast<uint32_t>(BoardDefinitions::BATTERY_CHARGER_WCINOKB));

        qHandleIrq = NULL;

        i2c.reset();
        gpio.reset();
    }

    void battery_setCriticalLevel(std::uint8_t level)
    {
        batteryLevelCritical = level;
        triggerCriticalLevelCheck();
    }

    bool battery_isLevelCritical(std::uint8_t level)
    {
        return level <= batteryLevelCritical;
    }

    void battery_getBatteryLevel(uint8_t &levelPercent)
    {
        uint16_t val = 0;
        if (battery_fuelGaugeRead(bsp::batteryChargerRegisters::RepSOC_REG, &val) != kStatus_Success) {
            LOG_ERROR("failed to get battery percent");
        }
        levelPercent                   = (val & 0xff00) >> 8;
        Store::Battery::modify().level = levelPercent;
    }

    void battery_getChargeStatus(bool &status)
    {
        uint8_t val = 0;
        // read clears state
        if (battery_chargerRead(bsp::batteryChargerRegisters::CHG_INT_OK, &val) != kStatus_Success) {
            LOG_ERROR("failed to read charge status");
        }
        status = val & B_CHG_INT::CHGIN_I;
        if (status) {
            Store::Battery::modify().state = Store::Battery::State::Charging;
        }
        else {
            Store::Battery::modify().state = Store::Battery::State::Discharging;
        }
    }

    uint16_t battery_get_STATUS()
    {
        uint16_t status = 0;
        battery_fuelGaugeRead(bsp::batteryChargerRegisters::STATUS_REG, &status);
        return status;
    }

    void battery_ClearAllIRQs(void)
    {
        uint8_t val = 0;
        battery_chargerRead(bsp::batteryChargerRegisters::CHG_INT_REG, &val);
        if (val != 0) {
            // write zero to clear irq source
            battery_chargerWrite(bsp::batteryChargerRegisters::CHG_INT_REG, 0);
        }

        uint16_t status = battery_get_STATUS();
        if (status != 0) {
            // write zero to clear irq source
            battery_fuelGaugeWrite(bsp::batteryChargerRegisters::STATUS_REG, 0);
        }
    }

    void battery_clearFuelGuageIRQ(void)
    {
        // write zero to clear interrupt source
        battery_fuelGaugeWrite(bsp::batteryChargerRegisters::STATUS_REG, 0x0000);
    }
} // namespace bsp

static int battery_fuelGaugeWrite(bsp::batteryChargerRegisters registerAddress, uint16_t value)
{
    I2CAddress addr{.deviceAddress  = BSP_FUEL_GAUGE_I2C_ADDR,
                    .subAddress     = static_cast<uint32_t>(registerAddress),
                    .subAddressSize = i2cSubaddresSize};
    auto ret = i2c->Write(addr, reinterpret_cast<uint8_t *>(&value), sizeof(uint16_t));

    if (ret != sizeof(uint16_t)) {
        return kStatus_Fail;
    }
    else {
        return kStatus_Success;
    }
}

static int battery_fuelGaugeRead(bsp::batteryChargerRegisters registerAddress, uint16_t *value)
{
    if (value == NULL) {
        return -1;
    }

    I2CAddress addr{.deviceAddress  = BSP_FUEL_GAUGE_I2C_ADDR,
                    .subAddress     = static_cast<uint32_t>(registerAddress),
                    .subAddressSize = i2cSubaddresSize};
    auto ret = i2c->Read(addr, reinterpret_cast<uint8_t *>(value), sizeof(uint16_t));

    if (ret != sizeof(uint16_t)) {
        return kStatus_Fail;
    }
    else {
        return kStatus_Success;
    }
}

static int battery_chargerWrite(bsp::batteryChargerRegisters registerAddress, uint8_t value)
{

    I2CAddress addr{.deviceAddress  = BSP_BATTERY_CHARGER_I2C_ADDR,
                    .subAddress     = static_cast<uint32_t>(registerAddress),
                    .subAddressSize = i2cSubaddresSize};
    auto ret = i2c->Write(addr, reinterpret_cast<uint8_t *>(&value), sizeof(uint8_t));

    if (ret != sizeof(uint8_t)) {
        return kStatus_Fail;
    }
    else {
        return kStatus_Success;
    }
}

static int battery_chargerRead(bsp::batteryChargerRegisters registerAddress, uint8_t *value)
{
    if (value == NULL) {
        return -1;
    }

    I2CAddress addr{.deviceAddress  = BSP_BATTERY_CHARGER_I2C_ADDR,
                    .subAddress     = static_cast<uint32_t>(registerAddress),
                    .subAddressSize = i2cSubaddresSize};
    auto ret = i2c->Read(addr, value, sizeof(uint8_t));
    if (ret != sizeof(uint8_t)) {
        return kStatus_Fail;
    }
    else {
        return kStatus_Success;
    }
}

static int battery_chargerTopControllerWrite(bsp::batteryChargerRegisters registerAddress, uint8_t value)
{

    I2CAddress addr{.deviceAddress  = BSP_TOP_CONTROLLER_I2C_ADDR,
                    .subAddress     = static_cast<uint32_t>(registerAddress),
                    .subAddressSize = i2cSubaddresSize};
    auto ret = i2c->Write(addr, reinterpret_cast<uint8_t *>(&value), sizeof(uint8_t));
    if (ret != sizeof(uint8_t)) {
        return kStatus_Fail;
    }
    else {
        return kStatus_Success;
    }
}

static int battery_chargerTopControllerRead(bsp::batteryChargerRegisters registerAddress, uint8_t *value)
{
    if (value == NULL) {
        return -1;
    }

    I2CAddress addr{.deviceAddress  = BSP_TOP_CONTROLLER_I2C_ADDR,
                    .subAddress     = static_cast<uint32_t>(registerAddress),
                    .subAddressSize = i2cSubaddresSize};
    auto ret = i2c->Read(addr, reinterpret_cast<uint8_t *>(value), sizeof(uint8_t));
    if (ret != sizeof(uint8_t)) {
        return kStatus_Fail;
    }
    else {
        return kStatus_Success;
    }
}

static bsp::batteryRetval battery_loadConfiguration(void)
{
    vfs::FILE *fd = NULL;

    fd = vfs.fopen(battery_cfgFile, "r");
    if (fd == NULL) {
        LOG_WARN("Configuration file [%s] not found. Searching for file [%s]", battery_cfgFile, battery_cfgFilePrev);
        fd = vfs.fopen(battery_cfgFilePrev, "r");
        if (fd == NULL) {
            LOG_WARN("Configuration file [%s] not found.", battery_cfgFilePrev);
            return bsp::batteryRetval::battery_ChargerError;
        }
    }

    uint16_t regValue = 0;
    ;
    for (uint8_t i = 0; i < 0xff; ++i) {
        if (vfs.fread(&regValue, sizeof(regValue), 1, fd) != sizeof(regValue)) {
            LOG_ERROR("Reading register 0x%x failed.", i);
            vfs.fclose(fd);
            return bsp::batteryRetval::battery_ChargerError;
        }

        if (battery_fuelGaugeWrite(static_cast<bsp::batteryChargerRegisters>(i), regValue) != kStatus_Success) {
            LOG_ERROR("Writing register 0x%x failed.", i);
            vfs.fclose(fd);
            return bsp::batteryRetval::battery_ChargerError;
        }
    }

    return bsp::batteryRetval::battery_OK;
}

static bsp::batteryRetval battery_storeConfiguration(void)
{
    // TODO:M.P procedure below seems to crash system, it should be fixed.
    if (ff_rename(battery_cfgFile, battery_cfgFilePrev, false) != 0) {
        LOG_ERROR("Could not move configuration file");
        return bsp::batteryRetval::battery_ChargerError;
    }

    vfs::FILE *fd = NULL;

    fd = vfs.fopen(battery_cfgFile, "w");
    if (fd == NULL) {
        LOG_ERROR("Could not open configuration file");
        return bsp::batteryRetval::battery_ChargerError;
    }

    uint16_t regVal = 0;
    for (unsigned int i = 0; i < 0xff; ++i) {
        if (battery_fuelGaugeRead(static_cast<bsp::batteryChargerRegisters>(i), &regVal) != kStatus_Success) {
            LOG_ERROR("Reading register 0x%x failed.", i);
            vfs.fclose(fd);
            return bsp::batteryRetval::battery_ChargerError;
        }

        if (vfs.fwrite(&regVal, sizeof(regVal), 1, fd) != sizeof(regVal)) {
            LOG_ERROR("Storing register 0x%x failed.", i);
            vfs.fclose(fd);
            vfs.remove(battery_cfgFile);
            return bsp::batteryRetval::battery_ChargerError;
        }
    }
    return bsp::batteryRetval::battery_OK;
}

static bsp::batteryRetval battery_setAvgCalcPeriods(void)
{
    uint16_t regVal = 0;
    regVal |= (BATT_SERVICE_AVG_CURRENT_PERIOD << 0);
    regVal |= (BATT_SERVICE_AVG_CELL_VOLTAGE_PERIOD << 4);
    regVal |= (BATT_SERVICE_AVG_MIXING_PERIOD << 7);
    regVal |= (BATT_SERVICE_AVG_TEMP_PERIOD << 11);
    regVal |= (BATT_SERVICE_AVG_NEMPTY_PERIOD << 14);

    if (battery_fuelGaugeWrite(bsp::batteryChargerRegisters::FilterCFG_REG, regVal) != kStatus_Success) {
        LOG_ERROR("battery_setAvgCalcPeriods failed.");
        return bsp::batteryRetval::battery_ChargerError;
    }
    return bsp::batteryRetval::battery_OK;
}

static bsp::batteryRetval battery_setNominalBatteryCapacity(uint16_t capacity)
{
    uint16_t regVal = capacity * 2;

    if (battery_fuelGaugeWrite(bsp::batteryChargerRegisters::DesignCap_REG, regVal) != kStatus_Success) {
        LOG_ERROR("battery_setNominalBatteryCapacity failed.");
        return bsp::batteryRetval::battery_ChargerError;
    }
    return bsp::batteryRetval::battery_OK;
}

static bsp::batteryRetval battery_setChargingDischargingThresholds(uint8_t chargedThresholdPercent,
                                                                   uint8_t dischargedThresholdPercent)
{
    uint16_t regVal = (chargedThresholdPercent << 8) | dischargedThresholdPercent;

    if (battery_fuelGaugeWrite(bsp::batteryChargerRegisters::SALRT_Th_REG, regVal) != kStatus_Success) {
        LOG_ERROR("battery_setChargingDischargingThresholds failed.");
        return bsp::batteryRetval::battery_ChargerError;
    }
    return bsp::batteryRetval::battery_OK;
}

static bsp::batteryRetval battery_setTemperatureThresholds(uint8_t maxTemperatureDegrees, uint8_t minTemperatureDegrees)
{
    uint16_t regVal = (maxTemperatureDegrees << 8) | minTemperatureDegrees;

    if (battery_fuelGaugeWrite(bsp::batteryChargerRegisters::TALRT_Th_REG, regVal) != kStatus_Success) {
        LOG_ERROR("battery_setTemperatureThresholds failed.");
        return bsp::batteryRetval::battery_ChargerError;
    }
    return bsp::batteryRetval::battery_OK;
}

static bsp::batteryRetval battery_setServiceVoltageThresholds(uint16_t maxVoltage_mV, uint16_t minVoltage_mV)
{
    uint16_t regVal = ((maxVoltage_mV / 20) << 8) | (minVoltage_mV / 20);

    if (battery_fuelGaugeWrite(bsp::batteryChargerRegisters::VALRT_Th_REG, regVal) != kStatus_Success) {

        LOG_ERROR("battery_setServiceVoltageThresholds failed.");
        return bsp::batteryRetval::battery_ChargerError;
    }
    return bsp::batteryRetval::battery_OK;
}

static bsp::batteryRetval battery_enableFuelGuageIRQs(void)
{
    uint16_t regVal = 0;
    // set dSOCen bit
    regVal |= (1 << 7);
    if (battery_fuelGaugeWrite(bsp::batteryChargerRegisters::CONFIG2_REG, regVal) != kStatus_Success) {
        LOG_ERROR("battery_enableFuelGuageIRQs failed.");
        return bsp::batteryRetval::battery_ChargerError;
    }

    return bsp::batteryRetval::battery_OK;
}

static bsp::batteryRetval battery_disableAlerts(void)
{
    uint16_t regVal = 0;

    if (battery_fuelGaugeWrite(bsp::batteryChargerRegisters::CONFIG_REG, regVal) != kStatus_Success) {
        LOG_ERROR("battery_disableAlerts failed.");
        return bsp::batteryRetval::battery_ChargerError;
    }

    return bsp::batteryRetval::battery_OK;
}

static bsp::batteryRetval battery_enableTopIRQs(void)
{
    uint8_t val = 0xf8;

    if (battery_chargerTopControllerWrite(bsp::batteryChargerRegisters::TOP_CONTROLL_IRQ_MASK_REG, val) !=
        kStatus_Success) {
        LOG_ERROR("battery_enableIRQs read failed.");
        return bsp::batteryRetval::battery_ChargerError;
    }

    return bsp::batteryRetval::battery_OK;
}

BaseType_t BSP_BatteryChargerINOKB_IRQHandler()
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    if (qHandleIrq != NULL) {
        uint8_t val = static_cast<uint8_t>(bsp::batteryIRQSource::INOKB);
        xQueueSendFromISR(qHandleIrq, &val, &xHigherPriorityTaskWoken);
    }
    return xHigherPriorityTaskWoken;
}

BaseType_t BSP_BatteryChargerINTB_IRQHandler()
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    if (qHandleIrq != NULL) {
        uint8_t val = static_cast<uint8_t>(bsp::batteryIRQSource::INTB);
        xQueueSendFromISR(qHandleIrq, &val, &xHigherPriorityTaskWoken);
    }
    return xHigherPriorityTaskWoken;
}

static void s_BSP_BatteryChargerIrqPinsInit()
{

    gpio = DriverGPIO::Create(static_cast<GPIOInstances>(BoardDefinitions::BATTERY_CHARGER_GPIO), DriverGPIOParams{});

    gpio->ConfPin(DriverGPIOPinParams{.dir      = DriverGPIOPinParams::Direction::Input,
                                      .irqMode  = DriverGPIOPinParams::InterruptMode::IntRisingOrFallingEdge,
                                      .defLogic = 0,
                                      .pin      = static_cast<uint32_t>(BoardDefinitions::BATTERY_CHARGER_INOKB_PIN)});

    gpio->ConfPin(DriverGPIOPinParams{.dir      = DriverGPIOPinParams::Direction::Input,
                                      .irqMode  = DriverGPIOPinParams::InterruptMode::IntRisingOrFallingEdge,
                                      .defLogic = 0,
                                      .pin      = static_cast<uint32_t>(BoardDefinitions::BATTERY_CHARGER_WCINOKB)});

    gpio->ConfPin(DriverGPIOPinParams{.dir      = DriverGPIOPinParams::Direction::Input,
                                      .irqMode  = DriverGPIOPinParams::InterruptMode::IntFallingEdge,
                                      .defLogic = 0,
                                      .pin      = static_cast<uint32_t>(BoardDefinitions::BATTERY_CHARGER_INTB_PIN)});

    gpio->EnableInterrupt(1 << static_cast<uint32_t>(BoardDefinitions::BATTERY_CHARGER_WCINOKB));
    gpio->EnableInterrupt(1 << static_cast<uint32_t>(BoardDefinitions::BATTERY_CHARGER_INOKB_PIN));
    gpio->EnableInterrupt(1 << static_cast<uint32_t>(BoardDefinitions::BATTERY_CHARGER_INTB_PIN));
}
