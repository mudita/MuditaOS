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
