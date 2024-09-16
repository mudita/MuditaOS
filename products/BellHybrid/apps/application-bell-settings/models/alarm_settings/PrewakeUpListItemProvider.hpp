// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

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
    class AbstractFrontlightModel;
    class AbstractPrewakeUpSettingsModel;

    class PrewakeUpListItemProvider : public SettingsListItemProvider
    {
      public:
        PrewakeUpListItemProvider(AbstractPrewakeUpSettingsModel &settingsModel,
                                  std::shared_ptr<SongsModel> songsModel);

        auto getCurrentVolume() -> std::uint8_t;

      private:
        void buildListItems();

        AbstractPrewakeUpSettingsModel &settingsModel;
        app::list_items::NumericWithBar *prewakeUpVolume{nullptr};
        std::shared_ptr<SongsModel> songsModel;
        UTF8 currentSoundPath;
    };
} // namespace app::bell_settings
