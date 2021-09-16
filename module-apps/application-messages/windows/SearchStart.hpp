// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "AppWindow.hpp"

namespace gui
{
    class SMSSearch : public AppWindow
    {
        gui::Item *body = nullptr;

      public:
        SMSSearch(app::ApplicationCommon *app);
        bool onInput(const InputEvent &inputEvent) override;
    };

} // namespace gui
