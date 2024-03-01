// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <common/widgets/BellSideListItemWithCallbacks.hpp>
#include <apps-common/InternalModel.hpp>
#include <common/models/AbstractBedtimeModel.hpp>
#include <Service/Service.hpp>
#include "SettingsListItemProvider.hpp"

namespace app::list_items
{
    class NumericWithBar;
}

namespace app::bell_settings
{
    class BedtimeSettingsListItemProvider : public SettingsListItemProvider
    {
      public:
        BedtimeSettingsListItemProvider(std::shared_ptr<AbstractBedtimeModel> model, std::vector<UTF8> chimeToneRange);

        auto getCurrentVolume() -> std::uint8_t;

      private:
        void buildListItems(std::vector<UTF8> bedtimeToneRange);

        std::shared_ptr<AbstractBedtimeModel> model;
        app::list_items::NumericWithBar *bedtimeVolume{nullptr};
    };
} // namespace app::bell_settings
