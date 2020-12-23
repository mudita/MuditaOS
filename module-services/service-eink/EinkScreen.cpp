// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "EinkScreen.hpp"

#include <gui/core/Color.hpp>
#include <gsl/gsl_util>

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
        constexpr auto LUTRSize                      = 256; ///< Needed due to \ref EINK_LUTS_FILE_PATH structure
        constexpr auto LUTSTotalSize                 = LUTDSize + LUTCSize + LUTRSize;

        EinkWaveFormSettings_t createDefaultWaveFormSettings(EinkWaveforms_e waveformMode)
        {
            EinkWaveFormSettings_t settings{};
            settings.mode        = waveformMode;
            settings.temperature = DefaultSurroundingTemperature;
            settings.LUTCData    = nullptr;
            settings.LUTCSize    = 0;
            settings.LUTDData    = nullptr;
            settings.LUTDSize    = 0;
            return settings;
        }

        std::unique_ptr<std::uint8_t[]> allocateScreenBuffer(::gui::Size screenSize)
        {
            return std::make_unique<std::uint8_t[]>(screenSize.width * screenSize.height);
        }
    } // namespace

    EinkScreen::EinkScreen(::gui::Size screenSize)
        : size{screenSize}, screenBuffer{allocateScreenBuffer(screenSize)},
          waveformSettings{createDefaultWaveFormSettings(EinkWaveformGC16)},
          displayMode{EinkDisplayColorMode_e::EinkDisplayColorModeStandard}
    {
        setScreenBuffer(gui::Color::White, screenSize.width * screenSize.height);
    }

    EinkScreen::~EinkScreen() noexcept
    {
        delete[] waveformSettings.LUTCData;
        delete[] waveformSettings.LUTDData;
    }

    EinkStatus_e EinkScreen::resetAndInit()
    {
        return EinkResetAndInitialize();
    }

    void EinkScreen::dither()
    {
        EinkDitherDisplay();
    }

    void EinkScreen::powerOn()
    {
        EinkPowerOn();
    }

    void EinkScreen::powerOff()
    {
        EinkPowerOff();
    }

    void EinkScreen::shutdown()
    {
        EinkPowerDown();
    }

    void EinkScreen::setScreenBuffer(const std::uint8_t *buffer, std::uint32_t bufferSize)
    {
        std::memcpy(screenBuffer.get(), buffer, bufferSize);
    }

    void EinkScreen::setScreenBuffer(std::uint8_t value, std::uint32_t bufferSize)
    {
        std::memset(screenBuffer.get(), value, bufferSize);
    }

    EinkStatus_e EinkScreen::update()
    {
        return EinkUpdateFrame(
            pointTopLeft.x, pointTopLeft.y, size.width, size.height, screenBuffer.get(), Eink4Bpp, displayMode);
    }

    EinkStatus_e EinkScreen::refresh(EinkDisplayTimingsMode_e refreshMode)
    {
        return EinkRefreshImage(pointTopLeft.x, pointTopLeft.y, size.width, size.height, refreshMode);
    }

    bool EinkScreen::changeWaveform(EinkWaveforms_e mode, std::int32_t temperature)
    {
        if (temperature == waveformSettings.temperature && mode == waveformSettings.mode) {
            return EinkOK;
        }
        waveformSettings.temperature = temperature;
        waveformSettings.mode        = mode;

        const auto segment = calculateWaveFormSegment(temperature);
        auto offset        = calculateWaveFormOffset(mode, segment);

        auto file = std::fopen(LutsFileName, "rb");
        if (file == nullptr) {
            LOG_FATAL("Could not find the LUTS.bin file. Returning");
            return false;
        }
        auto fileHandlerCleanup = gsl::finally([&file]() { std::fclose(file); });

        resetWaveFormSettings();
        std::fseek(file, offset, SEEK_SET);
        std::fread(&waveformSettings.LUTDData[1], 1, LUTDSize, file);

        // 0x00 - 1 frame, ... , 0x0F - 16 frames
        const uint8_t frameCount = waveformSettings.LUTDData[1] + 1;
        // (frameCount * 64) - size of actual LUT; (+1) - the byte containing frameCount; (+1) - EinkLUTD command
        waveformSettings.LUTDSize = (frameCount * 64) + 1 + 1;

        offset += LUTDSize;
        std::fseek(file, offset, SEEK_SET);
        std::fread(&waveformSettings.LUTCData[1], 1, LUTCSize, file);

        EinkUpdateWaveform(&waveformSettings);
        return true;
    }

    unsigned int EinkScreen::calculateWaveFormSegment(std::int32_t temperature) const
    {
        if (temperature < 38) {
            return temperature / 3;
        }
        if (temperature < 43) {
            return 12;
        }
        return 13;
    }

    unsigned int EinkScreen::calculateWaveFormOffset(EinkWaveforms_e mode, unsigned int segment) const
    {
        switch (mode) {
        case EinkWaveformINIT:
            return LUTSTotalSize * segment;
        case EinkWaveformA2:
            return LUTSTotalSize * (14 + segment);
        case EinkWaveformDU2:
            return LUTSTotalSize * (28 + segment);
        case EinkWaveformGLD16:
            return LUTSTotalSize * (42 + segment);
        case EinkWaveformGC16:
            return LUTSTotalSize * (56 + segment);
        }
        throw std::invalid_argument{"Invalid waveform mode."};
    }

    void EinkScreen::resetWaveFormSettings()
    {
        delete[] waveformSettings.LUTDData;
        waveformSettings.LUTDSize    = 0;
        waveformSettings.LUTDData    = new uint8_t[LUTDSize + 1];
        waveformSettings.LUTDData[0] = EinkLUTD;

        delete[] waveformSettings.LUTCData;
        waveformSettings.LUTCSize    = LUTCSize;
        waveformSettings.LUTCData    = new uint8_t[LUTCSize + 1];
        waveformSettings.LUTCData[0] = EinkLUTC;
    }

    void EinkScreen::setDisplayMode(EinkDisplayColorMode_e mode) noexcept
    {
        displayMode = mode;
    }

    ::gui::Size EinkScreen::getDisplaySize() const noexcept
    {
        return size;
    }
} // namespace service::eink
