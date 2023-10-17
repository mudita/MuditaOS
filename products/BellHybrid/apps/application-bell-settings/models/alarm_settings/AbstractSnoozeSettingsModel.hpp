// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <common/models/AbstractSettingsModel.hpp>

#include <cstdint>
#include <utf8/UTF8.hpp>

namespace app::bell_settings
{
    class AbstractSnoozeSettingsModel
    {
      public:
        AbstractSnoozeSettingsModel(std::unique_ptr<gui::AbstractSettingsModel<bool>> snoozeOnOff,
                                    std::unique_ptr<gui::AbstractSettingsModel<std::uint8_t>> snoozeLength,
                                    std::unique_ptr<gui::AbstractSettingsModel<std::uint8_t>> snoozeChimeInterval,
                                    std::unique_ptr<gui::AbstractSettingsModel<UTF8>> snoozeChimeTone,
                                    std::unique_ptr<gui::AbstractSettingsModel<std::uint8_t>> snoozeChimeVolume)
            : snoozeOnOff(std::move(snoozeOnOff)), snoozeLength(std::move(snoozeLength)),
              snoozeChimeInterval(std::move(snoozeChimeInterval)), snoozeChimeTone(std::move(snoozeChimeTone)),
              snoozeChimeVolume(std::move(snoozeChimeVolume))
        {}

        virtual ~AbstractSnoozeSettingsModel() = default;

        gui::AbstractSettingsModel<bool> &getSnoozeOnOff()
        {
            return *snoozeOnOff;
        }
        gui::AbstractSettingsModel<std::uint8_t> &getSnoozeLength()
        {
            return *snoozeLength;
        }
        gui::AbstractSettingsModel<std::uint8_t> &getSnoozeChimeInterval()
        {
            return *snoozeChimeInterval;
        }
        gui::AbstractSettingsModel<UTF8> &getSnoozeChimeTone()
        {
            return *snoozeChimeTone;
        }
        gui::AbstractSettingsModel<std::uint8_t> &getSnoozeChimeVolume()
        {
            return *snoozeChimeVolume;
        }

      private:
        std::unique_ptr<gui::AbstractSettingsModel<bool>> snoozeOnOff;
        std::unique_ptr<gui::AbstractSettingsModel<std::uint8_t>> snoozeLength;
        std::unique_ptr<gui::AbstractSettingsModel<std::uint8_t>> snoozeChimeInterval;
        std::unique_ptr<gui::AbstractSettingsModel<UTF8>> snoozeChimeTone;
        std::unique_ptr<gui::AbstractSettingsModel<std::uint8_t>> snoozeChimeVolume;
    };
} // namespace app::bell_settings
