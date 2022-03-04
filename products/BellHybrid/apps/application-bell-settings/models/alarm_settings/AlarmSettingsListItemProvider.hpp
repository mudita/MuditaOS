// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "SettingsListItemProvider.hpp"
#include <common/models/AbstractAlarmSettingsModel.hpp>

namespace app::bell_settings
{
    class AlarmSettingsListItemProvider : public SettingsListItemProvider
    {
      public:
        AlarmSettingsListItemProvider(AbstractAlarmSettingsModel &model, std::vector<UTF8> alarmToneRange);

      private:
        void buildListItems(std::vector<UTF8> alarmTonesRange);

        AbstractAlarmSettingsModel &model;
    };

} // namespace app::bell_settings
