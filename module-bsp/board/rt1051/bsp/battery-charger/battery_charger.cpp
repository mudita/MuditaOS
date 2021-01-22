// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "bsp/battery-charger/battery_charger.hpp"
#include "MAX77818.hpp"
#include "vfs.hpp"

#include "bsp/BoardDefinitions.hpp"
#include "common_data/EventStore.hpp"
#include "drivers/gpio/DriverGPIO.hpp"
#include "drivers/i2c/DriverI2C.hpp"
#include <purefs/filesystem_paths.hpp>
#include <utility>

namespace bsp::battery_charger
{
    namespace
    {
        constexpr std::uint32_t i2cSubaddresSize = 1;

        const auto cfgFile     = purefs::dir::getCurrentOSPath() / "batteryAdjustementConfig.cfg";
        const auto cfgFilePrev = purefs::dir::getCurrentOSPath() / "batteryAdjustementConfig_old.cfg";

        constexpr std::uint16_t BATT_SERVICE_AVG_CURRENT_PERIOD =
            0x00; //< 0.1758 ms * 2^(2 + BATT_SERVICE_AVG_CURRENT_PERIOD)         == 700ms
        constexpr std::uint16_t BATT_SERVICE_AVG_CELL_VOLTAGE_PERIOD =
            0x00; //< 0.1758 ms * 2^(6 + BATT_SERVICE_AVG_CELL_VOLTAGE_PERIOD)    == 11.25 s
        constexpr std::uint16_t BATT_SERVICE_AVG_MIXING_PERIOD =
            0x0D; //< 0.1758 ms * 2^(5 + BATT_SERVICE_AVG_MIXING_PERIOD)          == 12.8 h
        constexpr std::uint16_t BATT_SERVICE_AVG_TEMP_PERIOD =
            0x01; //< 0.1758 ms * 2^(11 + BATT_SERVICE_AVG_TEMP_PERIOD)           == 12 min
        constexpr std::uint16_t BATT_SERVICE_AVG_NEMPTY_PERIOD = 0x00;

        constexpr auto ENABLE_ALL_IRQ_MASK = 0xf8;

        constexpr std::uint16_t nominalCapacitymAh = 3000;

        constexpr std::uint8_t fullyChargedPercent = 100;
        constexpr std::uint8_t DischargedPercent   = 15;

        constexpr std::uint8_t maxTemperatureDegrees = 50;
        constexpr std::uint8_t minTemperatureDegrees = 5;

        constexpr std::uint16_t maxVoltagemV = 4200;
        constexpr std::uint16_t minVoltagemV = 3600;

        std::shared_ptr<drivers::DriverI2C> i2c;
        std::shared_ptr<drivers::DriverGPIO> gpio;

        drivers::I2CAddress fuelGaugeAddress      = {FUEL_GAUGE_I2C_ADDR, 0, i2cSubaddresSize};
        drivers::I2CAddress batteryChargerAddress = {BATTERY_CHARGER_I2C_ADDR, 0, i2cSubaddresSize};
        drivers::I2CAddress topControllerAddress  = {TOP_CONTROLLER_I2C_ADDR, 0, i2cSubaddresSize};

        xQueueHandle IRQQueueHandle = nullptr;

        int fuelGaugeWrite(Registers registerAddress, std::uint16_t value)
        {
            fuelGaugeAddress.subAddress = static_cast<std::uint32_t>(registerAddress);
            auto ret = i2c->Write(fuelGaugeAddress, reinterpret_cast<std::uint8_t *>(&value), sizeof(std::uint16_t));

            if (ret != sizeof(std::uint16_t)) {
                return kStatus_Fail;
            }
            return kStatus_Success;
        }

        std::pair<int, std::uint16_t> fuelGaugeRead(Registers registerAddress)
        {
            std::uint16_t value;
            int status                  = kStatus_Success;
            fuelGaugeAddress.subAddress = static_cast<std::uint32_t>(registerAddress);
            auto ret = i2c->Read(fuelGaugeAddress, reinterpret_cast<std::uint8_t *>(&value), sizeof(std::uint16_t));

            if (ret != sizeof(std::uint16_t)) {
                status = kStatus_Fail;
            }
            return std::make_pair(status, value);
        }

        int chargerWrite(Registers registerAddress, std::uint8_t value)
        {
            batteryChargerAddress.subAddress = static_cast<std::uint32_t>(registerAddress);
            auto ret =
                i2c->Write(batteryChargerAddress, reinterpret_cast<std::uint8_t *>(&value), sizeof(std::uint8_t));

            if (ret != sizeof(std::uint8_t)) {
                return kStatus_Fail;
            }
            return kStatus_Success;
        }

        std::pair<int, std::uint8_t> chargerRead(Registers registerAddress)
        {
            std::uint8_t value;
            int status                       = kStatus_Success;
            batteryChargerAddress.subAddress = static_cast<std::uint32_t>(registerAddress);
            auto ret                         = i2c->Read(batteryChargerAddress, &value, sizeof(std::uint8_t));

            if (ret != sizeof(std::uint8_t)) {
                status = kStatus_Fail;
            }
            return std::make_pair(status, value);
        }

        int chargerTopControllerWrite(Registers registerAddress, std::uint8_t value)
        {
            topControllerAddress.subAddress = static_cast<std::uint32_t>(registerAddress);
            auto ret = i2c->Write(topControllerAddress, reinterpret_cast<std::uint8_t *>(&value), sizeof(std::uint8_t));

            if (ret != sizeof(std::uint8_t)) {
                return kStatus_Fail;
            }
            return kStatus_Success;
        }

        std::pair<int, std::uint8_t> chargerTopControllerRead(Registers registerAddress)
        {
            std::uint8_t value;
            int status                      = kStatus_Success;
            topControllerAddress.subAddress = static_cast<std::uint32_t>(registerAddress);
            auto ret                        = i2c->Read(topControllerAddress, &value, sizeof(std::uint8_t));
            if (ret != sizeof(std::uint8_t)) {
                status = kStatus_Fail;
            }
            return std::make_pair(status, value);
        }

        batteryRetval loadConfiguration()
        {
            auto fd = std::fopen(cfgFile.c_str(), "r");
            if (fd == nullptr) {
                LOG_WARN("Configuration file [%s] could not be opened. Trying to open file [%s]",
                         cfgFile.c_str(),
                         cfgFilePrev.c_str());
                fd = std::fopen(cfgFilePrev.c_str(), "r");
                if (fd == nullptr) {
                    LOG_WARN("Configuration file [%s] could not be opened.", cfgFilePrev.c_str());
                    return batteryRetval::ChargerError;
                }
            }

            std::uint16_t regValue = 0;
            for (auto i = 0; i < 0xff; ++i) {
                if (std::fread(&regValue, sizeof(regValue), 1, fd) != sizeof(regValue)) {
                    LOG_ERROR("Reading register 0x%x failed.", i);
                    std::fclose(fd);
                    return batteryRetval::ChargerError;
                }

                if (fuelGaugeWrite(static_cast<Registers>(i), regValue) != kStatus_Success) {
                    LOG_ERROR("Writing register 0x%x failed.", i);
                    std::fclose(fd);
                    return batteryRetval::ChargerError;
                }
            }

            std::fclose(fd);
            return batteryRetval::OK;
        }

        batteryRetval storeConfiguration()
        {
            // TODO:M.P procedure below seems to crash system, it should be fixed.
            if (ff_rename(cfgFile.c_str(), cfgFilePrev.c_str(), false) != 0) {
                LOG_ERROR("Could not move configuration file");
                return batteryRetval::ChargerError;
            }

            auto fd = std::fopen(cfgFile.c_str(), "w");
            if (fd == nullptr) {
                LOG_ERROR("Could not open configuration file");
                return batteryRetval::ChargerError;
            }

            for (unsigned int i = 0; i < 0xff; ++i) {
                auto regVal = fuelGaugeRead(static_cast<Registers>(i));
                if (regVal.first != kStatus_Success) {
                    LOG_ERROR("Reading register 0x%x failed.", i);
                    std::fclose(fd);
                    return batteryRetval::ChargerError;
                }

                if (std::fwrite(&regVal.second, sizeof(regVal.second), 1, fd) != sizeof(regVal.second)) {
                    LOG_ERROR("Storing register 0x%x failed.", i);
                    std::fclose(fd);
                    std::remove(cfgFile.c_str());
                    return batteryRetval::ChargerError;
                }
            }

            std::fclose(fd);
            return batteryRetval::OK;
        }

        batteryRetval setAvgCalcPeriods()
        {
            std::uint16_t regVal = 0;
            regVal |= (BATT_SERVICE_AVG_CURRENT_PERIOD << 0);
            regVal |= (BATT_SERVICE_AVG_CELL_VOLTAGE_PERIOD << 4);
            regVal |= (BATT_SERVICE_AVG_MIXING_PERIOD << 7);
            regVal |= (BATT_SERVICE_AVG_TEMP_PERIOD << 11);
            regVal |= (BATT_SERVICE_AVG_NEMPTY_PERIOD << 14);

            if (fuelGaugeWrite(Registers::FilterCFG_REG, regVal) != kStatus_Success) {
                LOG_ERROR("setAvgCalcPeriods failed.");
                return batteryRetval::ChargerError;
            }
            return batteryRetval::OK;
        }

        batteryRetval setNominalBatteryCapacity(std::uint16_t capacity)
        {
            std::uint16_t regVal = capacity * 2;

            if (fuelGaugeWrite(Registers::DesignCap_REG, regVal) != kStatus_Success) {
                LOG_ERROR("setNominalBatteryCapacity failed.");
                return batteryRetval::ChargerError;
            }
            return batteryRetval::OK;
        }

        batteryRetval setChargingDischargingThresholds(std::uint8_t chargedThresholdPercent,
                                                       std::uint8_t dischargedThresholdPercent)
        {
            uint16_t regVal = (chargedThresholdPercent << 8) | dischargedThresholdPercent;

            if (fuelGaugeWrite(Registers::SALRT_Th_REG, regVal) != kStatus_Success) {
                LOG_ERROR("setChargingDischargingThresholds failed.");
                return batteryRetval::ChargerError;
            }
            return batteryRetval::OK;
        }

        batteryRetval setTemperatureThresholds(std::uint8_t maxTemperatureDegrees, std::uint8_t minTemperatureDegrees)
        {
            std::uint16_t regVal = (maxTemperatureDegrees << 8) | minTemperatureDegrees;

            if (fuelGaugeWrite(Registers::TALRT_Th_REG, regVal) != kStatus_Success) {
                LOG_ERROR("setTemperatureThresholds failed.");
                return batteryRetval::ChargerError;
            }
            return batteryRetval::OK;
        }

        batteryRetval setServiceVoltageThresholds(std::uint16_t maxVoltage_mV, std::uint16_t minVoltage_mV)
        {
            std::uint16_t regVal = ((maxVoltage_mV / 20) << 8) | (minVoltage_mV / 20);

            if (fuelGaugeWrite(Registers::VALRT_Th_REG, regVal) != kStatus_Success) {

                LOG_ERROR("setServiceVoltageThresholds failed.");
                return batteryRetval::ChargerError;
            }
            return batteryRetval::OK;
        }

        batteryRetval enableFuelGuageIRQs()
        {
            std::uint16_t regVal = static_cast<std::uint16_t>(CONFIG2::dSOCen);

            if (fuelGaugeWrite(Registers::CONFIG2_REG, regVal) != kStatus_Success) {
                LOG_ERROR("enableFuelGuageIRQs failed.");
                return batteryRetval::ChargerError;
            }

            return batteryRetval::OK;
        }

        batteryRetval configureAlerts()
        {
            std::uint16_t regVal = static_cast<std::uint16_t>(CONFIG::Aen);

            if (fuelGaugeWrite(Registers::CONFIG_REG, regVal) != kStatus_Success) {
                LOG_ERROR("configureAlerts failed.");
                return batteryRetval::ChargerError;
            }

            return batteryRetval::OK;
        }

        batteryRetval enableTopIRQs()
        {
            std::uint8_t val = ENABLE_ALL_IRQ_MASK;

            if (chargerTopControllerWrite(Registers::TOP_CONTROLL_IRQ_MASK_REG, val) != kStatus_Success) {
                LOG_ERROR("enableIRQs read failed.");
                return batteryRetval::ChargerError;
            }

            return batteryRetval::OK;
        }

        void IRQPinsInit()
        {
            gpio =
                drivers::DriverGPIO::Create(static_cast<drivers::GPIOInstances>(BoardDefinitions::BATTERY_CHARGER_GPIO),
                                            drivers::DriverGPIOParams{});

            drivers::DriverGPIOPinParams INOKBPinConfig;
            INOKBPinConfig.dir      = drivers::DriverGPIOPinParams::Direction::Input;
            INOKBPinConfig.irqMode  = drivers::DriverGPIOPinParams::InterruptMode::IntRisingOrFallingEdge;
            INOKBPinConfig.defLogic = 0;
            INOKBPinConfig.pin      = static_cast<std::uint32_t>(BoardDefinitions::BATTERY_CHARGER_INOKB_PIN);
            gpio->ConfPin(INOKBPinConfig);

            drivers::DriverGPIOPinParams INTBPinConfig;
            INTBPinConfig.dir      = drivers::DriverGPIOPinParams::Direction::Input;
            INTBPinConfig.irqMode  = drivers::DriverGPIOPinParams::InterruptMode::IntFallingEdge;
            INTBPinConfig.defLogic = 0;
            INTBPinConfig.pin      = static_cast<std::uint32_t>(BoardDefinitions::BATTERY_CHARGER_INTB_PIN);
            gpio->ConfPin(INTBPinConfig);

            gpio->EnableInterrupt(1 << static_cast<std::uint32_t>(BoardDefinitions::BATTERY_CHARGER_INOKB_PIN));
            gpio->EnableInterrupt(1 << static_cast<std::uint32_t>(BoardDefinitions::BATTERY_CHARGER_INTB_PIN));
        }
    } // namespace

    int init(xQueueHandle queueHandle)
    {
        drivers::DriverI2CParams i2cParams;
        i2cParams.baudrate = static_cast<std::uint32_t>(BoardDefinitions::BATTERY_CHARGER_I2C_BAUDRATE);
        i2c = drivers::DriverI2C::Create(static_cast<drivers::I2CInstances>(BoardDefinitions::BATTERY_CHARGER_I2C),
                                         i2cParams);

        IRQQueueHandle = queueHandle;

        // check Power-On reset bit
        std::uint16_t status = fuelGaugeRead(Registers::STATUS_REG).second;

        if (status & static_cast<std::uint16_t>(STATUS::POR)) {
            LOG_INFO("Initializing battery charger");
            loadConfiguration();
            setAvgCalcPeriods();
            setNominalBatteryCapacity(nominalCapacitymAh);
            setChargingDischargingThresholds(fullyChargedPercent, DischargedPercent);
            setTemperatureThresholds(maxTemperatureDegrees, minTemperatureDegrees);
            setServiceVoltageThresholds(maxVoltagemV, minVoltagemV);
        }

        configureAlerts();
        enableFuelGuageIRQs();

        StateOfCharge level = getBatteryLevel();
        bool charging       = getChargeStatus();
        LOG_INFO("Phone battery start state: %d %d", level, charging);

        clearAllIRQs();
        enableTopIRQs();

        IRQPinsInit();

        return 0;
    }

    void deinit()
    {
        storeConfiguration();

        gpio->DisableInterrupt(1 << static_cast<uint32_t>(BoardDefinitions::BATTERY_CHARGER_INTB_PIN));
        gpio->DisableInterrupt(1 << static_cast<uint32_t>(BoardDefinitions::BATTERY_CHARGER_INOKB_PIN));

        IRQQueueHandle = nullptr;

        i2c.reset();
        gpio.reset();
    }

    StateOfCharge getBatteryLevel()
    {
        auto readout = fuelGaugeRead(Registers::RepSOC_REG);
        if (readout.first != kStatus_Success) {
            LOG_ERROR("failed to get battery percent");
        }
        StateOfCharge levelPercent     = (readout.second & 0xff00) >> 8;
        Store::Battery::modify().level = levelPercent;
        return levelPercent;
    }

    bool getChargeStatus()
    {
        std::uint8_t val = 0;
        // read clears state
        auto value = chargerRead(Registers::CHG_INT_OK);
        if (value.first != kStatus_Success) {
            LOG_ERROR("failed to read charge status");
        }
        bool status = value.second & static_cast<std::uint8_t>(CHG_INT::CHGIN_I);
        if (status) {
            Store::Battery::modify().state = Store::Battery::State::Charging;
        }
        else {
            Store::Battery::modify().state = Store::Battery::State::Discharging;
        }
        return status;
    }

    std::uint16_t getStatusRegister()
    {
        auto status = fuelGaugeRead(Registers::STATUS_REG);
        return status.second;
    }

    void clearAllIRQs()
    {
        auto value = chargerRead(Registers::CHG_INT_REG);
        if (value.second != 0) {
            // write zero to clear irq source
            chargerWrite(Registers::CHG_INT_REG, 0);
        }

        std::uint16_t status = getStatusRegister();
        if (status != 0) {
            // write zero to clear irq source
            fuelGaugeWrite(Registers::STATUS_REG, 0);
        }
    }

    void clearFuelGuageIRQ()
    {
        // write zero to clear interrupt source
        fuelGaugeWrite(Registers::STATUS_REG, 0x0000);
    }

    BaseType_t INOKB_IRQHandler()
    {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        if (IRQQueueHandle != nullptr) {
            std::uint8_t val = static_cast<std::uint8_t>(batteryIRQSource::INOKB);
            xQueueSendFromISR(IRQQueueHandle, &val, &xHigherPriorityTaskWoken);
        }
        return xHigherPriorityTaskWoken;
    }

    BaseType_t INTB_IRQHandler()
    {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        if (IRQQueueHandle != nullptr) {
            std::uint8_t val = static_cast<std::uint8_t>(batteryIRQSource::INTB);
            xQueueSendFromISR(IRQQueueHandle, &val, &xHigherPriorityTaskWoken);
        }
        return xHigherPriorityTaskWoken;
    }

} // namespace bsp::battery_charger
