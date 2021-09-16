// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/popups/data/PopupRequestParams.hpp>

namespace gui
{
    class AlarmActivatedPopupRequestParams : public PopupRequestParams
    {
      public:
        AlarmActivatedPopupRequestParams() : PopupRequestParams{gui::popup::ID::AlarmActivated}
        {}
    };
} // namespace gui
