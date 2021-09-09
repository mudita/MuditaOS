// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "AbstractSnoozeSettingsModel.hpp"
#include <service-db/Settings.hpp>

namespace app
{
    class Application;
}

namespace app::bell_settings
{

    class SnoozeSettingsModel : public AbstractSnoozeSettingsModel
    {
      public:
        explicit SnoozeSettingsModel(Application *app);
        void setSnooze(bool value) override;
        bool getSnooze() const override;

        void setLength(std::uint8_t value) override;
        std::uint8_t getLength() const override;

        void setChimeInterval(std::uint8_t value) override;
        std::uint8_t getChimeInterval() const override;

        void setChimeTone(const UTF8 &value) override;
        UTF8 getChimeTone() const override;

        void setChimeVolume(std::uint8_t value) override;
        std::uint8_t getChimeVolume() const override;

      private:
        mutable settings::Settings settings;
    };

} // namespace app::bell_settings
