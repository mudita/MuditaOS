// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include <apps-common/popups/data/PopupRequestParams.hpp>

namespace gui
{
    class ChargingDoneNotificationPopupRequestParams : public PopupRequestParams
    {
      public:
        ChargingDoneNotificationPopupRequestParams() : PopupRequestParams{gui::popup::ID::ChargingDoneNotification}
        {}
    };
} // namespace gui
