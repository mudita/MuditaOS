// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "SettingsListItemProvider.hpp"
#include "AbstractSnoozeSettingsModel.hpp"

namespace app::list_items
{
    class NumericWithBar;
}

namespace app::bell_settings
{
    class SnoozeListItemProvider : public SettingsListItemProvider
    {
      public:
        SnoozeListItemProvider(AbstractSnoozeSettingsModel &model, std::vector<UTF8> chimeTonesRange);

        auto getCurrentVolume() -> std::uint8_t;

      private:
        void buildListItems(std::vector<UTF8> chimeTonesRange);

        AbstractSnoozeSettingsModel &model;
        app::list_items::NumericWithBar *snoozeChimeVolume{nullptr};
    };
} // namespace app::bell_settings
