// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include <common/models/AbstractBedtimeModel.hpp>
#include "SettingsListItemProvider.hpp"

namespace app
{
    class SongsModel;
}

namespace app::list_items
{
    class NumericWithBar;
}

namespace app::bell_settings
{
    class BedtimeSettingsListItemProvider : public SettingsListItemProvider
    {
      public:
        BedtimeSettingsListItemProvider(std::shared_ptr<AbstractBedtimeModel> model,
                                        std::shared_ptr<SongsModel> songsModel);

        auto getCurrentVolume() -> std::uint8_t;

      private:
        void buildListItems();

        std::shared_ptr<AbstractBedtimeModel> settingsModel;
        std::shared_ptr<SongsModel> songsModel;
        app::list_items::NumericWithBar *bedtimeVolume{nullptr};
        UTF8 currentSoundPath;
    };
} // namespace app::bell_settings
