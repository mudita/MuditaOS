// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include <common/models/AbstractSettingsModel.hpp>
#include <utf8/UTF8.hpp>
#include <cstdint>
#include <memory>
#include <string>

namespace app::bell_settings
{
    class AbstractPrewakeUpSettingsModel
    {
      public:
        AbstractPrewakeUpSettingsModel(std::unique_ptr<gui::AbstractSettingsModel<std::uint8_t>> chimeDuration,
                                       std::unique_ptr<gui::AbstractSettingsModel<UTF8>> chimeTone,
                                       std::unique_ptr<gui::AbstractSettingsModel<std::uint8_t>> chimeVolume,
                                       std::unique_ptr<gui::AbstractSettingsModel<std::uint8_t>> lightDuration,
                                       std::unique_ptr<gui::AbstractSettingsModel<std::uint8_t>> frontlight)
            : chimeDuration(std::move(chimeDuration)), chimeTone(std::move(chimeTone)),
              chimeVolume(std::move(chimeVolume)), lightDuration(std::move(lightDuration)),
              frontlight(std::move(frontlight))
        {}

        virtual ~AbstractPrewakeUpSettingsModel() = default;

        gui::AbstractSettingsModel<std::uint8_t> &getChimeDuration()
        {
            return *chimeDuration;
        }

        gui::AbstractSettingsModel<UTF8> &getChimeTone()
        {
            return *chimeTone;
        }

        gui::AbstractSettingsModel<std::uint8_t> &getChimeVolume()
        {
            return *chimeVolume;
        }

        gui::AbstractSettingsModel<std::uint8_t> &getLightDuration()
        {
            return *lightDuration;
        }

        gui::AbstractSettingsModel<std::uint8_t> &getBrightness()
        {
            return *frontlight;
        }

      private:
        std::unique_ptr<gui::AbstractSettingsModel<std::uint8_t>> chimeDuration;
        std::unique_ptr<gui::AbstractSettingsModel<UTF8>> chimeTone;
        std::unique_ptr<gui::AbstractSettingsModel<std::uint8_t>> chimeVolume;
        std::unique_ptr<gui::AbstractSettingsModel<std::uint8_t>> lightDuration;
        std::unique_ptr<gui::AbstractSettingsModel<std::uint8_t>> frontlight;
    };
} // namespace app::bell_settings
