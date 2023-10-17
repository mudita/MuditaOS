// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <AppWindow.hpp>

#include <vector>

namespace gui
{
    class ChargingBatteryWindow : public AppWindow
    {
      public:
        explicit ChargingBatteryWindow(app::ApplicationCommon *app);
        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;
        bool onInput(const InputEvent &inputEvent) override;
    };

} /* namespace gui */
