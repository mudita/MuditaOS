// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <vector>
#include "AppWindow.hpp"

namespace gui
{
    class DeadBatteryWindow : public AppWindow
    {
      public:
        explicit DeadBatteryWindow(app::Application *app);
        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;

      private:
        gui::Image *image = nullptr;
    };

} /* namespace gui */
