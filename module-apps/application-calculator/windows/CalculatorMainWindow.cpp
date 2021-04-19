// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "CalculatorMainWindow.hpp"
#include "application-calculator/widgets/CalculatorStyle.hpp"
#include "application-calculator/data/CalculatorUtility.hpp"
#include <i18n/i18n.hpp>

namespace gui
{

    CalculatorMainWindow::CalculatorMainWindow(app::Application *app, std::string name) : AppWindow(app, name)
    {
        buildInterface();
    }

    void CalculatorMainWindow::buildInterface()
    {
        AppWindow::buildInterface();

        setTitle(utils::localize.get("app_calculator_title_main"));

        bottomBar->setActive(gui::BottomBar::Side::CENTER, true);
        bottomBar->setActive(gui::BottomBar::Side::RIGHT, true);
        bottomBar->setActive(gui::BottomBar::Side::LEFT, true);
        bottomBar->setText(gui::BottomBar::Side::RIGHT, utils::localize.get(style::strings::common::back));
        bottomBar->setText(gui::BottomBar::Side::CENTER, utils::localize.get(style::calculator::equals));
        bottomBar->setText(gui::BottomBar::Side::LEFT, utils::localize.get(style::calculator::decimal_separator));
        bottomBar->setFont(BottomBar::Side::LEFT, style::window::font::largelight);

        mathOperationInput = new gui::Text(this,
                                           style::calculator::window::input_margin,
                                           style::calculator::window::input_offset_top,
                                           style::calculator::window::input_width,
                                           style::calculator::window::input_height);
        mathOperationInput->setEdges(gui::RectangleEdge::Bottom);
        mathOperationInput->setAlignment(
            gui::Alignment(gui::Alignment::Horizontal::Right, gui::Alignment::Vertical::Center));
        mathOperationInput->setFont(style::window::font::supersizemelight);
        mathOperationInput->setInputMode(new InputMode({InputMode::digit}));
        mathOperationInput->setPenFocusWidth(style::window::default_border_focus_w);
        mathOperationInput->setPenWidth(style::window::default_border_rect_no_focus);
        mathOperationInput->setEditMode(gui::EditMode::Edit);

        mathBox = new gui::MathOperationsBox(this,
                                             style::calculator::window::math_box_offset_top,
                                             style::window::default_body_width,
                                             style::calculator::window::math_box_height,
                                             style::calculator::window::math_box_cell_width,
                                             style::calculator::window::math_box_cell_height);
        addWidget(mathBox);
        setFocusItem(mathOperationInput);
        applyInputCallback();
    }

    void CalculatorMainWindow::applyInputCallback()
    {
        mathOperationInput->inputCallback = [&](Item &item, const InputEvent &event) {
            if (clearInput) {
                mathOperationInput->clear();
                clearInput = false;
            }
            if (event.isLongPress() && event.is(gui::KeyCode::KEY_PND)) {
                mathOperationInput->clear();
                return true;
            }
            if (!event.isShortPress()) {
                return false;
            }
            if (event.is(gui::KeyCode::KEY_0) && mathOperationInput->getText() == "0") {
                return true;
            }
            auto lastChar         = mathOperationInput->getText()[mathOperationInput->getText().length() - 1];
            bool lastCharIsSymbol = isSymbol(lastChar);
            if (lastChar == style::calculator::symbols::codes::zero && isSymbol(getPenultimate()) &&
                !isDecimalSeparator(getPenultimate()) && event.is(gui::KeyCode::KEY_0)) {
                return true;
            }
            if (event.keyCode == gui::KeyCode::KEY_UP) {
                writeEquation(lastCharIsSymbol, style::calculator::symbols::strings::plus);
                return true;
            }
            if (event.keyCode == gui::KeyCode::KEY_DOWN) {
                if (lastChar != style::calculator::symbols::codes::minus) {
                    writeEquation(lastCharIsSymbol, style::calculator::symbols::strings::minus);
                }
                return true;
            }
            if (event.keyCode == gui::KeyCode::KEY_LEFT) {
                writeEquation(lastCharIsSymbol, style::calculator::symbols::strings::multiplication);
                return true;
            }
            if (event.keyCode == gui::KeyCode::KEY_RIGHT) {
                writeEquation(lastCharIsSymbol, style::calculator::symbols::strings::division);
                return true;
            }
            if (event.keyCode == gui::KeyCode::KEY_LF) {
                if (!isPreviousNumberDecimal()) {
                    writeEquation(lastCharIsSymbol, utils::localize.get("app_calculator_decimal_separator"));
                }
                return true;
            }
            if (lastChar == style::calculator::symbols::codes::zero && isSymbol(getPenultimate()) &&
                !isDecimalSeparator(getPenultimate()) && !event.is(gui::KeyCode::KEY_0) &&
                !event.is(gui::KeyCode::KEY_PND) && !event.is(gui::KeyCode::KEY_ENTER)) {
                mathOperationInput->removeChar();
                return false;
            }
            if (!event.is(gui::KeyCode::KEY_0) && !event.is(gui::KeyCode::KEY_ENTER) &&
                mathOperationInput->getText() == "0") {
                mathOperationInput->clear();
            }
            return false;
        };
    }

    bool CalculatorMainWindow::isSymbol(uint32_t character)
    {
        return character == style::calculator::symbols::codes::plus ||
               character == style::calculator::symbols::codes::minus ||
               character == style::calculator::symbols::codes::division ||
               character == style::calculator::symbols::codes::multiplication ||
               character == style::calculator::symbols::codes::comma ||
               character == style::calculator::symbols::codes::full_stop;
    }

    bool CalculatorMainWindow::isDecimalSeparator(uint32_t character)
    {
        return character == style::calculator::symbols::codes::comma ||
               character == style::calculator::symbols::codes::full_stop;
    }

    uint32_t CalculatorMainWindow::getPenultimate()
    {
        if (mathOperationInput->getText().length() > 1) {
            return mathOperationInput->getText()[mathOperationInput->getText().length() - 2];
        }
        return 0;
    }

    void CalculatorMainWindow::writeEquation(bool lastCharIsSymbol, const UTF8 &symbol)
    {
        if (!mathOperationInput->getText().empty()) {

            if (lastCharIsSymbol && symbol != style::calculator::symbols::strings::minus) {
                if (!isSymbol(getPenultimate()) && mathOperationInput->getText().length() > 1) {
                    mathOperationInput->removeChar();
                    mathOperationInput->addText(symbol);
                }
            }
            else {
                mathOperationInput->addText(symbol);
            }
        }
        else if (symbol == style::calculator::symbols::strings::minus) {
            mathOperationInput->addText(symbol);
        }
    }

    bool CalculatorMainWindow::isPreviousNumberDecimal()
    {
        if (!mathOperationInput->getText().empty()) {
            std::vector<int> symbolsIndexes;
            auto input = std::string(mathOperationInput->getText()).erase(mathOperationInput->getText().length() - 1);
            auto exponentIndex = input.find_last_of('e');
            auto minusIndex    = input.find_last_of(style::calculator::symbols::strings::minus);
            if (minusIndex != exponentIndex + 1) {
                symbolsIndexes.push_back(minusIndex);
            }
            symbolsIndexes.push_back(input.find_last_of(style::calculator::symbols::strings::plus));
            symbolsIndexes.push_back(input.find_last_of(style::calculator::symbols::strings::division));
            symbolsIndexes.push_back(input.find_last_of(style::calculator::symbols::strings::multiplication));
            auto it = std::max_element(symbolsIndexes.begin(), symbolsIndexes.end());
            std::string lastNumber;

            if (*it == -1) {
                lastNumber = input.substr(0, std::string::npos);
            }
            else {
                lastNumber = input.substr(*it, std::string::npos);
            }
            return lastNumber.find(utils::localize.get("app_calculator_decimal_separator")) != std::string::npos;
        }
        return false;
    }

    bool CalculatorMainWindow::onInput(const gui::InputEvent &inputEvent)
    {
        if (AppWindow::onInput(inputEvent)) {
            return true;
        }

        if (!inputEvent.isShortPress()) {
            return false;
        }

        if (inputEvent.keyCode == gui::KeyCode::KEY_ENTER) {
            auto result = Calculator().calculate(std::string(mathOperationInput->getText()));
            mathOperationInput->setText(result.value);
            clearInput = result.isError;
            return true;
        }

        return false;
    }

} // namespace gui
