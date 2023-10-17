// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <windows/Dialog.hpp>

namespace gui
{
    class TetheringOffPopup : public DialogYesNo
    {
      public:
        TetheringOffPopup(app::ApplicationCommon *app, const std::string &name);
        status_bar::Configuration configureStatusBar(status_bar::Configuration appConfiguration) override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
    };

} // namespace gui
