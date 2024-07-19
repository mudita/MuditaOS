// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "EinkDisplay.hpp"

#include <purefs/filesystem_paths.hpp>
#include <board/BoardDefinitions.hpp>
#include <log/log.hpp>

#include <gsl/util>

#include <stdexcept>
#include <cstdio>
#include <cstring>

namespace hal::eink
{
    namespace
    {
        constexpr auto defaultSurroundingTemperature = -1000;
        constexpr auto LUTDSize                      = 16385;
        constexpr auto LUTCSize                      = 64;
        constexpr auto LUTRSize                      = 256; ///< Needed due to \ref LutsFileName structure
        constexpr auto LUTSTotalSize                 = LUTDSize + LUTCSize + LUTRSize;

        constexpr auto LUTTemperatureMinimal     = 0;
        constexpr auto LUTTemperatureSubcritical = 38;
        constexpr auto LUTTemperatureCritical    = 43;

        constexpr auto LUTTemperatureOffsetInterval    = 3;
        constexpr auto LUTTemperatureOffsetSubcritical = 12;
        constexpr auto LUTTemperatureOffsetCritical    = 13;
        constexpr auto einkDisplayMaxInitRetries       = 10U;

        const auto lutsFilePath = purefs::dir::getAssetsDirPath() / "luts.bin";

        constexpr auto createDefaultWaveformSettings(bsp::eink::EinkWaveform waveformMode)
            -> bsp::eink::EinkWaveformSettings
        {
            bsp::eink::EinkWaveformSettings settings{};
            settings.mode        = waveformMode;
            settings.temperature = defaultSurroundingTemperature;
            settings.useCounter  = 0;
            settings.LUTCData    = nullptr;
            settings.LUTCSize    = 0;
            settings.LUTDData    = nullptr;
            settings.LUTDSize    = 0;
            return settings;
        }

        constexpr auto toWaveformTemperatureOffset(std::int32_t temperature) noexcept -> unsigned
        {
            if (temperature >= LUTTemperatureCritical) {
                return LUTTemperatureOffsetCritical;
            }
            if (temperature >= LUTTemperatureSubcritical) {
                return LUTTemperatureOffsetSubcritical;
            }
            if (temperature < LUTTemperatureMinimal) {
                temperature = LUTTemperatureMinimal;
            }
            return temperature / LUTTemperatureOffsetInterval;
        }

        constexpr auto toWaveformOffset(unsigned short LUTbank, unsigned temperatureOffset) noexcept -> unsigned
        {
            constexpr auto singleLUTOffset = (LUTTemperatureOffsetCritical + 1);
            return LUTSTotalSize * (singleLUTOffset * LUTbank + temperatureOffset);
        }

        constexpr auto toWaveformOffset(bsp::eink::EinkWaveform mode, unsigned temperatureOffset) -> unsigned
        {
            switch (mode) {
            case bsp::eink::EinkWaveform::INIT:
                return toWaveformOffset(0, temperatureOffset);
            case bsp::eink::EinkWaveform::A2:
                return toWaveformOffset(1, temperatureOffset);
            case bsp::eink::EinkWaveform::DU2:
                return toWaveformOffset(2, temperatureOffset);
            case bsp::eink::EinkWaveform::GLD16:
                return toWaveformOffset(3, temperatureOffset);
            case bsp::eink::EinkWaveform::GC16:
                return toWaveformOffset(4, temperatureOffset);
            default:
                throw std::invalid_argument{"Invalid waveform mode."};
            }
        }

        constexpr auto translateDisplayColorMode(EinkDisplayColorMode mode) -> bsp::eink::EinkDisplayColorMode
        {
            return mode == EinkDisplayColorMode::EinkDisplayColorModeStandard
                       ? bsp::eink::EinkDisplayColorMode::Standard
                       : bsp::eink::EinkDisplayColorMode::Inverted;
        }

        constexpr auto translateStatus(bsp::eink::EinkStatus status) -> EinkStatus
        {
            switch (status) {
            case bsp::eink::EinkStatus::OK:
                return EinkStatus::EinkOK;
            case bsp::eink::EinkStatus::Error:
                return EinkStatus::EinkError;
            case bsp::eink::EinkStatus::SPIErr:
                return EinkStatus::EinkSPIErr;
            case bsp::eink::EinkStatus::SPINotInitializedErr:
                return EinkStatus::EinkSPINotInitializedErr;
            case bsp::eink::EinkStatus::DMAErr:
                return EinkStatus::EinkDMAErr;
            case bsp::eink::EinkStatus::InitErr:
                return EinkStatus::EinkInitErr;
            case bsp::eink::EinkStatus::Timeout:
                return EinkStatus::EinkTimeout;
            case bsp::eink::EinkStatus::NoMem:
                return EinkStatus::EinkNoMem;
            case bsp::eink::EinkStatus::WaveformsFileOpenFail:
                return EinkStatus::EinkWaveformsFileOpenFail;
            default:
                return EinkStatus::EinkUnknown;
            }
        }
    } // namespace

    EinkDisplay::EinkDisplay(FrameSize size)
        : size{size}, currentWaveform{createDefaultWaveformSettings(bsp::eink::EinkWaveform::GC16)},
          displayMode{EinkDisplayColorMode::EinkDisplayColorModeStandard}
    {
#if defined(TARGET_RT1051)
        driverLPSPI = drivers::DriverLPSPI::Create(
            "EInk", static_cast<drivers::LPSPIInstances>(BoardDefinitions::EINK_LPSPI_INSTANCE));
#endif
    }

    EinkDisplay::~EinkDisplay() noexcept
    {
        delete[] currentWaveform.LUTCData;
        delete[] currentWaveform.LUTDData;
    }

    auto EinkDisplay::prepareDisplay(EinkRefreshMode refreshMode, WaveformTemperature behaviour) -> EinkStatus
    {
        if (const auto status = reinitAndPowerOn(); status != EinkStatus::EinkOK) {
            return status;
        }

        const auto temperature = (behaviour == WaveformTemperature::KeepCurrent) ? getLastTemperature()
                                                                                 : bsp::eink::getTemperatureInternal();

        if (refreshMode == EinkRefreshMode::REFRESH_DEEP) {
            auto status = setWaveform(bsp::eink::EinkWaveform::GC16, temperature);
            if (status == EinkStatus::EinkOK) {
                if (const auto ditherStatus = dither(); ditherStatus != EinkStatus::EinkOK) {
                    return ditherStatus;
                }
            }
            return status;
        }
        return setWaveform(bsp::eink::EinkWaveform::DU2, temperature);
    }

    auto EinkDisplay::updateDisplay(EinkFrame frame, const std::uint8_t *frameBuffer) -> EinkStatus
    {
        return translateStatus(
            updateFrame(bsp::eink::EinkFrame{frame.pos_x, frame.pos_y, frame.size.width, frame.size.height},
                        frameBuffer,
                        getCurrentBitsPerPixelFormat(),
                        translateDisplayColorMode(displayMode)));
    }

    auto EinkDisplay::refreshDisplay(EinkFrame frame) -> EinkStatus
    {
        currentWaveform.useCounter++;
        return translateStatus(bsp::eink::refreshImage(
            bsp::eink::EinkFrame{frame.pos_x, frame.pos_y, frame.size.width, frame.size.height}));
    }

    auto EinkDisplay::showImageUpdate(const std::vector<EinkFrame> &updateFrames, const std::uint8_t *frameBuffer)
        -> EinkStatus
    {
        if (const auto status = reinitAndPowerOn(); status != EinkStatus::EinkOK) {
            return status;
        }

        for (const EinkFrame &frame : updateFrames) {
            const std::uint8_t *buffer = frameBuffer + frame.pos_y * frame.size.width;
            if (const auto status = updateDisplay(frame, buffer); status != EinkStatus::EinkOK) {
                return status;
            }
        }

        return EinkStatus::EinkOK;
    }

    auto EinkDisplay::showImageRefresh(const EinkFrame &refreshFrame, const EinkRefreshMode refreshMode) -> EinkStatus
    {
        if (const auto status = prepareDisplay(refreshMode, WaveformTemperature::KeepCurrent);
            status != EinkStatus::EinkOK) {
            return status;
        }

        if (const auto status = refreshDisplay(refreshFrame); status != EinkStatus::EinkOK) {
            return status;
        }

        return EinkStatus::EinkOK;
    }

    auto EinkDisplay::showImage(const std::vector<EinkFrame> &updateFrames,
                                const EinkFrame &refreshFrame,
                                const std::uint8_t *frameBuffer,
                                const EinkRefreshMode refreshMode) -> EinkStatus
    {
        if (const auto status = prepareDisplay(refreshMode, WaveformTemperature::KeepCurrent);
            status != EinkStatus::EinkOK) {
            return status;
        }

        for (const EinkFrame &frame : updateFrames) {
            const std::uint8_t *buffer = frameBuffer + frame.pos_y * frame.size.width;
            if (const auto status = updateDisplay(frame, buffer); status != EinkStatus::EinkOK) {
                return status;
            }
        }

        if (const auto status = refreshDisplay(refreshFrame); status != EinkStatus::EinkOK) {
            return status;
        }

        return EinkStatus::EinkOK;
    }

    auto EinkDisplay::prepareEarlyRequest(const EinkRefreshMode refreshMode, const WaveformTemperature behaviour)
        -> void
    {
        prepareDisplay(refreshMode, behaviour);
    }

    auto EinkDisplay::resetAndInit() -> EinkStatus
    {
        return translateStatus(bsp::eink::resetAndInitialize());
    }

    auto EinkDisplay::dither() -> EinkStatus
    {
        return translateStatus(bsp::eink::ditherDisplay());
    }

    auto EinkDisplay::powerOn() -> EinkStatus
    {
        if (driverLPSPI) {
            driverLPSPI->Enable();
        }
        return translateStatus(bsp::eink::powerOn());
    }

    auto EinkDisplay::powerOff() -> EinkStatus
    {
        const auto status = translateStatus(bsp::eink::powerOff());
        if (driverLPSPI) {
            driverLPSPI->Disable();
        }
        return status;
    }

    auto EinkDisplay::shutdown() -> EinkStatus
    {
        const auto status = translateStatus(bsp::eink::powerDown());
        if (driverLPSPI) {
            driverLPSPI->Disable();
        }
        return status;
    }

    auto EinkDisplay::wipeOut() -> EinkStatus
    {
        if (const auto status = prepareDisplay(EinkRefreshMode::REFRESH_DEEP, WaveformTemperature::KeepCurrent);
            status != EinkStatus::EinkOK) {
            return status;
        }
        return translateStatus(bsp::eink::fillScreenWithColor(bsp::eink::EinkDisplayColorFilling::White));
    }

    auto EinkDisplay::getCurrentBitsPerPixelFormat() const noexcept -> bsp::eink::EinkBpp
    {
        if ((currentWaveform.mode == bsp::eink::EinkWaveform::A2) ||
            (currentWaveform.mode == bsp::eink::EinkWaveform::DU2)) {
            /* Should be 1Bpp, but the OS is not ready for this and probably never will (in 1Bpp halftones disappear) */
            return bsp::eink::EinkBpp::Eink4Bpp;
        }
        return bsp::eink::EinkBpp::Eink4Bpp;
    }

    auto EinkDisplay::isNewWaveformNeeded(bsp::eink::EinkWaveform newMode, std::int32_t newTemperature) const -> bool
    {
        constexpr auto lenientTemperatureUseCounter = 50; // Arbitrary, not documented
        const auto allowLenientTemperature          = currentWaveform.useCounter < lenientTemperatureUseCounter;

        // At least: modes cannot differ
        if (allowLenientTemperature && newMode == currentWaveform.mode) {
            bool temperatureFine = false;

            switch (currentWaveform.mode) {
            case bsp::eink::EinkWaveform::A2:
            case bsp::eink::EinkWaveform::DU2:
                temperatureFine = abs(newTemperature - currentWaveform.temperature) <= 3;
                break;
            case bsp::eink::EinkWaveform::INIT:
            case bsp::eink::EinkWaveform::GLD16:
            case bsp::eink::EinkWaveform::GC16:
                temperatureFine = abs(newTemperature - currentWaveform.temperature) <= 2;
                break;
            }

            if (temperatureFine) {
                return false;
            }
        }
        return true;
    }

    auto EinkDisplay::setWaveform(bsp::eink::EinkWaveform mode, std::int32_t temperature) -> EinkStatus
    {
        if (!isNewWaveformNeeded(mode, temperature)) {
            updateWaveform(&currentWaveform);
            return EinkStatus::EinkOK;
        }

        const auto currentOffset =
            toWaveformOffset(currentWaveform.mode, toWaveformTemperatureOffset(currentWaveform.temperature));

        // Assume it's changed
        currentWaveform.useCounter  = 0;
        currentWaveform.temperature = temperature;
        currentWaveform.mode        = mode;

        auto offset = static_cast<long>(toWaveformOffset(mode, toWaveformTemperatureOffset(temperature)));

        if (offset == currentOffset) {
            // Current waveform is still the best fit
            return EinkStatus::EinkOK;
        }

        auto file = std::fopen(lutsFilePath.c_str(), "rb");
        if (file == nullptr) {
            LOG_FATAL("Could not find LUTS file in '%s'! Returning...", lutsFilePath.c_str());
            return EinkStatus::EinkWaveformsFileOpenFail;
        }
        auto fileHandlerCleanup = gsl::finally([&file]() { std::fclose(file); });

        resetWaveformSettings();
        std::fseek(file, offset, SEEK_SET);
        std::fread(&currentWaveform.LUTDData[1], 1, LUTDSize, file);

        // 0x00 - 1 frame, ... , 0x0F - 16 frames
        const std::uint8_t waveformFrameCount = currentWaveform.LUTDData[1] + 1;
        // (frameCount * 64) - size of actual LUT; (+1) - the byte containing frameCount; (+1) - EinkLUTD command
        currentWaveform.LUTDSize = (waveformFrameCount * 64) + 1 + 1;

        offset += LUTDSize;
        std::fseek(file, offset, SEEK_SET);
        std::fread(&currentWaveform.LUTCData[1], 1, LUTCSize, file);

        updateWaveform(&currentWaveform);
        return EinkStatus::EinkOK;
    }

    auto EinkDisplay::resetWaveformSettings() -> void
    {
        delete[] currentWaveform.LUTDData;
        currentWaveform.LUTDSize    = 0;
        currentWaveform.LUTDData    = new std::uint8_t[LUTDSize + 1];
        currentWaveform.LUTDData[0] = EinkLUTD;

        delete[] currentWaveform.LUTCData;
        currentWaveform.LUTCSize    = LUTCSize;
        currentWaveform.LUTCData    = new std::uint8_t[LUTCSize + 1];
        currentWaveform.LUTCData[0] = EinkLUTC;
    }

    auto EinkDisplay::setMode(EinkDisplayColorMode mode) noexcept -> void
    {
        displayMode = mode;
    }

    auto EinkDisplay::getMode() const noexcept -> EinkDisplayColorMode
    {
        return displayMode;
    }

    auto EinkDisplay::getLastTemperature() const noexcept -> std::int32_t
    {
        return currentWaveform.temperature;
    }

    auto EinkDisplay::getDevice() const noexcept -> std::shared_ptr<devices::Device>
    {
        return driverLPSPI;
    }

    auto AbstractEinkDisplay::Factory::create(FrameSize size) -> std::unique_ptr<AbstractEinkDisplay>
    {
        return std::make_unique<EinkDisplay>(size);
    }

    auto EinkDisplay::reinitAndPowerOn() -> EinkStatus
    {
        auto errorCounter = 0U;
        auto status       = EinkStatus::EinkUnknown;
        if (bsp::eink::isPoweredOn()) {
            return EinkStatus::EinkOK;
        }
        while (status != EinkStatus::EinkOK && errorCounter++ < einkDisplayMaxInitRetries) {
            status = tryReinitAndPowerOn();
            if (status != EinkStatus::EinkOK) {
                if (errorCounter < einkDisplayMaxInitRetries) {
                    LOG_WARN("Failed to initialize and power on Eink, trying once more...");
                }
                else {
                    LOG_ERROR("Permanently failed to initialize and power on Eink");
                }
            }
        }
        return status;
    }

    auto EinkDisplay::tryReinitAndPowerOn() -> EinkStatus
    {
        if (const auto status = resetAndInit(); status != EinkStatus::EinkOK) {
            LOG_WARN("Eink initialization failed: %s", magic_enum::enum_name(status).data());
            return status;
        }
        if (const auto status = powerOn(); status != EinkStatus::EinkOK) {
            LOG_WARN("Eink power on failed: %s", magic_enum::enum_name(status).data());
            return status;
        }
        return EinkStatus::EinkOK;
    }
} // namespace hal::eink
