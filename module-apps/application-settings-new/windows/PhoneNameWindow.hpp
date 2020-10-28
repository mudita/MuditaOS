// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "AppWindow.hpp"

#include <Text.hpp>

namespace gui
{
    class PhoneNameWindow : public AppWindow
    {
      public:
        PhoneNameWindow(app::Application *app);
        void buildInterface() override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;

      private:
        Text *inputField = nullptr;
    };

} /* namespace gui */
