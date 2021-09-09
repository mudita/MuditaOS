// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <string>

#include "Dialog.hpp"

namespace gui
{
    class TetheringConfirmationPopup : public DialogYesNo
    {
      public:
        TetheringConfirmationPopup(app::ApplicationCommon *app, const std::string &name);

        void onBeforeShow(ShowMode mode, SwitchData *data) override;
    };
}; // namespace gui
