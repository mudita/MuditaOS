// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "OptionWindow.hpp"
#include "application-phonebook/data/PhonebookItemData.hpp"

#include <ContactRecord.hpp>

namespace gui
{
    class PhonebookContactOptions : public OptionWindow
    {
      public:
        PhonebookContactOptions(app::ApplicationCommon *app);
        ~PhonebookContactOptions() override = default;
        auto handleSwitchData(SwitchData *data) -> bool override;

      private:
        PhonebookItemData::RequestType requestType = PhonebookItemData::RequestType::Internal;
        enum class NotificationType
        {
            Block,
            Delete,
            Unblock
        };
        std::shared_ptr<ContactRecord> contact = nullptr;
        auto contactOptionsList() -> std::list<gui::Option>;
        auto contactRemove() -> bool;
        auto showNotification(NotificationType notificationType) -> bool;
    };
}; // namespace gui
