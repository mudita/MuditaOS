// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

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
        constexpr auto DefaultSurroundingTemperature = -1000;
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

        const auto LutsFilePath = purefs::dir::getAssetsDirPath() / "luts.bin";

        EinkWaveformSettings_t createDefaultWaveFormSettings(EinkWaveforms_e waveformMode)
        {
            EinkWaveformSettings_t settings{};
            settings.mode        = waveformMode;
            settings.temperature = DefaultSurroundingTemperature;
            settings.useCounter  = 0;
            settings.LUTCData    = nullptr;
            settings.LUTCSize    = 0;
            settings.LUTDData    = nullptr;
            settings.LUTDSize    = 0;
            return settings;
        }

        unsigned int toWaveformTemperatureOffset(std::int32_t temperature) noexcept
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

        unsigned int toWaveformOffset(unsigned short LUTbank, unsigned int temperatureOffset) noexcept
        {
            constexpr auto singleLUTOffset = (LUTTemperatureOffsetCritical + 1);
            return LUTSTotalSize * (singleLUTOffset * LUTbank + temperatureOffset);
        }

        unsigned int toWaveformOffset(EinkWaveforms_e mode, unsigned int temperatureOffset)
        {
            switch (mode) {
            case EinkWaveformINIT:
                return toWaveformOffset(0, temperatureOffset);
            case EinkWaveformA2:
                return toWaveformOffset(1, temperatureOffset);
            case EinkWaveformDU2:
                return toWaveformOffset(2, temperatureOffset);
            case EinkWaveformGLD16:
                return toWaveformOffset(3, temperatureOffset);
            case EinkWaveformGC16:
                return toWaveformOffset(4, temperatureOffset);
            default:
                throw std::invalid_argument{"Invalid waveform mode."};
            }
        }
    } // namespace

    EinkDisplayColorMode_e translateDisplayColorMode(const EinkDisplayColorMode mode)
    {
        return mode == EinkDisplayColorMode::EinkDisplayColorModeStandard
                   ? EinkDisplayColorMode_e::EinkDisplayColorModeStandard
                   : EinkDisplayColorMode_e::EinkDisplayColorModeInverted;
    }

    EinkStatus translateStatus(const EinkStatus_e status_e)
    {
        switch (status_e) {
        case EinkOK:
            return EinkStatus::EinkOK;
        case EinkError:
            return EinkStatus::EinkError;
        case EinkSPIErr:
            return EinkStatus::EinkSPIErr;
        case EinkSPINotInitializedErr:
            return EinkStatus::EinkSPINotInitializedErr;
        case EinkDMAErr:
            return EinkStatus::EinkDMAErr;
        case EinkInitErr:
            return EinkStatus::EinkInitErr;
        case EinkTimeout:
            return EinkStatus::EinkTimeout;
        case EinkNoMem:
            return EinkStatus::EinkNoMem;
        case EinkWaveformsFileOpenFail:
            return EinkStatus::EinkWaveformsFileOpenFail;
        default:
            return EinkStatus::EinkUnknown;
        }
    }

    EinkDisplay::EinkDisplay(FrameSize size)
        : size{size}, currentWaveform{createDefaultWaveFormSettings(EinkWaveformGC16)},
          displayMode{EinkDisplayColorMode_e::EinkDisplayColorModeStandard}
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

    EinkStatus EinkDisplay::prepareDisplay(const EinkRefreshMode refreshMode, const WaveformTemperature behaviour)
    {
        powerOn();

        const auto temperature =
            behaviour == WaveformTemperature::KEEP_CURRENT ? getLastTemperature() : EinkGetTemperatureInternal();

        if (refreshMode == EinkRefreshMode::REFRESH_DEEP) {
            auto status = setWaveform(EinkWaveforms_e::EinkWaveformGC16, temperature);
            if (status == EinkStatus::EinkOK) {
                dither();
            }
            return status;
        }
        return setWaveform(EinkWaveforms_e::EinkWaveformDU2, temperature);
    }

    EinkStatus EinkDisplay::updateDisplay(EinkFrame frame, const std::uint8_t *frameBuffer)
    {
        return translateStatus(
            EinkUpdateFrame(EinkFrame_t{frame.pos_x, frame.pos_y, frame.size.width, frame.size.height},
                            frameBuffer,
                            getCurrentBitsPerPixelFormat(),
                            translateDisplayColorMode(displayMode)));
    }

    EinkStatus EinkDisplay::refreshDisplay(EinkFrame frame, const EinkRefreshMode refreshMode)
    {
        const auto refreshTimingsMode = refreshMode == EinkRefreshMode::REFRESH_DEEP
                                            ? EinkDisplayTimingsDeepCleanMode
                                            : EinkDisplayTimingsFastRefreshMode;

        currentWaveform.useCounter += 1;
        return translateStatus(EinkRefreshImage(
            EinkFrame_t{frame.pos_x, frame.pos_y, frame.size.width, frame.size.height}, refreshTimingsMode));
    }

    EinkStatus EinkDisplay::showImageUpdate(const std::vector<EinkFrame> &updateFrames, const std::uint8_t *frameBuffer)
    {
        powerOn();

        for (const EinkFrame &frame : updateFrames) {
            const std::uint8_t *buffer = frameBuffer + frame.pos_y * frame.size.width;
            if (const auto status = updateDisplay(frame, buffer); status != EinkStatus::EinkOK) {
                return status;
            }
        }

        return EinkStatus::EinkOK;
    }

    EinkStatus EinkDisplay::showImageRefresh(const EinkFrame &refreshFrame, const EinkRefreshMode refreshMode)
    {
        if (const auto status = prepareDisplay(refreshMode, WaveformTemperature::KEEP_CURRENT);
            status != EinkStatus::EinkOK) {
            return status;
        }

        if (const auto status = refreshDisplay(refreshFrame, refreshMode); status != EinkStatus::EinkOK) {
            return status;
        }

        return EinkStatus::EinkOK;
    }

    EinkStatus EinkDisplay::showImage(const std::vector<EinkFrame> &updateFrames,
                                      const EinkFrame &refreshFrame,
                                      const std::uint8_t *frameBuffer,
                                      const EinkRefreshMode refreshMode)
    {
        if (const auto status = prepareDisplay(refreshMode, WaveformTemperature::KEEP_CURRENT);
            status != EinkStatus::EinkOK) {
            return status;
        }

        for (const EinkFrame &frame : updateFrames) {
            const std::uint8_t *buffer = frameBuffer + frame.pos_y * frame.size.width;
            if (const auto status = updateDisplay(frame, buffer); status != EinkStatus::EinkOK) {
                return status;
            }
        }

        if (const auto status = refreshDisplay(refreshFrame, refreshMode); status != EinkStatus::EinkOK) {
            return status;
        }

        return EinkStatus::EinkOK;
    }

    void EinkDisplay::prepareEarlyRequest(const EinkRefreshMode refreshMode, const WaveformTemperature behaviour)
    {
        prepareDisplay(refreshMode, behaviour);
    }

    EinkStatus EinkDisplay::resetAndInit()
    {
        return translateStatus(EinkResetAndInitialize());
    }

    void EinkDisplay::dither()
    {
        EinkDitherDisplay();
    }

    void EinkDisplay::powerOn()
    {
        if (driverLPSPI) {
            driverLPSPI->Enable();
        }
        EinkPowerOn();
    }

    void EinkDisplay::powerOff()
    {
        EinkPowerOff();
        if (driverLPSPI) {
            driverLPSPI->Disable();
        }
    }

    void EinkDisplay::shutdown()
    {
        EinkPowerDown();
    }

    void EinkDisplay::wipeOut()
    {
        EinkFillScreenWithColor(EinkDisplayColorFilling_e::EinkDisplayColorWhite);
    }

    EinkBpp_e EinkDisplay::getCurrentBitsPerPixelFormat() const noexcept
    {
        if ((currentWaveform.mode == EinkWaveformA2) || (currentWaveform.mode == EinkWaveformDU2)) {
            return Eink4Bpp; // this should be 1Bpp, but the OS is not ready for this (in 1Bpp → halftones disappear)
        }
        return Eink4Bpp;
    }

    bool EinkDisplay::isNewWaveformNeeded(const EinkWaveforms_e newMode, const std::int32_t newTemperature) const
    {
        constexpr auto lenientTemperatureUseCounter = 50; // arbitrary. not documented
        auto alloweLenientTemperature               = currentWaveform.useCounter < lenientTemperatureUseCounter;

        // at least: modes cannot differ
        if (alloweLenientTemperature && newMode == currentWaveform.mode) {
            bool temperatureFine = false;

            switch (currentWaveform.mode) {
            case EinkWaveformA2:
                [[fallthrough]];
            case EinkWaveformDU2:
                temperatureFine = abs(newTemperature - currentWaveform.temperature) <= 3;
                break;
            case EinkWaveformINIT:
                [[fallthrough]];
            case EinkWaveformGLD16:
                [[fallthrough]];
            case EinkWaveformGC16:
                temperatureFine = abs(newTemperature - currentWaveform.temperature) <= 2;
                break;
            }

            if (temperatureFine) {
                return false;
            }
        }
        return true;
    }

    EinkStatus EinkDisplay::setWaveform(const EinkWaveforms_e mode, const std::int32_t temperature)
    {
        if (!isNewWaveformNeeded(mode, temperature)) {
            return EinkStatus::EinkOK;
        }

        auto currentOffset =
            toWaveformOffset(currentWaveform.mode, toWaveformTemperatureOffset(currentWaveform.temperature));
        // assume it is changed
        currentWaveform.useCounter  = 0;
        currentWaveform.temperature = temperature;
        currentWaveform.mode        = mode;

        auto offset = toWaveformOffset(mode, toWaveformTemperatureOffset(temperature));

        if (offset == currentOffset) {
            // current waveform is still the best fit
            return EinkStatus::EinkOK;
        }

        auto file = std::fopen(LutsFilePath.c_str(), "rb");
        if (file == nullptr) {
            LOG_FATAL("Could not find LUTS file in '%s'! Returning...", LutsFilePath.c_str());
            return EinkStatus::EinkWaveformsFileOpenFail;
        }
        auto fileHandlerCleanup = gsl::finally([&file]() { std::fclose(file); });

        resetWaveformSettings();
        std::fseek(file, offset, SEEK_SET);
        std::fread(&currentWaveform.LUTDData[1], 1, LUTDSize, file);

        // 0x00 - 1 frame, ... , 0x0F - 16 frames
        const uint8_t waveformFrameCount = currentWaveform.LUTDData[1] + 1;
        // (frameCount * 64) - size of actual LUT; (+1) - the byte containing frameCount; (+1) - EinkLUTD command
        currentWaveform.LUTDSize = (waveformFrameCount * 64) + 1 + 1;

        offset += LUTDSize;
        std::fseek(file, offset, SEEK_SET);
        std::fread(&currentWaveform.LUTCData[1], 1, LUTCSize, file);

        EinkUpdateWaveform(&currentWaveform);
        return EinkStatus::EinkOK;
    }

    void EinkDisplay::resetWaveformSettings()
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

    void EinkDisplay::setMode(const EinkDisplayColorMode mode) noexcept
    {
        displayMode = mode;
    }

    std::int32_t EinkDisplay::getLastTemperature() const noexcept
    {
        return currentWaveform.temperature;
    }

    [[nodiscard]] auto EinkDisplay::getDevice() const noexcept -> std::shared_ptr<devices::Device>
    {
        return driverLPSPI;
    }

    std::unique_ptr<AbstractEinkDisplay> AbstractEinkDisplay::Factory::create(FrameSize size)
    {
        return std::make_unique<EinkDisplay>(size);
    }

} // namespace hal::eink
