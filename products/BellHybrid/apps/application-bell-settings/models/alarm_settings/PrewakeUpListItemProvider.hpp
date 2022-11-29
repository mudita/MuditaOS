// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "SettingsListItemProvider.hpp"
#include "AbstractPrewakeUpSettingsModel.hpp"
#include <common/models/AbstractSettingsModel.hpp>

namespace app::bell_settings
{
    class AbstractFrontlightModel;
    class PrewakeUpListItemProvider : public SettingsListItemProvider
    {
      public:
        PrewakeUpListItemProvider(AbstractPrewakeUpSettingsModel &settingsModel, std::vector<UTF8> chimeToneRange);

      private:
        void buildListItems(std::vector<UTF8> preWakeupToneRange);

        AbstractPrewakeUpSettingsModel &settingsModel;
    };

} // namespace app::bell_settings
