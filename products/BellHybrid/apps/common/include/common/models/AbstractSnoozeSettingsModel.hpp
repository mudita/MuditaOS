// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <utf8/UTF8.hpp>
#include <cstdint>

namespace app::bell_settings
{

    class AbstractSnoozeSettingsModel
    {
      public:
        virtual ~AbstractSnoozeSettingsModel() = default;

        virtual void setSnooze(bool value) = 0;
        virtual bool getSnooze() const     = 0;

        virtual void setLength(std::uint8_t value) = 0;
        virtual std::uint8_t getLength() const     = 0;

        virtual void setChimeInterval(std::uint8_t value) = 0;
        virtual std::uint8_t getChimeInterval() const     = 0;

        virtual void setChimeTone(const UTF8 &value) = 0;
        virtual UTF8 getChimeTone() const            = 0;

        virtual void setChimeVolume(std::uint8_t value) = 0;
        virtual std::uint8_t getChimeVolume() const     = 0;
    };

} // namespace app::bell_settings
