// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/popups/data/PopupRequestParams.hpp>

namespace gui
{
    class AlarmDeactivatedPopupRequestParams : public PopupRequestParams
    {
      public:
        AlarmDeactivatedPopupRequestParams() : PopupRequestParams{gui::popup::ID::AlarmDeactivated}
        {
          ignoreCurrentWindowOnStack = true;
        }
    };
} // namespace gui
