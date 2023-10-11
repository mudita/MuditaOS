// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
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

        constexpr std::uint32_t i2cSubaddressSize{1};

        const auto cfgFile = purefs::dir::getSystemVarDirPath() / "batteryFuelGaugeConfig.cfg";

        constexpr auto registersToStore{0xFF + 1};
        constexpr auto configFileSizeWithoutChecksum = registersToStore * sizeof(Register);
        constexpr auto configFileSizeWithChecksum    = configFileSizeWithoutChecksum + sizeof(Crc32);

        constexpr std::uint8_t VSYS_MIN{0x80}; // 3.6V

        constexpr units::SOC fullyChargedSOC{100};
        constexpr units::SOC dischargingSocDelta{5};
        constexpr units::SOC chargingSocDelta{1};
        constexpr units::SOC maxSocPercentageError{20};

        constexpr std::uint16_t maxVoltagemV{4400};
        constexpr std::uint16_t minVoltagemV{3600};

        constexpr auto currentSenseGain{0.15625f};  // mA
        constexpr auto voltageSenseGain{0.078125f}; // mV

        constexpr std::uint16_t maxMinMilliVoltGain{20};

        // NTC calibration values
        constexpr std::uint16_t temperatureConversionGain{0xEE56};
        constexpr std::uint16_t temperatureConversionOffset{0x1DA4};

        namespace fuel_gauge_params
        {
            /// Parameters calculated in in MAXIM EVKIT software
            /// Initial parameters for fuel gauge battery model
            constexpr std::uint16_t LearnCFG{0x2602};
            constexpr std::uint16_t FilterCFG{0xCEA0}; // Current filter Tc = 0.7s
            constexpr std::uint16_t MiscCFG{0x01D0};
            constexpr std::uint16_t RelaxCFG{0x2039};
            constexpr std::uint16_t RCOMP0{0x0070};
            constexpr std::uint16_t TempCo{0x263D};
            constexpr std::uint16_t QResidual00{0x2280};
            constexpr std::uint16_t QResidual10{0x1000};
            constexpr std::uint16_t QResidual20{0x0681};
            constexpr std::uint16_t QResidual30{0x0682};
            /// 1600mAh initial battery capacity
            constexpr std::uint16_t FullCAPNom{0x0C80};
            constexpr std::uint16_t FullCAPRep{0x0C80};
            constexpr std::uint16_t DesignCap{0x0C80};
            constexpr std::uint16_t dQacc{0x00C8};
            constexpr std::uint16_t dPacc{0x0C80};
            /// Charge termination current = 150mA
            constexpr std::uint16_t ICHGTerm{0x03C0};
            /// Empty battery = 3V
            constexpr std::uint16_t V_empty{0x965A};
            /// Fully charged threshold = 90%
            constexpr std::uint16_t FullSOCthr{0x5A00};
        } // namespace fuel_gauge_params

        enum class FileConfigRetval
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

        struct TemperatureThresholds
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

        // INT is triggered if T >= maxThreshold || T < minThreshold
        constexpr std::uint8_t lowHysteresis{1};
        constexpr std::uint8_t highHysteresis{2};

        const std::map<TemperatureRanges, TemperatureThresholds> temperatureRanges{
            {TemperatureRanges::Cold, {std::numeric_limits<std::int8_t>::min(), 1, minAlertDisabled, 1}},
            {TemperatureRanges::Cdeg1to15, {1, 15, 0, 15 + highHysteresis}},
            {TemperatureRanges::Cdeg15to35, {15, 35, 15 - lowHysteresis, 35 + highHysteresis}},
            {TemperatureRanges::Cdeg35to45, {35, 45, 35 - lowHysteresis, 45 + highHysteresis}},
            {TemperatureRanges::Hot, {45, std::numeric_limits<std::int8_t>::max(), 45, maxAlertDisabled}}};

        std::shared_ptr<drivers::DriverI2C> i2c;
        std::shared_ptr<drivers::DriverGPIO> gpio;

        drivers::I2CAddress fuelGaugeAddress      = {FUEL_GAUGE_I2C_ADDR, 0, i2cSubaddressSize};
        drivers::I2CAddress batteryChargerAddress = {BATTERY_CHARGER_I2C_ADDR, 0, i2cSubaddressSize};
        drivers::I2CAddress topControllerAddress  = {TOP_CONTROLLER_I2C_ADDR, 0, i2cSubaddressSize};

        void addChecksumToConfigFile()
        {
            std::fstream file(cfgFile.c_str(), std::ios::binary | std::ios::in | std::ios::out);
            if (!file.is_open()) {
                return;
            }

            CRC32 checksum;
            std::uint16_t regVal;
            for (auto i = 0; i < registersToStore; ++i) {
                file.read(reinterpret_cast<char *>(&regVal), sizeof(regVal));
                checksum.add(&regVal, sizeof(regVal));
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
                    file.read(reinterpret_cast<char *>(&regVal), sizeof(regVal));
                    checksum.add(&regVal, sizeof(regVal));
                }
                std::uint32_t fileChecksum;
                file.read(reinterpret_cast<char *>(&fileChecksum), sizeof(fileChecksum));
                ret = (fileChecksum == checksum.getHashValue());
            } break;

            default:
                ret = false;
                break;
            }

            file.close();
            return ret;
        }

        inline constexpr bool isChargerInputValid(std::uint8_t chargerStatus)
        {
            return static_cast<bool>(chargerStatus & static_cast<std::uint8_t>(CHG_INT::CHGIN_I));
        }

        units::SOC getBatteryLevelFromRegisterValue(const Register registerValue)
        {
            /// 16 bit result. The high byte indicates 1% per LSB. The low byte reports fractional percent. We don't
            /// care about fractional part.
            return (registerValue & 0xFF00) >> 8;
        }

        int fuelGaugeWrite(Registers registerAddress, std::uint16_t value)
        {
            fuelGaugeAddress.subAddress = static_cast<std::uint32_t>(registerAddress);
            const auto ret = i2c->Write(fuelGaugeAddress, reinterpret_cast<std::uint8_t *>(&value), sizeof(value));

            if (ret != sizeof(value)) {
                return kStatus_Fail;
            }
            return kStatus_Success;
        }

        std::pair<int, std::uint16_t> fuelGaugeRead(Registers registerAddress)
        {
            std::uint16_t value;
            auto status                 = kStatus_Success;
            fuelGaugeAddress.subAddress = static_cast<std::uint32_t>(registerAddress);
            const auto ret = i2c->Read(fuelGaugeAddress, reinterpret_cast<std::uint8_t *>(&value), sizeof(value));

            if (ret != sizeof(value)) {
                status = kStatus_Fail;
            }
            return std::make_pair(status, value);
        }

        int chargerWrite(Registers registerAddress, std::uint8_t value)
        {
            batteryChargerAddress.subAddress = static_cast<std::uint32_t>(registerAddress);
            const auto ret = i2c->Write(batteryChargerAddress, reinterpret_cast<std::uint8_t *>(&value), sizeof(value));

            if (ret != sizeof(value)) {
                return kStatus_Fail;
            }
            return kStatus_Success;
        }

        std::pair<int, std::uint8_t> chargerRead(Registers registerAddress)
        {
            std::uint8_t value;
            auto status                      = kStatus_Success;
            batteryChargerAddress.subAddress = static_cast<std::uint32_t>(registerAddress);
            const auto ret                   = i2c->Read(batteryChargerAddress, &value, sizeof(value));

            if (ret != sizeof(value)) {
                status = kStatus_Fail;
            }
            return std::make_pair(status, value);
        }

        int chargerTopControllerWrite(Registers registerAddress, std::uint8_t value)
        {
            topControllerAddress.subAddress = static_cast<std::uint32_t>(registerAddress);
            const auto ret = i2c->Write(topControllerAddress, reinterpret_cast<std::uint8_t *>(&value), sizeof(value));

            if (ret != sizeof(value)) {
                return kStatus_Fail;
            }
            return kStatus_Success;
        }

        std::pair<int, std::uint8_t> chargerTopControllerRead(Registers registerAddress)
        {
            std::uint8_t value;
            auto status                     = kStatus_Success;
            topControllerAddress.subAddress = static_cast<std::uint32_t>(registerAddress);
            const auto ret                  = i2c->Read(topControllerAddress, &value, sizeof(value));
            if (ret != sizeof(value)) {
                status = kStatus_Fail;
            }
            return std::make_pair(status, value);
        }

        BatteryRetval unlockProtectedChargerRegisters()
        {
            if (chargerWrite(Registers::CHG_CNFG_06, static_cast<std::uint8_t>(ChargerSettingsProtection::Disable)) !=
                kStatus_Success) {
                LOG_ERROR("Failed to unlock charger registers");
                return BatteryRetval::ChargerError;
            }
            return BatteryRetval::OK;
        }

        BatteryRetval lockProtectedChargerRegisters()
        {
            if (chargerWrite(Registers::CHG_CNFG_06, static_cast<std::uint8_t>(ChargerSettingsProtection::Enable)) !=
                kStatus_Success) {
                LOG_ERROR("Failed to lock charger registers");
                return BatteryRetval::ChargerError;
            }
            return BatteryRetval::OK;
        }

        void enableCharging()
        {
            if (chargerWrite(Registers::CHG_CNFG_00, static_cast<std::uint8_t>(ChargerMode::ChargerOnBuckOn)) !=
                kStatus_Success) {
                LOG_ERROR("Failed to enable charger");
            }
        }

        void disableCharging()
        {
            if (chargerWrite(Registers::CHG_CNFG_00, static_cast<std::uint8_t>(ChargerMode::ChargerOffBuckOn)) !=
                kStatus_Success) {
                LOG_ERROR("Failed to disable charger");
            }
        }

        void setMaxChargeCurrent(ChargeCurrentLimit limit)
        {
            unlockProtectedChargerRegisters();
            const auto value = static_cast<std::uint8_t>(limit);
            if (chargerWrite(Registers::CHG_CNFG_02, value) != kStatus_Success) {
                LOG_ERROR("Failed to set fast charge current");
            }
            lockProtectedChargerRegisters();
        }

        void setChargeTargetVoltage(ChargeTerminationVoltage voltage)
        {
            unlockProtectedChargerRegisters();
            const auto value = static_cast<std::uint8_t>(voltage) | VSYS_MIN;
            if (chargerWrite(Registers::CHG_CNFG_04, value) != kStatus_Success) {
                LOG_ERROR("Failed to set charge target voltage");
            }
            lockProtectedChargerRegisters();
        }

        void resetUSBCurrentLimit()
        {
            LOG_INFO("USB current limit reset to 500mA");
            setMaxBusCurrent(USBCurrentLimit::lim500mA);
        }

        void configureBatteryCharger()
        {
            setChargeTargetVoltage(ChargeTerminationVoltage::mV4350);
            resetUSBCurrentLimit();
            setMaxChargeCurrent(ChargeCurrentLimit::lim1600mA);
        }

        std::optional<std::uint8_t> getChargerDetails()
        {
            const auto [status, value] = chargerRead(Registers::CHG_DETAILS_01);
            if (status != kStatus_Success) {
                LOG_ERROR("Failed to get charger details");
                return std::nullopt;
            }
            return value & 0x0F;
        }

        BatteryRetval configureFuelGaugeBatteryModel()
        {
            auto status = fuelGaugeWrite(Registers::LearnCFG_REG, fuel_gauge_params::LearnCFG);
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
                LOG_ERROR("Failed to configure fuel gauge battery model");
                return BatteryRetval::ChargerError;
            }
            return BatteryRetval::OK;
        }

        BatteryRetval resetFuelGaugeModel()
        {
            const auto [status, value] = fuelGaugeRead(Registers::CONFIG2_REG);
            if (status != kStatus_Success) {
                LOG_ERROR("Failed to read CONFIG2 register");
            }

            const std::uint16_t toWrite = value | static_cast<std::uint16_t>(CONFIG2::LdMdl);

            if (fuelGaugeWrite(Registers::CONFIG2_REG, toWrite) != kStatus_Success) {
                LOG_ERROR("Failed to reset fuel gauge model");
                return BatteryRetval::ChargerError;
            }
            return BatteryRetval::OK;
        }

        FileConfigRetval saveConfigurationFile(std::vector<Register> &savedData)
        {
            std::ofstream file(cfgFile.c_str(), std::ios::binary | std::ios::out);
            if (!file.is_open()) {
                return FileConfigRetval::MissingFileError;
            }
            if (savedData.size() != registersToStore) {
                return FileConfigRetval::DataSizeError;
            }

            for (auto i = 0; i < registersToStore; ++i) {
                const auto regVal = fuelGaugeRead(static_cast<Registers>(i));
                if (regVal.first != kStatus_Success) {
                    file.close();
                    return FileConfigRetval::ReadingRegisterError;
                }
                file.write(reinterpret_cast<const char *>(&regVal.second), sizeof(regVal.second));
                savedData[i] = regVal.second;
            }
            file.close();
            addChecksumToConfigFile();
            return FileConfigRetval::OK;
        }

        FileConfigRetval verifySavedConfigurationFile(const std::vector<Register> &dataForVerification)
        {
            std::ifstream readFile(cfgFile.c_str(), std::ios::binary | std::ios::in);
            if (!readFile.is_open()) {
                return FileConfigRetval::MissingFileError;
            }
            if (dataForVerification.size() != registersToStore) {
                return FileConfigRetval::DataSizeError;
            }

            std::uint16_t savedRegValue;
            for (auto i = 0; i < registersToStore; ++i) {
                readFile.read(reinterpret_cast<char *>(&savedRegValue), sizeof(savedRegValue));
                if (savedRegValue != dataForVerification[i]) {
                    readFile.close();
                    return FileConfigRetval::StoreVerificationError;
                }
            }
            readFile.close();
            return FileConfigRetval::OK;
        }

        FileConfigRetval readConfigurationFile(std::vector<Register> &readData)
        {
            std::ifstream file(cfgFile.c_str(), std::ios::binary | std::ios::in);
            if (!file.is_open()) {
                return FileConfigRetval::MissingFileError;
            }
            if (readData.size() != registersToStore) {
                return FileConfigRetval::DataSizeError;
            }
            std::uint16_t regVal;
            for (auto i = 0; i < registersToStore; ++i) {
                file.read(reinterpret_cast<char *>(&regVal), sizeof(regVal));
                readData[i] = regVal;
            }
            file.close();
            if (!isCorrectChecksumFromConfigFile()) {
                return FileConfigRetval::FileCrcError;
            }
            return FileConfigRetval::OK;
        }

        FileConfigRetval storeFuelGaugeRegisters(const std::vector<Register> &writeData)
        {
            if (writeData.size() != registersToStore) {
                return FileConfigRetval::DataSizeError;
            }

            for (auto i = 0; i < registersToStore; ++i) {
                if (fuelGaugeWrite(static_cast<Registers>(i), writeData[i]) != kStatus_Success) {
                    return FileConfigRetval::WritingRegisterError;
                }
            }
            return FileConfigRetval::OK;
        }

        bool isSocDataDiscrepancy(const units::SOC SoC1, const units::SOC SoC2)
        {
            return (std::abs(SoC1 - SoC2) > maxSocPercentageError);
        }

        FileConfigRetval storeConfiguration()
        {
            std::vector<Register> storedData(registersToStore);

            if (const auto retVal = saveConfigurationFile(storedData); retVal != FileConfigRetval::OK) {
                LOG_ERROR("Save configuration file error: %s", magic_enum::enum_name(retVal).data());
                return retVal;
            }
            if (const auto retVal = verifySavedConfigurationFile(storedData); retVal != FileConfigRetval::OK) {
                LOG_ERROR("Verify configuration file error: %s", magic_enum::enum_name(retVal).data());
                return retVal;
            }

            const auto soc = getBatteryLevelFromRegisterValue(storedData[static_cast<Register>(Registers::RepSOC_REG)]);
            LOG_INFO("Saved fuel gauge configuration (RawSoC: %d)", soc);

            return FileConfigRetval::OK;
        }

        FileConfigRetval loadConfiguration()
        {
            // First, we load the default battery configuration
            if (configureFuelGaugeBatteryModel() == BatteryRetval::OK) {
                resetFuelGaugeModel();
            }
            else {
                LOG_ERROR("Failed to configure fuel gauge battery model");
                return FileConfigRetval::ConfigFuelGaugeModelError;
            }

            const auto batteryLevelAfterDefaultConfiguration = getBatteryLevel();
            if (batteryLevelAfterDefaultConfiguration.has_value()) {
                rawSoc.update(batteryLevelAfterDefaultConfiguration.value());
            }
            std::vector<Register> readData(registersToStore);

            // then we read the battery configuration from the saved file
            if (const auto retVal = readConfigurationFile(readData); retVal != FileConfigRetval::OK) {
                // if there is a problem with reading the data, leave the default configuration and exit
                LOG_ERROR("Read configuration file error: %s", magic_enum::enum_name(retVal).data());
                storeConfiguration();
                return FileConfigRetval::OK;
            }

            const auto savedBatteryLevel =
                getBatteryLevelFromRegisterValue(readData[static_cast<Register>(Registers::RepSOC_REG)]);

            // if the difference in SOC readings is less than 'maxSocPercentageError' then load the configuration read
            // from the file
            if (batteryLevelAfterDefaultConfiguration.has_value() &&
                !isSocDataDiscrepancy(batteryLevelAfterDefaultConfiguration.value(), savedBatteryLevel)) {
                if (const auto retVal = storeFuelGaugeRegisters(readData); retVal != FileConfigRetval::OK) {
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

            return FileConfigRetval::OK;
        }

        BatteryRetval setTemperatureThresholds(std::uint8_t minTemperatureDegrees, std::uint8_t maxTemperatureDegrees)
        {
            const std::uint16_t regVal =
                (static_cast<std::uint16_t>(maxTemperatureDegrees) << 8) | minTemperatureDegrees;
            if (fuelGaugeWrite(Registers::TALRT_Th_REG, regVal) != kStatus_Success) {
                LOG_ERROR("Failed to set temperature thresholds");
                return BatteryRetval::ChargerError;
            }
            return BatteryRetval::OK;
        }

        BatteryRetval setServiceVoltageThresholds(std::uint16_t minVoltage_mV, std::uint16_t maxVoltage_mV)
        {
            const std::uint16_t regVal = ((maxVoltage_mV / 20) << 8) | (minVoltage_mV / 20);

            if (fuelGaugeWrite(Registers::VALRT_Th_REG, regVal) != kStatus_Success) {
                LOG_ERROR("Failed to set service voltage thresholds");
                return BatteryRetval::ChargerError;
            }
            return BatteryRetval::OK;
        }

        BatteryRetval fillConfigRegisterValue()
        {
            const std::uint16_t regVal = static_cast<std::uint16_t>(CONFIG::Aen)      // Enable alerts
                                         | static_cast<std::uint16_t>(CONFIG::Ten)    // Enable temperature conversion
                                         | static_cast<std::uint16_t>(CONFIG::ETHRM); // External thermistor

            if (fuelGaugeWrite(Registers::CONFIG_REG, regVal) != kStatus_Success) {
                LOG_ERROR("Failed to fill CONFIG register");
                return BatteryRetval::ChargerError;
            }
            return BatteryRetval::OK;
        }

        BatteryRetval fillConfig2RegisterValue()
        {
            const std::uint16_t regVal =
                static_cast<std::uint16_t>(CONFIG2::dSOCen) |  // SOC 1% change alert
                static_cast<std::uint16_t>(CONFIG2::TAlrtEn) | // Temperature alerts
                static_cast<std::uint16_t>(CONFIG2::OCVQen);   // Enable automatic empty compensation

            if (fuelGaugeWrite(Registers::CONFIG2_REG, regVal) != kStatus_Success) {
                LOG_ERROR("Failed to fill CONFIG2 register");
                return BatteryRetval::ChargerError;
            }
            return BatteryRetval::OK;
        }

        BatteryRetval configureTemperatureMeasurement()
        {
            if ((fuelGaugeWrite(Registers::TGAIN_REG, temperatureConversionGain) &
                 fuelGaugeWrite(Registers::TOFF_REG, temperatureConversionOffset)) != kStatus_Success) {
                LOG_ERROR("Failed to configure temperature measurement");
                return BatteryRetval::ChargerError;
            }
            return BatteryRetval::OK;
        }

        BatteryRetval enableTopIRQs()
        {
            const std::uint8_t mask = ~(static_cast<std::uint8_t>(TOP_MASK::CHGR_INT_MASK) |
                                        static_cast<std::uint8_t>(TOP_MASK::FG_INT_MASK));

            if (chargerTopControllerWrite(Registers::TOP_CONTROLL_IRQ_MASK_REG, mask) != kStatus_Success) {
                LOG_ERROR("Failed to enable top-level controller IRQs");
                return BatteryRetval::ChargerError;
            }
            return BatteryRetval::OK;
        }

        BatteryRetval enableChargerIRQs()
        {
            const std::uint8_t mask =
                ~(static_cast<std::uint8_t>(CHG_MASK::CHG_M) | static_cast<std::uint8_t>(CHG_MASK::CHGIN_M));

            if (chargerWrite(Registers::CHG_INT_MASK, mask) != kStatus_Success) {
                LOG_ERROR("Failed to enable charger IRQs");
                return BatteryRetval::ChargerError;
            }
            return BatteryRetval::OK;
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

            gpio->EnableInterrupt(1 << static_cast<std::uint32_t>(BoardDefinitions::BATTERY_CHARGER_INTB_PIN));
        }

        std::optional<int> getCellTemperature()
        {
            const auto [status, value] = fuelGaugeRead(Registers::TEMP_REG);
            if (status != kStatus_Success) {
                LOG_ERROR("Failed to read cell temperature");
                return std::nullopt;
            }
            // Round to integer by stripping fractions
            const std::uint8_t temperatureInt = value >> 8;
            return utils::twosComplimentToInt(temperatureInt);
        }

        void onChargingFinished()
        {
            LOG_DEBUG("Charging finished");
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
                LOG_DEBUG("Cell temperature too low, charging disabled");
                disableCharging();
                break;
            case TemperatureRanges::Hot:
                LOG_DEBUG("Cell temperature too high, charging disabled");
                disableCharging();
                break;
            default:
                LOG_DEBUG("Unhandled battery temperature range");
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

        // Check power-on reset bit
        const auto [status, registerValue] = fuelGaugeRead(Registers::STATUS_REG);
        if (status != kStatus_Success) {
            LOG_INFO("Failed to check POR bit!");
        }

        if (static_cast<bool>(registerValue & static_cast<std::uint16_t>(STATUS::POR))) {
            LOG_INFO("Initializing battery fuel gauge model.");
            loadConfiguration();
        }

        setServiceVoltageThresholds(minVoltagemV, maxVoltagemV);

        fillConfigRegisterValue();
        fillConfig2RegisterValue();
        configureTemperatureMeasurement();

        checkTemperatureRange();

        // Short time to synchronize after configuration
        vTaskDelay(pdMS_TO_TICKS(100));

        clearAllChargerIRQs();
        clearFuelGaugeIRQ(static_cast<std::uint16_t>(BatteryINTBSource::all));
        enableTopIRQs();
        enableChargerIRQs();
        IRQPinsInit();

        return kStatus_Success;
    }

    void deinit()
    {
        resetUSBCurrentLimit();
        storeConfiguration();

        gpio->DisableInterrupt(1 << static_cast<std::uint32_t>(BoardDefinitions::BATTERY_CHARGER_INTB_PIN));

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

        units::SOC percentLevelDelta;

        const auto status = getChargeStatus();
        switch (status) {
        case BatteryRetval::ChargerCharging:
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
        const auto readout = fuelGaugeRead(Registers::RepSOC_REG);
        if (readout.first != kStatus_Success) {
            LOG_ERROR("Failed to get battery level");
            return std::nullopt;
        }

        auto soc = getBatteryLevelFromRegisterValue(readout.second);
        if (soc > fullyChargedSOC) {
            // Sometimes MAX77818 can return SOC > 100% when config file is missing
            // or SOC is based on voltage, so we need to truncate value.
            LOG_WARN("Raw SOC value has been truncated (value read: %d)", soc);
            soc = fullyChargedSOC;
        }
        return soc;
    }

    TemperatureRanges getTemperatureRange()
    {
        const auto temperature = getCellTemperature();
        if (!temperature.has_value()) {
            return TemperatureRanges::Unknown;
        }

        LOG_DEBUG("Cell temperature: %dC", temperature.value());

        for (const auto &[range, thresholds] : temperatureRanges) {
            if ((temperature > thresholds.lowTemperatureThreshold) &&
                (temperature <= thresholds.highTemperatureThreshold)) {
                return range;
            }
        }

        return TemperatureRanges::Unknown;
    }

    BatteryRetval getChargeStatus()
    {
        BatteryRetval retVal{};

        // Read register to clear pending interrupts
        const auto IRQSource = chargerRead(Registers::CHG_INT_REG);
        if (IRQSource.first != kStatus_Success) {
            LOG_ERROR("Failed to read charger INT source");
        }

        const auto chargerStatus = chargerRead(Registers::CHG_INT_OK);
        if (chargerStatus.first != kStatus_Success) {
            LOG_ERROR("Failed to read charger status");
            return BatteryRetval::ChargerError;
        }

        const auto chargerDetails = getChargerDetails();
        if (!chargerDetails.has_value()) {
            return BatteryRetval::ChargerError;
        }

        const auto chargingSetup = chargerRead(Registers::CHG_CNFG_00);
        if (chargingSetup.first != kStatus_Success) {
            LOG_ERROR("Failed to read charging setup");
            return BatteryRetval::ChargerError;
        }

        if (isChargerInputValid(chargerStatus.second)) {
            retVal = BatteryRetval::ChargerCharging;
        }
        else {
            retVal = BatteryRetval::ChargerNotCharging;
        }

        switch (static_cast<CHG_DETAILS_01>(chargerDetails.value())) {
        case CHG_DETAILS_01::CHARGER_DONE:
            retVal = BatteryRetval::ChargingDone;
            onChargingFinished();
            break;

        case CHG_DETAILS_01::CHARGER_OFF:
            if (chargingSetup.second != static_cast<std::uint8_t>(ChargerMode::ChargerOnBuckOn)) {
                if (isChargerInputValid(chargerStatus.second)) {
                    retVal = BatteryRetval::ChargerPluggedNotCharging;
                }
                else {
                    retVal = BatteryRetval::ChargerNotCharging;
                }
            }
            break;

        case CHG_DETAILS_01::CHARGER_OFF_OVERHEATED:
            retVal = BatteryRetval::ChargerNotCharging;
            LOG_WARN("Not charging due to exceeded junction temperature");
            break;

        case CHG_DETAILS_01::CHARGER_OFF_WATCHDOG_EXPIRED:
            retVal = BatteryRetval::ChargerNotCharging;
            LOG_WARN("Not charging due to expired watchdog timer");
            break;

        case CHG_DETAILS_01::CHARGER_PREQUALIFICATION:
        case CHG_DETAILS_01::CHARGER_CC:
        case CHG_DETAILS_01::CHARGER_CV:
        case CHG_DETAILS_01::CHARGER_TOPOFF:
            retVal = BatteryRetval::ChargerCharging;
            break;

        case CHG_DETAILS_01::CHARGER_TIMER_FAULT:
        case CHG_DETAILS_01::CHARGER_BATTERY_DETECT:
            retVal = BatteryRetval::ChargerError;
            break;

        default:
            LOG_ERROR("Unhandled charger details value: 0x%02X", chargerDetails.value());
            break;
        }

        return retVal;
    }

    Register getStatusRegister()
    {
        const auto [status, value] = fuelGaugeRead(Registers::STATUS_REG);
        if (status != kStatus_Success) {
            LOG_ERROR("Failed to read STATUS register");
        }
        return value;
    }

    void clearAllChargerIRQs()
    {
        const auto [status, value] = chargerRead(Registers::CHG_INT_REG);
        if (status != kStatus_Success) {
            LOG_ERROR("Failed to read charger IRQs register");
        }

        if (value != 0) {
            // Write zero to clear IRQ sources
            if (chargerWrite(Registers::CHG_INT_REG, 0) != kStatus_Success) {
                LOG_ERROR("Failed to clear charger IRQs");
            }
        }
    }

    void clearFuelGaugeIRQ(std::uint16_t intToClear)
    {
        const auto readout          = getStatusRegister();
        const std::uint16_t toWrite = readout & (~intToClear);
        if (fuelGaugeWrite(Registers::STATUS_REG, toWrite) != kStatus_Success) {
            LOG_ERROR("Failed to clear fuel gauge IRQ");
        }
    }

    void checkTemperatureRange()
    {
        processTemperatureRange(getTemperatureRange());
    }

    std::optional<std::uint8_t> getTopControllerINTSource()
    {
        const auto [status, value] = chargerTopControllerRead(Registers::TOP_CONTROLL_IRQ_SRC_REG);
        if (status != kStatus_Success) {
            LOG_ERROR("Failed to read interrupt source");
            return std::nullopt;
        }
        return value;
    }

    void setMaxBusCurrent(USBCurrentLimit limit)
    {
        const auto value = static_cast<std::uint8_t>(limit);
        if (chargerWrite(Registers::CHG_CNFG_09, value) != kStatus_Success) {
            LOG_ERROR("Failed to set maximum USB current");
        }
    }

    std::optional<int> getVoltageFilteredMeasurement()
    {
        const auto [status, value] = fuelGaugeRead(Registers::AvgVCELL_REG);
        if (status != kStatus_Success) {
            LOG_ERROR("Failed to get filtered voltage measurement");
            return std::nullopt;
        }
        return static_cast<int>(value * voltageSenseGain);
    }

    std::optional<int> getAvgCurrent()
    {
        const auto [status, value] = fuelGaugeRead(Registers::AvgCurrent_REG);
        if (status != kStatus_Success) {
            LOG_ERROR("Failed to get average current measurement");
            return std::nullopt;
        }
        return static_cast<int>(utils::twosComplimentToInt(value) * currentSenseGain);
    }

    std::optional<int> getCurrentMeasurement()
    {
        const auto [status, value] = fuelGaugeRead(Registers::Current_REG);
        if (status != kStatus_Success) {
            LOG_ERROR("Failed to get current measurement");
            return std::nullopt;
        }
        return static_cast<int>(utils::twosComplimentToInt(value) * currentSenseGain);
    }

    std::optional<MaxMinVolt> getMaxMinVolt()
    {
        const auto [status, value] = fuelGaugeRead(Registers::MaxMinVolt_REG);
        if (status != kStatus_Success) {
            LOG_ERROR("Failed to get MaxMin voltage measurement");
            return std::nullopt;
        }

        MaxMinVolt ret;
        ret.maxMilliVolt = ((value & 0xFF00) >> 8) * maxMinMilliVoltGain;
        ret.minMilliVolt = (value & 0xFF) * maxMinMilliVoltGain;
        return ret;
    }

    void printFuelGaugeInfo()
    {
        const auto maxMinVoltage = getMaxMinVolt().value_or(MaxMinVolt{0, 0});

        LOG_INFO("Fuel Gauge info:");
        LOG_INFO("\tAvgVCell: %dmV", getVoltageFilteredMeasurement().value_or(0));
        LOG_INFO("\tMaxVolt: %dmV", maxMinVoltage.maxMilliVolt);
        LOG_INFO("\tMinVolt: %dmV", maxMinVoltage.minMilliVolt);
        LOG_INFO("\tAvgCurrent: %dmA", getAvgCurrent().value_or(0));
        LOG_INFO("\tRawSoC: %d%%", getBatteryLevel().value_or(0));
    }

    bool isChargerPlugged()
    {
        const auto chargerStatus = chargerRead(Registers::CHG_INT_OK);
        if (chargerStatus.first != kStatus_Success) {
            LOG_ERROR("Failed to read charger status");
            return false;
        }
        return isChargerInputValid(chargerStatus.second);
    }
} // namespace bsp::battery_charger
