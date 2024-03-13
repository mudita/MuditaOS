// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "SettingsListItemProvider.hpp"
#include "AbstractPrewakeUpSettingsModel.hpp"
#include <common/models/AbstractSettingsModel.hpp>
#include <common/models/SongsModel.hpp>

namespace app::list_items
{
    class NumericWithBar;
}

namespace app::bell_settings
{
    class AbstractFrontlightModel;
    class PrewakeUpListItemProvider : public SettingsListItemProvider
    {
      public:
        PrewakeUpListItemProvider(
            AbstractPrewakeUpSettingsModel &settingsModel,
            std::vector<UTF8> chimeToneRange,
            std::shared_ptr<SongsModel> model); // TODO chimeToneRange is probably no longer needed

        auto getCurrentVolume() -> std::uint8_t;

      private:
        void buildListItems(std::vector<UTF8> preWakeupToneRange);

        AbstractPrewakeUpSettingsModel &settingsModel;
        app::list_items::NumericWithBar *prewakeUpVolume{nullptr};
        std::shared_ptr<SongsModel> model;
    };
} // namespace app::bell_settings
