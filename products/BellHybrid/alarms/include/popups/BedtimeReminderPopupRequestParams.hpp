// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/popups/data/PopupRequestParams.hpp>
#include <Tables/Record.hpp>

namespace gui
{
    class BedtimeReminderPopupRequestParams : public PopupRequestParams
    {
      public:
        BedtimeReminderPopupRequestParams(Record record)
            : PopupRequestParams{gui::popup::ID::BedtimeNotification}, eventRecord{record}
        {}

        Record eventRecord;
    };
} // namespace gui
