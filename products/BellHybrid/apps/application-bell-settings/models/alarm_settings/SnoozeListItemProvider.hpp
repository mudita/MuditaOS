// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "SettingsListItemProvider.hpp"
#include "AbstractSnoozeSettingsModel.hpp"

namespace app::bell_settings
{
    class SnoozeListItemProvider : public SettingsListItemProvider
    {
      public:
        SnoozeListItemProvider(AbstractSnoozeSettingsModel &model, std::vector<UTF8> chimeTonesRange);

      private:
        void buildListItems(std::vector<UTF8> chimeTonesRange);

        AbstractSnoozeSettingsModel &model;
    };

} // namespace app::bell_settings
