// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/popups/data/PopupRequestParams.hpp>

namespace gui
{
    class ChargingNotificationPopupRequestParams : public PopupRequestParams
    {
      public:
        ChargingNotificationPopupRequestParams() : PopupRequestParams{gui::popup::ID::ChargingNotification}
        {}
    };
} // namespace gui
