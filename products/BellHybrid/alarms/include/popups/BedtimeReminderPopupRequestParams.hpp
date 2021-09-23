// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/popups/data/PopupRequestParams.hpp>

namespace gui
{
    class BedtimeReminderPopupRequestParams : public PopupRequestParams
    {
      public:
        BedtimeReminderPopupRequestParams() : PopupRequestParams{gui::popup::ID::BedtimeNotification}
        {}
    };
} // namespace gui
