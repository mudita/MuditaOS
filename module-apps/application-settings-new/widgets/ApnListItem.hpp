// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "application-settings-new/data/SettingsItemData.hpp"

namespace gui
{
    class ApnListItem : public ListItem
    {
      public:
        std::function<void(std::shared_ptr<packet_data::APN::Config> apnRecord)> onSaveCallback = nullptr;
        std::function<void(std::shared_ptr<packet_data::APN::Config> apnRecord)> onLoadCallback = nullptr;
        std::function<bool()> onEmptyCallback                                                   = nullptr;
    };

} /* namespace gui */
