// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <data/CalculatorInputProcessor.hpp>
#include <widgets/MathOperationsBox.hpp>
#include <apps-common/Application.hpp>
#include <apps-common/windows/AppWindow.hpp>
#include <module-gui/gui/widgets/Text.hpp>

namespace gui
{
    class CalculatorMainWindow : public gui::AppWindow
    {
        gui::Text *mathOperationInput   = nullptr;
        gui::MathOperationsBox *mathBox = nullptr;
        std::unique_ptr<calc::InputProcessor> inputProcessor;

        void setupInputProcessing();

      public:
        CalculatorMainWindow(app::Application *app, std::string name);

        ~CalculatorMainWindow() override = default;
        void buildInterface() override;
        bool onInput(const gui::InputEvent &inputEvent) override;
    };

} // namespace gui
