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
#include <bitset>
#include <fstream>

namespace bsp::battery_charger
{
    namespace
    {
        constexpr std::uint32_t i2cSubaddresSize = 1;

        const auto cfgFile              = purefs::dir::getUserDiskPath() / "batteryFuelGaugeConfig.cfg";
        constexpr auto registersToStore = 0xFF + 1;

        constexpr std::uint16_t ENABLE_ALL_IRQ_MASK = 0xF8;
        constexpr std::uint8_t UNLOCK_CHARGER       = 0x3 << 2;

        constexpr std::uint8_t CHG_ON_OTG_OFF_BUCK_ON  = 0b00000101;
        constexpr std::uint8_t CHG_OFF_OTG_OFF_BUCK_ON = 0b00000100;

        constexpr std::uint8_t VSYS_MIN              = 0x80; // 3.6V
        constexpr std::uint8_t CHARGE_TARGET_VOLTAGE = 0x1D; // 4.35V

        constexpr std::uint8_t MAX_CHARGE_CURRENT  = 0x30; // 1600mA -> 1C
        constexpr std::uint8_t FAST_CHARGE_CURRENT = 0x0A; // 500mA

        constexpr std::uint16_t nominalCapacitymAh = 1600;

        constexpr std::uint16_t fullyChargedSOC = 100;

        constexpr std::uint8_t maxTemperatureDegrees = 46;
        constexpr std::uint8_t minTemperatureDegrees = 0;
        constexpr std::uint8_t maxDisabled           = 0x7F;
        constexpr std::uint8_t minDisabled           = 0x80;
        constexpr auto temperatureHysteresis         = 1;

        constexpr std::uint16_t maxVoltagemV = 4400;
        constexpr std::uint16_t minVoltagemV = 3600;

        constexpr auto currentSenseGain = 0.15625;  // mA
        constexpr auto voltageSenseGain = 0.078125; // mV

        // NTC calibration values
        constexpr std::uint16_t temperatureConversionGain   = 0xEE56;
        constexpr std::uint16_t temperatureConversionOffset = 0x1DA4;

        namespace fuel_gauge_params
        {
            /// Parameters calculated in in MAXIM EVKIT software
            /// Initial parameters for fuel gauge battery model

            constexpr std::uint16_t LearnCFG    = 0x2602;
            constexpr std::uint16_t FilterCFG   = 0xCEA4;
            constexpr std::uint16_t MiscCFG     = 0x01D0;
            constexpr std::uint16_t RelaxCFG    = 0x2039;
            constexpr std::uint16_t RCOMP0      = 0x0070;
            constexpr std::uint16_t TempCo      = 0x263D;
            constexpr std::uint16_t QResidual00 = 0x2280;
            constexpr std::uint16_t QResidual10 = 0x1000;
            constexpr std::uint16_t QResidual20 = 0x0681;
            constexpr std::uint16_t QResidual30 = 0x0682;
            /// 1600mAh initial battery capacity
            constexpr std::uint16_t FullCAPNom = 0x0C80;
            constexpr std::uint16_t FullCAPRep = 0x0C80;
            constexpr std::uint16_t DesignCap  = 0x0C80;
            constexpr std::uint16_t dQacc      = 0x00C8;
            constexpr std::uint16_t dPacc      = 0x0C80;
            /// Charge termination current = 150mA
            constexpr std::uint16_t ICHGTerm = 0x03C0;
            /// Empty battery = 3V
            constexpr std::uint16_t V_empty = 0x965A;
            /// Fully charged threshold = 90%
            constexpr std::uint16_t FullSOCthr = 0x5A00;

        } // namespace fuel_gauge_params

        enum class TemperatureRanges
        {
            normal,
            cold,
            hot
        };

        std::shared_ptr<drivers::DriverI2C> i2c;
        std::shared_ptr<drivers::DriverGPIO> gpio;

        drivers::I2CAddress fuelGaugeAddress      = {FUEL_GAUGE_I2C_ADDR, 0, i2cSubaddresSize};
        drivers::I2CAddress batteryChargerAddress = {BATTERY_CHARGER_I2C_ADDR, 0, i2cSubaddresSize};
        drivers::I2CAddress topControllerAddress  = {TOP_CONTROLLER_I2C_ADDR, 0, i2cSubaddresSize};

        xQueueHandle IRQQueueHandle = nullptr;
        xQueueHandle DCDQueueHandle = nullptr;

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

        batteryRetval unlockProtectedChargerRegisters()
        {
            if (chargerWrite(Registers::CHG_CNFG_06, UNLOCK_CHARGER) != kStatus_Success) {
                LOG_ERROR("Charger registers unlock failed!");
                return batteryRetval::ChargerError;
            }
            return batteryRetval::OK;
        }

        batteryRetval lockProtectedChargerRegisters()
        {
            if (chargerWrite(Registers::CHG_CNFG_06, 0) != kStatus_Success) {
                LOG_ERROR("Charger registers lock failed!");
                return batteryRetval::ChargerError;
            }
            return batteryRetval::OK;
        }

        void enableCharging()
        {
            if (chargerWrite(Registers::CHG_CNFG_00, CHG_ON_OTG_OFF_BUCK_ON) != kStatus_Success) {
                LOG_ERROR("Charge enable fail");
            }
        }

        void disableCharging()
        {
            if (chargerWrite(Registers::CHG_CNFG_00, CHG_OFF_OTG_OFF_BUCK_ON) != kStatus_Success) {
                LOG_ERROR("Charge disable fail");
            }
        }

        void configureBatteryCharger()
        {
            unlockProtectedChargerRegisters();

            std::uint8_t value = CHARGE_TARGET_VOLTAGE | VSYS_MIN;
            if (chargerWrite(Registers::CHG_CNFG_04, value) != kStatus_Success) {
                LOG_ERROR("Charge target voltage write fail");
            }

            value = MAX_CHARGE_CURRENT;
            if (chargerWrite(Registers::CHG_CNFG_09, value) != kStatus_Success) {
                LOG_ERROR("Maximum charge current write fail");
            }

            value = FAST_CHARGE_CURRENT;
            if (chargerWrite(Registers::CHG_CNFG_02, value) != kStatus_Success) {
                LOG_ERROR("Fast charge current write fail");
            }

            lockProtectedChargerRegisters();
        }

        std::uint8_t getChargerDetails()
        {
            auto status = chargerRead(Registers::CHG_DETAILS_01);
            return status.second & 0x0F;
        }

        batteryRetval configureFuelGaugeBatteryModel()
        {
            int status = fuelGaugeWrite(Registers::LearnCFG_REG, fuel_gauge_params::LearnCFG);
            status |= fuelGaugeWrite(Registers::FilterCFG_REG, fuel_gauge_params::FilterCFG);
            status |= fuelGaugeWrite(Registers::MiscCFG_REG, fuel_gauge_params::MiscCFG);
            status |= fuelGaugeWrite(Registers::RelaxCFG_REG, fuel_gauge_params::RelaxCFG);
            status |= fuelGaugeWrite(Registers::RCOMP0_REG, fuel_gauge_params::RCOMP0);
            status |= fuelGaugeWrite(Registers::TempCo_REG, fuel_gauge_params::TempCo);
            status |= fuelGaugeWrite(Registers::QRtable00_REG, fuel_gauge_params::QResidual00);
            status |= fuelGaugeWrite(Registers::QRtable10_REG, fuel_gauge_params::QResidual10);
            status |= fuelGaugeWrite(Registers::QRtable20_REG, fuel_gauge_params::QResidual20);
            status |= fuelGaugeWrite(Registers::QRtable30_REG, fuel_gauge_params::QResidual30);
            status |= fuelGaugeWrite(Registers::FullCAPNom_REG, fuel_gauge_params::FullCAPNom);
            status |= fuelGaugeWrite(Registers::FullCapRep_REG, fuel_gauge_params::FullCAPRep);
            status |= fuelGaugeWrite(Registers::DesignCap_REG, fuel_gauge_params::DesignCap);
            status |= fuelGaugeWrite(Registers::dQ_acc_REG, fuel_gauge_params::dQacc);
            status |= fuelGaugeWrite(Registers::dP_acc_REG, fuel_gauge_params::dPacc);
            status |= fuelGaugeWrite(Registers::ICHGTERM_REG, fuel_gauge_params::ICHGTerm);
            status |= fuelGaugeWrite(Registers::VEmpty_REG, fuel_gauge_params::V_empty);
            status |= fuelGaugeWrite(Registers::FullSOCthr_REG, fuel_gauge_params::FullSOCthr);

            if (status != kStatus_Success) {
                LOG_ERROR("configureFuelGaugeBatteryModel failed.");
                return batteryRetval::ChargerError;
            }
            return batteryRetval::OK;
        }

        batteryRetval resetFuelGaugeModel()
        {
            auto regVal = fuelGaugeRead(Registers::CONFIG2_REG);

            std::uint16_t toWrite = regVal.second | static_cast<std::uint16_t>(CONFIG2::LdMdl);

            if (fuelGaugeWrite(Registers::CONFIG2_REG, toWrite) != kStatus_Success) {
                LOG_ERROR("resetFuelGaugeModel failed.");
                return batteryRetval::ChargerError;
            }
            return batteryRetval::OK;
        }

        batteryRetval storeConfiguration()
        {
            LOG_INFO("Storing fuel gauge configuration...");
            std::ofstream file(cfgFile.c_str(), std::ios::binary | std::ios::out);
            if (!file.is_open()) {
                LOG_WARN("Configuration file [%s] could not be opened.", cfgFile.c_str());
                return batteryRetval::ChargerError;
            }

            for (unsigned int i = 0; i < registersToStore; ++i) {
                auto regVal = fuelGaugeRead(static_cast<Registers>(i));
                if (regVal.first != kStatus_Success) {
                    LOG_ERROR("Reading register 0x%x failed.", i);
                    file.close();
                    return batteryRetval::ChargerError;
                }
                file.write(reinterpret_cast<const char *>(&regVal.second), sizeof(std::uint16_t));
            }
            file.close();

            return batteryRetval::OK;
        }

        batteryRetval loadConfiguration()
        {
            std::ifstream file(cfgFile.c_str(), std::ios::binary | std::ios::in);
            if (!file.is_open()) {
                LOG_WARN("Configuration file [%s] could not be opened. Loading initial configuration.",
                         cfgFile.c_str());
                if (configureFuelGaugeBatteryModel() == batteryRetval::OK) {
                    storeConfiguration();
                    resetFuelGaugeModel();
                    return batteryRetval::OK;
                }
                else {
                    return batteryRetval::ChargerError;
                }
            }

            std::uint16_t regVal;
            for (auto i = 0; i < registersToStore; ++i) {
                file.read(reinterpret_cast<char *>(&regVal), sizeof(std::uint16_t));
                if (fuelGaugeWrite(static_cast<Registers>(i), regVal) != kStatus_Success) {
                    LOG_ERROR("Writing register 0x%x failed.", i);
                    file.close();
                    return batteryRetval::ChargerError;
                }
            }
            file.close();
            resetFuelGaugeModel();

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

        batteryRetval fillConfig2RegisterValue()
        {
            std::uint16_t regVal =
                static_cast<std::uint16_t>(
                    CONFIG2::dSOCen) | // SOC 1% change alert
                                       //    static_cast<std::uint16_t>(CONFIG2::TAlrtEn) | // Temperature alerts
                static_cast<std::uint16_t>(CONFIG2::OCVQen); // Enable  automatic empty compensation

            if (fuelGaugeWrite(Registers::CONFIG2_REG, regVal) != kStatus_Success) {
                LOG_ERROR("fillConfig2RegisterValue failed.");
                return batteryRetval::ChargerError;
            }

            return batteryRetval::OK;
        }

        batteryRetval fillConfigRegisterValue()
        {
            std::uint16_t regVal = static_cast<std::uint16_t>(CONFIG::Aen)      // Enable alerts
                                   | static_cast<std::uint16_t>(CONFIG::Ten)    // Enable tepreature conversion
                                   | static_cast<std::uint16_t>(CONFIG::ETHRM); // External thermistor

            if (fuelGaugeWrite(Registers::CONFIG_REG, regVal) != kStatus_Success) {
                LOG_ERROR("fillConfigRegisterValue failed.");
                return batteryRetval::ChargerError;
            }

            return batteryRetval::OK;
        }

        batteryRetval configureTemperatureMeasurement()
        {
            if ((fuelGaugeWrite(Registers::TGAIN_REG, temperatureConversionGain) &
                 fuelGaugeWrite(Registers::TOFF_REG, temperatureConversionOffset)) != kStatus_Success) {
                LOG_ERROR("configureTemperatureMeasurement failed.");
                return batteryRetval::ChargerError;
            }

            return batteryRetval::OK;
        }

        batteryRetval enableTopIRQs()
        {
            std::uint8_t val = ENABLE_ALL_IRQ_MASK;

            if (chargerTopControllerWrite(Registers::TOP_CONTROLL_IRQ_MASK_REG, val) != kStatus_Success) {
                LOG_ERROR("enableIRQs failed.");
                return batteryRetval::ChargerError;
            }

            return batteryRetval::OK;
        }

        batteryRetval enableChargerIRQs()
        {
            std::uint8_t mask = ~(static_cast<std::uint8_t>(CHG_MASK::CHG_M) |
                                  static_cast<std::uint8_t>(CHG_MASK::CHGIN_M)); // unmask IRQs

            if (chargerWrite(Registers::CHG_INT_MASK, mask) != kStatus_Success) {
                LOG_ERROR("enableChargerIRQs failed.");
                return batteryRetval::ChargerError;
            }

            return batteryRetval::OK;
        }

        void IRQPinsInit()
        {
            gpio =
                drivers::DriverGPIO::Create(static_cast<drivers::GPIOInstances>(BoardDefinitions::BATTERY_CHARGER_GPIO),
                                            drivers::DriverGPIOParams{});

            drivers::DriverGPIOPinParams INTBPinConfig;
            INTBPinConfig.dir      = drivers::DriverGPIOPinParams::Direction::Input;
            INTBPinConfig.irqMode  = drivers::DriverGPIOPinParams::InterruptMode::IntFallingEdge;
            INTBPinConfig.defLogic = 0;
            INTBPinConfig.pin      = static_cast<std::uint32_t>(BoardDefinitions::BATTERY_CHARGER_INTB_PIN);
            gpio->ConfPin(INTBPinConfig);

            gpio->EnableInterrupt(1 << static_cast<std::uint32_t>(BoardDefinitions::BATTERY_CHARGER_INTB_PIN));
        }

        int getCellTemperature()
        {
            auto value      = fuelGaugeRead(Registers::TEMP_REG);
            int temperature = value.second >> 8;
            if (value.second & 0x8000) {
                temperature *= -1;
            }
            return temperature;
        }

        int getCurrentMeasurement()
        {
            auto value = fuelGaugeRead(Registers::Current_REG);
            int current;
            // 2's compliment into decimal
            if (value.second & 0x8000) {
                // negative numbers
                std::bitset<16> currentBitset = std::bitset<16>(value.second - 1);
                currentBitset.flip();
                current =
                    static_cast<int>((static_cast<std::uint16_t>(currentBitset.to_ulong()) * -1) * currentSenseGain);
            }
            else {
                // positive numbers
                current = static_cast<int>(value.second * currentSenseGain);
            }
            return current;
        }

        int getCellVoltage()
        {
            auto value  = fuelGaugeRead(Registers::VCELL_REG);
            int voltage = value.second * voltageSenseGain;
            return voltage;
        }

        void chargingFinishedAction()
        {
            LOG_DEBUG("Charging finished.");
            storeConfiguration();
        }

        void processTemperatureRange(TemperatureRanges temperatureRange)
        {
            switch (temperatureRange) {
            case TemperatureRanges::normal:
                LOG_DEBUG("Normal temperature range, charging enabled.");
                enableCharging();
                setTemperatureThresholds(maxTemperatureDegrees, minTemperatureDegrees);
                break;
            case TemperatureRanges::cold:
                LOG_DEBUG("Temperature too low, charging disabled.");
                disableCharging();
                setTemperatureThresholds(minTemperatureDegrees + temperatureHysteresis, minDisabled);
                break;
            case TemperatureRanges::hot:
                LOG_DEBUG("Temperature too high, charging disabled.");
                disableCharging();
                setTemperatureThresholds(maxDisabled, maxTemperatureDegrees - temperatureHysteresis);
                break;
            }
        }

    } // namespace

    int init(xQueueHandle irqQueueHandle, xQueueHandle dcdQueueHandle)
    {
        drivers::DriverI2CParams i2cParams;
        i2cParams.baudrate = static_cast<std::uint32_t>(BoardDefinitions::BATTERY_CHARGER_I2C_BAUDRATE);
        i2c = drivers::DriverI2C::Create(static_cast<drivers::I2CInstances>(BoardDefinitions::BATTERY_CHARGER_I2C),
                                         i2cParams);

        IRQQueueHandle = irqQueueHandle;
        DCDQueueHandle = dcdQueueHandle;

        configureBatteryCharger();

        // check Power-On reset bit
        std::uint16_t status = fuelGaugeRead(Registers::STATUS_REG).second;

        if (status & static_cast<std::uint16_t>(STATUS::POR)) {
            LOG_INFO("Initializing battery fuel gauge model.");
            loadConfiguration();
        }

        setTemperatureThresholds(maxTemperatureDegrees, minTemperatureDegrees);
        setServiceVoltageThresholds(maxVoltagemV, minVoltagemV);

        fillConfigRegisterValue();
        fillConfig2RegisterValue();
        configureTemperatureMeasurement();

        StateOfCharge level = getBatteryLevel();
        bool charging       = getChargeStatus();
        LOG_INFO("Phone battery start state: %d %d", level, charging);

        clearAllChargerIRQs();
        clearFuelGuageIRQ(static_cast<std::uint16_t>(batteryINTBSource::all));
        enableTopIRQs();
        enableChargerIRQs();
        IRQPinsInit();

        return 0;
    }

    void deinit()
    {
        storeConfiguration();

        gpio->DisableInterrupt(1 << static_cast<uint32_t>(BoardDefinitions::BATTERY_CHARGER_INTB_PIN));

        IRQQueueHandle = nullptr;
        DCDQueueHandle = nullptr;

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
        // read clears state
        auto IRQSource = chargerRead(Registers::CHG_INT_REG);
        if (IRQSource.first != kStatus_Success) {
            LOG_ERROR("failed to read charge INT source");
        }
        auto summary = chargerRead(Registers::CHG_INT_OK);
        if (summary.first != kStatus_Success) {
            LOG_ERROR("failed to read charge summary");
        }
        auto chargerDetails = getChargerDetails();

        if (summary.second & static_cast<std::uint8_t>(CHG_INT::CHGIN_I)) {
            Store::Battery::modify().state = Store::Battery::State::Charging;
        }
        else {
            Store::Battery::modify().state = Store::Battery::State::Discharging;
        }

        switch (chargerDetails) {
        case CHG_DETAILS_01::CHARGER_DONE:
            Store::Battery::modify().state = Store::Battery::State::PluggedNotCharging;
            chargingFinishedAction();
            break;
        case CHG_DETAILS_01::CHARGER_OFF:
            // IRQ from other source than CHGIN && Charger already plugged
            if (!(IRQSource.second & static_cast<std::uint8_t>(CHG_INT::CHGIN_I)) &&
                summary.second & static_cast<std::uint8_t>(CHG_INT::CHGIN_I)) {
                Store::Battery::modify().state = Store::Battery::State::PluggedNotCharging;
            }
            break;
        case CHG_DETAILS_01::CHARGER_PREQUALIFICATION:
            [[fallthrough]];
        case CHG_DETAILS_01::CHARGER_CC:
            [[fallthrough]];
        case CHG_DETAILS_01::CHARGER_CV:
            [[fallthrough]];
        case CHG_DETAILS_01::CHARGER_TOPOFF:
            Store::Battery::modify().state = Store::Battery::State::Charging;
            break;
        }

        return (Store::Battery::get().state == Store::Battery::State::PluggedNotCharging ||
                Store::Battery::get().state == Store::Battery::State::Charging);
    }

    std::uint16_t getStatusRegister()
    {
        auto status = fuelGaugeRead(Registers::STATUS_REG);
        return status.second;
    }

    void clearAllChargerIRQs()
    {
        auto value = chargerRead(Registers::CHG_INT_REG);
        if (value.second != 0) {
            // write zero to clear irq source
            chargerWrite(Registers::CHG_INT_REG, 0);
        }
    }

    void clearFuelGuageIRQ(std::uint16_t intToClear)
    {
        auto readout          = fuelGaugeRead(Registers::STATUS_REG);
        std::uint16_t toWrite = readout.second & (~intToClear);
        fuelGaugeWrite(Registers::STATUS_REG, toWrite);
    }

    void checkTemperatureRange()
    {
        TemperatureRanges temperatureRange;

        int temperature = getCellTemperature();
        LOG_DEBUG("Cell temperature: %d", temperature);
        if (temperature >= maxTemperatureDegrees) {
            temperatureRange = TemperatureRanges::hot;
        }
        else if (temperature > minTemperatureDegrees) {
            temperatureRange = TemperatureRanges::normal;
        }
        else {
            temperatureRange = TemperatureRanges::cold;
        }

        processTemperatureRange(temperatureRange);
    }

    std::uint8_t getTopControllerINTSource()
    {
        auto value = chargerTopControllerRead(Registers::TOP_CONTROLL_IRQ_SRC_REG);
        return value.second;
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

    extern "C"
    {
        void USB_ChargerDetectedCB(std::uint8_t detectedType)
        {
            BaseType_t xHigherPriorityTaskWoken = pdFALSE;
            if (DCDQueueHandle != nullptr) {
                std::uint8_t val = static_cast<std::uint8_t>(detectedType);
                xQueueSend(DCDQueueHandle, &val, portMAX_DELAY);
            }
        }
    }
} // namespace bsp::battery_charger
