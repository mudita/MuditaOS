// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "battery_charger.hpp"
#include "battery_charger_utils.hpp"

#include <fsl_common.h>
#include "FreeRTOS.h"
#include "task.h"

#include <board/BoardDefinitions.hpp>
#include <drivers/gpio/DriverGPIO.hpp>
#include <drivers/i2c/DriverI2C.hpp>
#include <purefs/filesystem_paths.hpp>

#include <utility>
#include <map>
#include <log/log.hpp>
#include <vector>
#include <magic_enum.hpp>
#include <crc32.h>

namespace bsp::battery_charger
{
    namespace
    {
        using Crc32 = std::uint32_t;

        constexpr std::uint32_t i2cSubaddresSize = 1;

        const auto cfgFile = purefs::dir::getSystemVarDirPath() / "batteryFuelGaugeConfig.cfg";

        constexpr auto registersToStore              = 0xFF + 1;
        constexpr auto configFileSizeWithoutChecksum = registersToStore * sizeof(Register);
        constexpr auto configFileSizeWithChecksum    = configFileSizeWithoutChecksum + sizeof(Crc32);

        constexpr std::uint16_t ENABLE_CHG_FG_IRQ_MASK = 0xFC;
        constexpr std::uint8_t UNLOCK_CHARGER          = 0x3 << 2;

        constexpr std::uint8_t CHG_ON_OTG_OFF_BUCK_ON  = 0b00000101;
        constexpr std::uint8_t CHG_OFF_OTG_OFF_BUCK_ON = 0b00000100;

        constexpr std::uint8_t VSYS_MIN = 0x80; // 3.6V

        constexpr std::uint16_t nominalCapacitymAh = 1600;

        constexpr units::SOC fullyChargedSOC       = 100;
        constexpr units::SOC dischargingSocDelta   = 5;
        constexpr units::SOC chargingSocDelta      = 1;
        constexpr units::SOC maxSocPercentageError = 20;

        constexpr std::uint16_t maxVoltagemV = 4400;
        constexpr std::uint16_t minVoltagemV = 3600;

        constexpr auto currentSenseGain = 0.15625;  // mA
        constexpr auto voltageSenseGain = 0.078125; // mV

        constexpr std::uint16_t maxMinMilliVoltGain = 20;

        // NTC calibration values
        constexpr std::uint16_t temperatureConversionGain   = 0xEE56;
        constexpr std::uint16_t temperatureConversionOffset = 0x1DA4;

        namespace fuel_gauge_params
        {
            /// Parameters calculated in in MAXIM EVKIT software
            /// Initial parameters for fuel gauge battery model

            constexpr std::uint16_t LearnCFG    = 0x2602;
            constexpr std::uint16_t FilterCFG   = 0xCEA0; // Current filter Tc = 0.7s
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

        enum class fileConfigRetval
        {
            OK,
            MissingFileError,
            DataSizeError,
            FileCrcError,
            ReadingRegisterError,
            WritingRegisterError,
            StoreVerificationError,
            ConfigFuelGaugeModelError
        };

        enum class TemperatureRanges
        {
            Cold,
            Cdeg1to15,
            Cdeg15to35,
            Cdeg35to45,
            Hot
        };

        struct TemparatureThresholds
        {
            int lowTemperatureThreshold;
            int highTemperatureThreshold;
            std::uint8_t alertLow;
            std::uint8_t alertHigh;
        };

        class RawSocValue
        {
          public:
            bool update(const units::SOC newSoc)
            {
                last    = current;
                current = newSoc;
                return current == last;
            }

          private:
            units::SOC current{};
            units::SOC last{};
        };
        RawSocValue rawSoc{};

        constexpr std::uint8_t maxAlertDisabled{0x7F};
        constexpr std::uint8_t minAlertDisabled{0x80};

        // INT is triggered if T>=maxThreshold || T<minThreshold
        constexpr std::uint8_t lowHysteresis{1};
        constexpr std::uint8_t highHysteresis{2};

        const std::map<TemperatureRanges, TemparatureThresholds> temperatureRanges{
            {TemperatureRanges::Cold, {std::numeric_limits<std::int8_t>::min(), 1, minAlertDisabled, 1}},
            {TemperatureRanges::Cdeg1to15, {1, 15, 0, 15 + highHysteresis}},
            {TemperatureRanges::Cdeg15to35, {15, 35, 15 - lowHysteresis, 35 + highHysteresis}},
            {TemperatureRanges::Cdeg35to45, {35, 45, 35 - lowHysteresis, 45 + highHysteresis}},
            {TemperatureRanges::Hot,
             {45, std::numeric_limits<std::int8_t>::max(), 45 - lowHysteresis, maxAlertDisabled}}};

        std::shared_ptr<drivers::DriverI2C> i2c;
        std::shared_ptr<drivers::DriverGPIO> gpio;

        drivers::I2CAddress fuelGaugeAddress      = {FUEL_GAUGE_I2C_ADDR, 0, i2cSubaddresSize};
        drivers::I2CAddress batteryChargerAddress = {BATTERY_CHARGER_I2C_ADDR, 0, i2cSubaddresSize};
        drivers::I2CAddress topControllerAddress  = {TOP_CONTROLLER_I2C_ADDR, 0, i2cSubaddresSize};

        void addChecksumToConfigFile()
        {
            std::fstream file(cfgFile.c_str(), std::ios::binary | std::ios::in | std::ios::out);
            if (!file.is_open()) {
                return;
            }

            CRC32 checksum;
            std::uint16_t regVal;
            for (auto i = 0; i < registersToStore; ++i) {
                file.read(reinterpret_cast<char *>(&regVal), sizeof(std::uint16_t));
                checksum.add(&regVal, sizeof(std::uint16_t));
            }
            const auto result = checksum.getHashValue();
            file.write(reinterpret_cast<const char *>(&result), sizeof(result));
            file.close();
        }

        bool isCorrectChecksumFromConfigFile()
        {
            bool ret = false;
            std::ifstream file(cfgFile.c_str(), std::ios::binary | std::ios::in);
            if (!file.is_open()) {
                return false;
            }

            const auto size = bsp::battery_charger::utils::getFileSize(file);
            switch (size) {
            case configFileSizeWithoutChecksum:
                ret = true;
                break;
            case configFileSizeWithChecksum: {
                CRC32 checksum;
                std::uint16_t regVal;
                for (auto i = 0; i < registersToStore; ++i) {
                    file.read(reinterpret_cast<char *>(&regVal), sizeof(std::uint16_t));
                    checksum.add(&regVal, sizeof(std::uint16_t));
                }
                std::uint32_t fileChecksum;
                file.read(reinterpret_cast<char *>(&fileChecksum), sizeof(std::uint32_t));
                ret = fileChecksum == checksum.getHashValue();
            } break;
            default:
                ret = false;
                break;
            }
            file.close();
            return ret;
        }

        units::SOC getBatteryLevelFromRegisterValue(const Register registerValue)
        {
            /// 16 bit result. The high byte indicates 1% per LSB. The low byte reports fractional percent. We don't
            /// care about fractional part.
            return (registerValue & 0xff00) >> 8;
        }

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

        void setMaxChargeCurrent(ChargeCurrentLimit limit)
        {
            unlockProtectedChargerRegisters();
            const auto value = static_cast<std::uint8_t>(limit);
            if (chargerWrite(Registers::CHG_CNFG_02, value) != kStatus_Success) {
                LOG_ERROR("Fast charge current write fail");
            }
            lockProtectedChargerRegisters();
        }

        void setChargeTargetVoltage(ChargeTerminationVoltage voltage)
        {
            unlockProtectedChargerRegisters();
            std::uint8_t value = static_cast<std::uint8_t>(voltage) | VSYS_MIN;
            if (chargerWrite(Registers::CHG_CNFG_04, value) != kStatus_Success) {
                LOG_ERROR("Charge target voltage write fail");
            }
            lockProtectedChargerRegisters();
        }

        void resetUSBCurrrentLimit()
        {
            LOG_INFO("USB current limit set to 500mA");
            setMaxBusCurrent(USBCurrentLimit::lim500mA);
        }

        void configureBatteryCharger()
        {
            setChargeTargetVoltage(ChargeTerminationVoltage::mV4350);
            resetUSBCurrrentLimit();
            setMaxChargeCurrent(ChargeCurrentLimit::lim1600mA);
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

        fileConfigRetval saveConfigurationFile(std::vector<Register> &savedData)
        {
            std::ofstream file(cfgFile.c_str(), std::ios::binary | std::ios::out);
            if (!file.is_open()) {
                return fileConfigRetval::MissingFileError;
            }
            if (savedData.size() != registersToStore) {
                return fileConfigRetval::DataSizeError;
            }
            for (unsigned int i = 0; i < registersToStore; ++i) {
                auto regVal = fuelGaugeRead(static_cast<Registers>(i));
                if (regVal.first != kStatus_Success) {
                    file.close();
                    return fileConfigRetval::ReadingRegisterError;
                }
                file.write(reinterpret_cast<const char *>(&regVal.second), sizeof(Register));
                savedData[i] = regVal.second;
            }
            file.close();
            addChecksumToConfigFile();
            return fileConfigRetval::OK;
        }

        fileConfigRetval verifySavedConfigurationFile(const std::vector<Register> &dataForVerification)
        {
            std::ifstream readFile(cfgFile.c_str(), std::ios::binary | std::ios::in);
            if (!readFile.is_open()) {
                return fileConfigRetval::MissingFileError;
            }
            if (dataForVerification.size() != registersToStore) {
                return fileConfigRetval::DataSizeError;
            }
            std::uint16_t savedRegValue;
            for (unsigned int i = 0; i < registersToStore; ++i) {
                readFile.read(reinterpret_cast<char *>(&savedRegValue), sizeof(savedRegValue));
                if (savedRegValue != dataForVerification[i]) {
                    readFile.close();
                    return fileConfigRetval::StoreVerificationError;
                }
            }
            readFile.close();
            return fileConfigRetval::OK;
        }

        fileConfigRetval readConfigurationFile(std::vector<Register> &readData)
        {
            std::ifstream file(cfgFile.c_str(), std::ios::binary | std::ios::in);
            if (!file.is_open()) {
                return fileConfigRetval::MissingFileError;
            }
            if (readData.size() != registersToStore) {
                return fileConfigRetval::DataSizeError;
            }
            std::uint16_t regVal;
            for (auto i = 0; i < registersToStore; ++i) {
                file.read(reinterpret_cast<char *>(&regVal), sizeof(regVal));
                readData[i] = regVal;
            }
            file.close();
            if (!isCorrectChecksumFromConfigFile()) {
                return fileConfigRetval::FileCrcError;
            }
            return fileConfigRetval::OK;
        }

        fileConfigRetval storeFuelGaugeRegisters(const std::vector<Register> &writeData)
        {
            if (writeData.size() != registersToStore) {
                return fileConfigRetval::DataSizeError;
            }
            for (auto i = 0; i < registersToStore; ++i) {
                if (fuelGaugeWrite(static_cast<Registers>(i), writeData[i]) != kStatus_Success) {
                    return fileConfigRetval::WritingRegisterError;
                }
            }
            return fileConfigRetval::OK;
        }

        bool isSocDataDiscrepancy(const units::SOC SoC1, const units::SOC SoC2)
        {
            return (std::abs(SoC1 - SoC2) > maxSocPercentageError);
        }

        fileConfigRetval storeConfiguration()
        {
            std::vector<Register> storedData(registersToStore);

            if (auto retVal = saveConfigurationFile(storedData); retVal != fileConfigRetval::OK) {
                LOG_ERROR("Save configuration file error: %s", magic_enum::enum_name(retVal).data());
                return retVal;
            }
            if (auto retVal = verifySavedConfigurationFile(storedData); retVal != fileConfigRetval::OK) {
                LOG_ERROR("Verify configuration file error: %s", magic_enum::enum_name(retVal).data());
                return retVal;
            }

            const auto soc = getBatteryLevelFromRegisterValue(storedData[static_cast<Register>(Registers::RepSOC_REG)]);
            LOG_INFO("Storing fuel gauge configuration (RawSoC: %d)", soc);

            return fileConfigRetval::OK;
        }

        fileConfigRetval loadConfiguration()
        {
            // First, we load the default battery configuration
            if (configureFuelGaugeBatteryModel() == batteryRetval::OK) {
                resetFuelGaugeModel();
            }
            else {
                LOG_ERROR("Configure fuel gauge battery model failed.");
                return fileConfigRetval::ConfigFuelGaugeModelError;
            }

            const auto batteryLevelAfterDefaultConfiguration = getBatteryLevel();
            if (batteryLevelAfterDefaultConfiguration != std::nullopt) {
                rawSoc.update(batteryLevelAfterDefaultConfiguration.value());
            }
            std::vector<Register> readData(registersToStore);

            // then we read the battery configuration from the saved file
            if (auto retVal = readConfigurationFile(readData); retVal != fileConfigRetval::OK) {
                // if there is a problem with reading the data, leave the default configuration and exit
                LOG_ERROR("Read configuration file error: %s", magic_enum::enum_name(retVal).data());
                storeConfiguration();
                return fileConfigRetval::OK;
            }

            const auto savedBatteryLevel =
                getBatteryLevelFromRegisterValue(readData[static_cast<Register>(Registers::RepSOC_REG)]);

            // if the difference in SOC readings is less than 'maxSocPercentageError' then load the configuration read
            // from the file
            if (batteryLevelAfterDefaultConfiguration &&
                not isSocDataDiscrepancy(batteryLevelAfterDefaultConfiguration.value(), savedBatteryLevel)) {
                if (auto retVal = storeFuelGaugeRegisters(readData); retVal != fileConfigRetval::OK) {
                    LOG_ERROR("Store configuration file error: %s", magic_enum::enum_name(retVal).data());
                    return retVal;
                }
                resetFuelGaugeModel();
            }
            else {
                LOG_WARN("The discrepancy between SOC from the file %d and the current value %d. Loading initial "
                         "configuration.",
                         savedBatteryLevel,
                         batteryLevelAfterDefaultConfiguration.value());
                storeConfiguration();
            }

            return fileConfigRetval::OK;
        }

        batteryRetval setTemperatureThresholds(std::uint8_t minTemperatureDegrees, std::uint8_t maxTemperatureDegrees)
        {
            std::uint16_t regVal = (static_cast<uint16_t>(maxTemperatureDegrees) << 8) | minTemperatureDegrees;
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
            std::uint16_t regVal = static_cast<std::uint16_t>(CONFIG2::dSOCen) |  // SOC 1% change alert
                                   static_cast<std::uint16_t>(CONFIG2::TAlrtEn) | // Temperature alerts
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
            std::uint8_t val = ENABLE_CHG_FG_IRQ_MASK;

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

            drivers::DriverGPIOPinParams INTBPinConfig{};
            INTBPinConfig.dir      = drivers::DriverGPIOPinParams::Direction::Input;
            INTBPinConfig.irqMode  = drivers::DriverGPIOPinParams::InterruptMode::IntFallingEdge;
            INTBPinConfig.defLogic = 0;
            INTBPinConfig.pin      = static_cast<std::uint32_t>(BoardDefinitions::BATTERY_CHARGER_INTB_PIN);
            gpio->ConfPin(INTBPinConfig);

            drivers::DriverGPIOPinParams INOKBPinConfig{};
            INOKBPinConfig.dir      = drivers::DriverGPIOPinParams::Direction::Input;
            INOKBPinConfig.irqMode  = drivers::DriverGPIOPinParams::InterruptMode::IntRisingOrFallingEdge;
            INOKBPinConfig.defLogic = 0;
            INOKBPinConfig.pin      = static_cast<std::uint32_t>(BoardDefinitions::BATTERY_CHARGER_INOKB_PIN);
            gpio->ConfPin(INOKBPinConfig);

            gpio->EnableInterrupt(1 << static_cast<std::uint32_t>(BoardDefinitions::BATTERY_CHARGER_INTB_PIN));
            gpio->EnableInterrupt(1 << static_cast<std::uint32_t>(BoardDefinitions::BATTERY_CHARGER_INOKB_PIN));
        }

        int getCellTemperature()
        {
            auto value = fuelGaugeRead(Registers::TEMP_REG);
            // Round to integer by stripping fractions
            std::uint8_t temperatureInt = value.second >> 8;
            return utils::twosComplimentToInt(temperatureInt);
        }

        void chargingFinishedAction()
        {
            LOG_DEBUG("Charging finished.");
        }

        void processTemperatureRange(TemperatureRanges temperatureRange)
        {
            switch (temperatureRange) {
            case TemperatureRanges::Cdeg1to15:
                LOG_DEBUG("1 to 15 cell temperature range");
                enableCharging();
                setChargeTargetVoltage(ChargeTerminationVoltage::mV4350);
                setMaxChargeCurrent(ChargeCurrentLimit::lim300mA);
                break;
            case TemperatureRanges::Cdeg15to35:
                LOG_DEBUG("15 to 35 cell temperature range");
                enableCharging();
                setChargeTargetVoltage(ChargeTerminationVoltage::mV4350);
                setMaxChargeCurrent(ChargeCurrentLimit::lim1600mA);
                break;
            case TemperatureRanges::Cdeg35to45:
                LOG_DEBUG("35 to 45 cell temperature range");
                enableCharging();
                setChargeTargetVoltage(ChargeTerminationVoltage::mV4100);
                setMaxChargeCurrent(ChargeCurrentLimit::lim1600mA);
                break;
            case TemperatureRanges::Cold:
                LOG_DEBUG("Cell temperature too low, charging disabled.");
                disableCharging();
                break;
            case TemperatureRanges::Hot:
                LOG_DEBUG("Cell temperature too high, charging disabled.");
                disableCharging();
                break;
            }

            setTemperatureThresholds(temperatureRanges.at(temperatureRange).alertLow,
                                     temperatureRanges.at(temperatureRange).alertHigh);
        }

    } // namespace

    int init()
    {
        drivers::DriverI2CParams i2cParams{};
        i2cParams.baudrate = static_cast<std::uint32_t>(BoardDefinitions::BATTERY_CHARGER_I2C_BAUDRATE);
        i2c = drivers::DriverI2C::Create(static_cast<drivers::I2CInstances>(BoardDefinitions::BATTERY_CHARGER_I2C),
                                         i2cParams);

        configureBatteryCharger();

        // check Power-On reset bit
        std::uint16_t status = fuelGaugeRead(Registers::STATUS_REG).second;

        if (status & static_cast<std::uint16_t>(STATUS::POR)) {
            LOG_INFO("Initializing battery fuel gauge model.");
            loadConfiguration();
        }

        setServiceVoltageThresholds(maxVoltagemV, minVoltagemV);

        fillConfigRegisterValue();
        fillConfig2RegisterValue();
        configureTemperatureMeasurement();

        checkTemperatureRange();

        // Short time to synchronize after configuration
        vTaskDelay(pdMS_TO_TICKS(100));

        clearAllChargerIRQs();
        clearFuelGuageIRQ(static_cast<std::uint16_t>(batteryINTBSource::all));
        enableTopIRQs();
        enableChargerIRQs();
        IRQPinsInit();

        return 0;
    }

    void deinit()
    {
        resetUSBCurrrentLimit();
        storeConfiguration();

        gpio->DisableInterrupt(1 << static_cast<uint32_t>(BoardDefinitions::BATTERY_CHARGER_INTB_PIN));
        gpio->DisableInterrupt(1 << static_cast<std::uint32_t>(BoardDefinitions::BATTERY_CHARGER_INOKB_PIN));

        i2c.reset();
        gpio.reset();
    }

    bool levelSocToStore(const units::SOC soc, std::uint16_t percentLevelDelta)
    {
        return (soc % percentLevelDelta) == 0;
    }

    void storeBatteryLevelChange(const units::SOC newSocValue)
    {
        if (rawSoc.update(newSocValue)) {
            return;
        }

        const auto status = getChargeStatus();
        units::SOC percentLevelDelta;
        switch (status) {
        case batteryRetval::ChargerCharging:
            percentLevelDelta = chargingSocDelta;
            break;
        default:
            percentLevelDelta = dischargingSocDelta;
            break;
        }

        if (levelSocToStore(newSocValue, percentLevelDelta)) {
            storeConfiguration();
        }
    }

    std::optional<units::SOC> getBatteryLevel()
    {
        auto readout = fuelGaugeRead(Registers::RepSOC_REG);
        if (readout.first != kStatus_Success) {
            LOG_ERROR("failed to get battery percent");
            return std::nullopt;
        }
        auto soc = getBatteryLevelFromRegisterValue(readout.second);
        if (soc > fullyChargedSOC) {
            // Sometimes MAX77818 can return soc > 100% when config file is missing
            // or Soc is based on voltage, so we need to truncate value.
            LOG_WARN("Raw SOC value has been truncated (read value: %d)", soc);
            soc = fullyChargedSOC;
        }
        return soc;
    }

    batteryRetval getChargeStatus()
    {
        batteryRetval retVal{};
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
        auto chargingSetup  = chargerRead(Registers::CHG_CNFG_00);

        if (summary.second & static_cast<std::uint8_t>(CHG_INT::CHGIN_I)) {
            retVal = batteryRetval::ChargerCharging;
        }
        else {
            retVal = batteryRetval::ChargerNotCharging;
        }

        switch (static_cast<CHG_DETAILS_01>(chargerDetails)) {
        case CHG_DETAILS_01::CHARGER_DONE:
            retVal = batteryRetval::ChargingDone;
            chargingFinishedAction();
            break;
        case CHG_DETAILS_01::CHARGER_OFF:
            if (chargingSetup.second != CHG_ON_OTG_OFF_BUCK_ON) {
                retVal = batteryRetval::ChargerNotCharging;
            }
            break;
        case CHG_DETAILS_01::CHARGER_PREQUALIFICATION:
        case CHG_DETAILS_01::CHARGER_CC:
        case CHG_DETAILS_01::CHARGER_CV:
        case CHG_DETAILS_01::CHARGER_TOPOFF:
            retVal = batteryRetval::ChargerCharging;
            break;
        case CHG_DETAILS_01::CHARGER_TIMER_FAULT:
        case CHG_DETAILS_01::CHARGER_BATTERY_DETECT:
            retVal = batteryRetval::ChargerError;
            break;
        }

        return retVal;
    }

    Register getStatusRegister()
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
        int temperature = getCellTemperature();
        LOG_DEBUG("Cell temperature: %d", temperature);

        for (const auto &[range, thresholds] : temperatureRanges) {
            if ((temperature > thresholds.lowTemperatureThreshold) &&
                (temperature <= thresholds.highTemperatureThreshold)) {
                processTemperatureRange(range);
                break;
            }
        }
    }

    std::uint8_t getTopControllerINTSource()
    {
        auto value = chargerTopControllerRead(Registers::TOP_CONTROLL_IRQ_SRC_REG);
        return value.second;
    }

    void setMaxBusCurrent(USBCurrentLimit limit)
    {
        const auto value = static_cast<std::uint8_t>(limit);
        if (chargerWrite(Registers::CHG_CNFG_09, value) != kStatus_Success) {
            LOG_ERROR("Maximum usb current write fail");
        }
    }

    int getVoltageFilteredMeasurement()
    {
        const auto [_, value] = fuelGaugeRead(Registers::AvgVCELL_REG);
        int voltage           = value * voltageSenseGain;
        return voltage;
    }

    int getAvgCurrent()
    {
        const auto [_, value] = fuelGaugeRead(Registers::AvgCurrent_REG);
        return static_cast<int>(utils::twosComplimentToInt(value) * currentSenseGain);
    }

    int getCurrentMeasurement()
    {
        auto [_, value] = fuelGaugeRead(Registers::Current_REG);
        return static_cast<int>(utils::twosComplimentToInt(value) * currentSenseGain);
    }

    MaxMinVolt getMaxMinVolt()
    {
        MaxMinVolt ret;

        const auto [_, value] = fuelGaugeRead(Registers::MaxMinVolt_REG);
        ret.maxMilliVolt      = ((value & 0xFF00) >> 8) * maxMinMilliVoltGain;
        ret.minMilliVolt      = (value & 0xFF) * maxMinMilliVoltGain;

        return ret;
    }

    void printFuelGaugeInfo()
    {
        const auto maxMinVolt = getMaxMinVolt();

        LOG_INFO("Fuel Gauge info:");
        LOG_INFO("\tAvgVCell: %dmV", getVoltageFilteredMeasurement());
        LOG_INFO("\tMaxVolt: %dmV", maxMinVolt.maxMilliVolt);
        LOG_INFO("\tMinVolt: %dmV", maxMinVolt.minMilliVolt);
        LOG_INFO("\tAvgCurrent: %dmA", getAvgCurrent());
        LOG_INFO("\tRawSoC: %d%%", getBatteryLevel().value());
    }

    bool isChargerPlugged()
    {
        const auto chargerStatus = chargerRead(Registers::CHG_INT_OK);
        if (chargerStatus.first != kStatus_Success) {
            LOG_ERROR("failed to read charger status");
            return false;
        }
        return chargerStatus.second & static_cast<std::uint8_t>(CHG_INT::CHGIN_I);
    }
} // namespace bsp::battery_charger
