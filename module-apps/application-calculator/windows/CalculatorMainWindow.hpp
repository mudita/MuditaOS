// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "windows/AppWindow.hpp"
#include "Application.hpp"
#include "application-calculator/widgets/MathOperationsBox.hpp"
#include <module-gui/gui/widgets/Text.hpp>
#include <module-utils/tinyexpr/tinyexpr.h>

namespace gui
{

    class CalculatorMainWindow : public gui::AppWindow
    {
        gui::Text *mathOperationInput   = nullptr;
        gui::MathOperationsBox *mathBox = nullptr;

        bool clearInput = false;
        UTF8 previousOperation;
        void writeEquation(bool lastCharIsSymbol, const UTF8 &symbol);
        void applyInputCallback();
        bool isPreviousNumberDecimal();
        bool isSymbol(uint32_t character);
        bool isDecimalSeparator(uint32_t character);
        void processInput(bool lastCharIsSymbol, const UTF8 &symbol, uint32_t lastChar);
        void processClearInput();
        void showResult();
        uint32_t getPenultimate();

      public:
        CalculatorMainWindow(app::Application *app, std::string name);

        ~CalculatorMainWindow() override = default;
        void buildInterface() override;
        bool onInput(const gui::InputEvent &inputEvent) override;
    };

} // namespace gui
