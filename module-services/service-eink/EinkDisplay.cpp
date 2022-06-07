// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "EinkDisplay.hpp"

#if defined(TARGET_RT1051)
#include <board/BoardDefinitions.hpp>
#endif

#include <log/log.hpp>
#include <gui/core/Color.hpp>
#include <gsl/util>
#include <cstdio>
#include <cstring>

namespace service::eink
{
    namespace
    {
        constexpr auto DefaultSurroundingTemperature = -1000;
        constexpr auto LutsFileName                  = "Luts.bin";
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
    } // namespace

    EinkDisplay::EinkDisplay(::gui::Size screenSize)
        : size{screenSize}, currentWaveform{createDefaultWaveFormSettings(EinkWaveformGC16)},
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

    EinkStatus_e EinkDisplay::resetAndInit()
    {
        return EinkResetAndInitialize();
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
        if (eInkSentinel) {
            eInkSentinel->HoldMinimumFrequency();
        }
        EinkPowerOn();
    }

    void EinkDisplay::powerOff()
    {
        EinkPowerOff();
        if (driverLPSPI) {
            driverLPSPI->Disable();
        }
        if (eInkSentinel) {
            eInkSentinel->ReleaseMinimumFrequency();
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

    EinkStatus_e EinkDisplay::update(std::uint8_t *displayBuffer)
    {
        return EinkUpdateFrame(pointTopLeft.x,
                               pointTopLeft.y,
                               size.width,
                               size.height,
                               displayBuffer,
                               getCurrentBitsPerPixelFormat(),
                               displayMode);
    }

    EinkStatus_e EinkDisplay::update(int32_t x, int32_t y, uint32_t w, uint32_t h, std::uint8_t *displayBuffer)
    {
        return EinkUpdateFrame(x, y, w, h, displayBuffer, getCurrentBitsPerPixelFormat(), displayMode);
    }

    EinkBpp_e EinkDisplay::getCurrentBitsPerPixelFormat() const noexcept
    {
        if ((currentWaveform.mode == EinkWaveformA2) || (currentWaveform.mode == EinkWaveformDU2)) {
            return Eink4Bpp; // this should be 1Bpp, but the OS is not ready for this (in 1Bpp → halftones disappear)
        }
        return Eink4Bpp;
    }

    EinkStatus_e EinkDisplay::refresh(EinkDisplayTimingsMode_e refreshMode)
    {
        currentWaveform.useCounter += 1;
        return EinkRefreshImage(pointTopLeft.x, pointTopLeft.y, size.width, size.height, refreshMode);
    }

    bool EinkDisplay::isNewWaveformNeeded(EinkWaveforms_e newMode, std::int32_t newTemperature) const
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

    EinkStatus_e EinkDisplay::setWaveform(EinkWaveforms_e mode, std::int32_t temperature)
    {
        if (!isNewWaveformNeeded(mode, temperature)) {
            return EinkOK;
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
            return EinkOK;
        }

        auto file = std::fopen(LutsFileName, "rb");
        if (file == nullptr) {
            LOG_FATAL("Could not find the LUTS.bin file. Returning");
            return EinkWaveformsFileOpenFail;
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
        return EinkOK;
    }

    unsigned int EinkDisplay::toWaveformTemperatureOffset(std::int32_t temperature) noexcept
    {
        if (temperature >= LUTTemperatureCritical)
            return LUTTemperatureOffsetCritical;
        if (temperature >= LUTTemperatureSubcritical)
            return LUTTemperatureOffsetSubcritical;
        if (temperature < LUTTemperatureMinimal) {
            temperature = LUTTemperatureMinimal;
        }
        return temperature / LUTTemperatureOffsetInterval;
    }

    unsigned int EinkDisplay::toWaveformOffset(unsigned short LUTbank, unsigned int temperatureOffset) noexcept
    {
        constexpr auto singleLUTOffset = (LUTTemperatureOffsetCritical + 1);
        return LUTSTotalSize * (singleLUTOffset * LUTbank + temperatureOffset);
    }

    unsigned int EinkDisplay::toWaveformOffset(EinkWaveforms_e mode, unsigned int temperatureOffset)
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

    void EinkDisplay::setMode(EinkDisplayColorMode_e mode) noexcept
    {
        displayMode = mode;
    }

    std::int32_t EinkDisplay::getLastTemperature() const noexcept
    {
        return currentWaveform.temperature;
    }

    ::gui::Size EinkDisplay::getSize() const noexcept
    {
        return size;
    }

    [[nodiscard]] auto EinkDisplay::getDevice() const noexcept -> std::shared_ptr<devices::Device>
    {
        return driverLPSPI;
    }

    void EinkDisplay::setEinkSentinel(std::shared_ptr<EinkSentinel> sentinel)
    {
        eInkSentinel = std::move(sentinel);
    }

} // namespace service::eink
