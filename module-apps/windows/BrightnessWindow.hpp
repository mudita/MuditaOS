// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "module-apps/Application.hpp"
#include "AppWindow.hpp"
#include "module-apps/widgets/BarGraph.hpp"
#include "module-apps/widgets/BightnessBox.hpp"
#include <functional>

namespace gui
{
    class BrightnessWindow : public AppWindow
    {

      protected:
        Rect *border                  = nullptr;
        BrightnessBox *brightnessText = nullptr;
        HBarGraph *brightnessBar      = nullptr;

      public:
        BrightnessWindow(app::Application *app, const std::string &name);

        ~BrightnessWindow() override;

        void addBox();

        void addBrightnessText();

        void addBrightnessBar();

        void onBeforeShow(ShowMode mode, SwitchData *data) override;

        void rebuild() override;

        void buildInterface() override;

        void destroyInterface() override;

        bool onInput(const gui::InputEvent &inputEvent) override;
    };
}; // namespace gui
