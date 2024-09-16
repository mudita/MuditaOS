// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include "SettingsListItemProvider.hpp"
#include "AbstractAlarmSettingsModel.hpp"
#include <common/models/AbstractSettingsModel.hpp>
#include <common/models/SongsModel.hpp>

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
        AlarmSettingsListItemProvider(AbstractAlarmSettingsModel &settingsModel,
                                      std::shared_ptr<SongsModel> songsModel);

        auto getCurrentVolume() -> std::uint8_t;

      private:
        void buildListItems();

        AbstractAlarmSettingsModel &settingsModel;
        app::list_items::NumericWithBar *alarmVolume{nullptr};
        std::shared_ptr<SongsModel> songsModel;
        UTF8 currentSoundPath;
    };
} // namespace app::bell_settings
