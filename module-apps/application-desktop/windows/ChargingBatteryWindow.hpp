// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <vector>
#include "AppWindow.hpp"

namespace gui
{
    class ChargingBatteryWindow : public AppWindow
    {
      public:
        explicit ChargingBatteryWindow(app::Application *app);
        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;
        bool onInput(const InputEvent &inputEvent) override;
    };

} /* namespace gui */
