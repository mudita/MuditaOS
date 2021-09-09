// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <string>

#include "TetheringNotificationPopup.hpp"

namespace gui
{
    class TetheringPhoneModePopup : public TetheringNotificationPopup
    {
      public:
        TetheringPhoneModePopup(app::ApplicationCommon *app, const std::string &name);

        void onBeforeShow(ShowMode mode, SwitchData *data) override;
    };
}; // namespace gui
