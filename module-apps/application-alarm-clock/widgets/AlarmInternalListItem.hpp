// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include <module-db/Interface/AlarmsRecord.hpp>
#include <ListItem.hpp>

namespace gui
{
    class AlarmInternalListItem : public ListItem
    {
      public:
        std::function<void(std::shared_ptr<AlarmsRecord> event)> onSaveCallback = nullptr;
        std::function<void(std::shared_ptr<AlarmsRecord> event)> onLoadCallback = nullptr;
        std::function<bool()> onContentChangedCallback                          = nullptr;
    };

} /* namespace gui */
