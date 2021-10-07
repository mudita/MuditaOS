// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "SettingsListItemProvider.hpp"
#include "AbstractPrewakeUpSettingsModel.hpp"

namespace app::bell_settings
{
    class PrewakeUpListItemProvider : public SettingsListItemProvider
    {
      public:
        PrewakeUpListItemProvider(AbstractPrewakeUpSettingsModel &model, std::vector<UTF8> chimeToneRange);

      private:
        void buildListItems(std::vector<UTF8> preWakeupToneRange);

        AbstractPrewakeUpSettingsModel &model;
    };

} // namespace app::bell_settings
