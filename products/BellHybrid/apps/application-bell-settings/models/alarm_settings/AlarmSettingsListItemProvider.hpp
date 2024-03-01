// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "SettingsListItemProvider.hpp"
#include <common/models/AbstractAlarmSettingsModel.hpp>
#include <common/models/AbstractSettingsModel.hpp>

namespace app::list_items
{
    class NumericWithBar;
}

namespace app::bell_settings
{
    class AbstractFrontlightModel;
    class AlarmSettingsListItemProvider : public SettingsListItemProvider
    {
      public:
        AlarmSettingsListItemProvider(AbstractAlarmSettingsModel &settingsModel, std::vector<UTF8> alarmToneRange);

        auto getCurrentVolume() -> std::uint8_t;

      private:
        void buildListItems(std::vector<UTF8> alarmTonesRange);

        AbstractAlarmSettingsModel &settingsModel;
        app::list_items::NumericWithBar *alarmVolume{nullptr};
    };
} // namespace app::bell_settings
