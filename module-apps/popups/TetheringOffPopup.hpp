// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <windows/Dialog.hpp>

namespace gui
{
    class TetheringOffPopup : public DialogYesNo
    {
      public:
        TetheringOffPopup(app::Application *app, const std::string &name);
        top_bar::Configuration configureTopBar(top_bar::Configuration appConfiguration) override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
    };

} // namespace gui
