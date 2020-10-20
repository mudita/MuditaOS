// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "application-phonebook/data/PhonebookItemData.hpp"

namespace gui
{
    class ContactListItem : public ListItem
    {
      public:
        std::function<void(std::shared_ptr<ContactRecord> contact)> onSaveCallback = nullptr;
        std::function<void(std::shared_ptr<ContactRecord> contact)> onLoadCallback = nullptr;
        std::function<bool()> onEmptyCallback                                      = nullptr;
    };

} /* namespace gui */
