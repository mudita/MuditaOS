// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "CalculatorInputProcessorText.hpp"

#include "CalculatorUtility.hpp"
#include <gui/input/InputEvent.hpp>
#include <gui/widgets/Text.hpp>
#include <i18n/i18n.hpp>

calc::InputProcessorText::InputProcessorText(gsl::strict_not_null<gui::Text *> inputField) : inputField{inputField}
{}

bool calc::InputProcessorText::handle(const gui::InputEvent &event)
{
    if (clearInput || inputContainsExponent()) {
        clear();
    }

    if (event.isLongPress() && event.is(gui::KeyCode::KEY_PND)) {
        clear();
        return true;
    }

    if (!event.isShortPress()) {
        return false;
    }

    if (event.is(gui::KeyCode::KEY_0) && inputField->getText() == "0") {
        return true;
    }

    const auto lastChar         = inputField->getText()[inputField->getText().length() - 1];
    const bool lastCharIsSymbol = isSymbol(lastChar);
    if (lastChar == symbols::codes::zero && isSymbol(getPenultimate()) && !isDecimalSeparator(getPenultimate()) &&
        event.is(gui::KeyCode::KEY_0)) {
        return true;
    }

    if (event.keyCode == gui::KeyCode::KEY_UP) {
        writeEquation(lastCharIsSymbol, symbols::strings::plus);
        return true;
    }

    if (event.keyCode == gui::KeyCode::KEY_DOWN) {
        if (lastChar != symbols::codes::minus) {
            writeEquation(lastCharIsSymbol, symbols::strings::minus);
        }
        return true;
    }

    if (event.keyCode == gui::KeyCode::KEY_LEFT) {
        writeEquation(lastCharIsSymbol, symbols::strings::multiplication);
        return true;
    }

    if (event.keyCode == gui::KeyCode::KEY_RIGHT) {
        writeEquation(lastCharIsSymbol, symbols::strings::division);
        return true;
    }

    if (event.keyCode == gui::KeyCode::KEY_LF) {
        if (!isPreviousNumberDecimal()) {
            writeEquation(lastCharIsSymbol, utils::translate("app_calculator_decimal_separator"));
        }
        return true;
    }

    if (lastChar == symbols::codes::zero && isSymbol(getPenultimate()) && !isDecimalSeparator(getPenultimate()) &&
        !event.is(gui::KeyCode::KEY_0) && !event.is(gui::KeyCode::KEY_PND) && !event.is(gui::KeyCode::KEY_ENTER)) {
        inputField->removeChar();
        return false;
    }

    if (!event.is(gui::KeyCode::KEY_0) && !event.is(gui::KeyCode::KEY_ENTER) && inputField->getText() == "0") {
        clear();
    }

    if (event.keyCode == gui::KeyCode::KEY_ENTER) {
        auto result = Calculator().calculate(std::string(inputField->getText()));
        inputField->setText(result.value);
        clearInput = result.isError;
        return true;
    }

    if (decimalLimitReached()) {
        // Consume event to don't allow more decimals
        return true;
    }

    return false;
}

void calc::InputProcessorText::clear()
{
    inputField->clear();
    clearInput = false;
}

void calc::InputProcessorText::writeEquation(bool lastCharIsSymbol, const UTF8 &symbol)
{
    if (!inputField->getText().empty()) {

        if (lastCharIsSymbol && symbol != symbols::strings::minus) {
            if (!isSymbol(getPenultimate()) && inputField->getText().length() > 1) {
                inputField->removeChar();
                inputField->addText(symbol);
            }
        }
        else {
            inputField->addText(symbol);
        }
    }
    else if (symbol == symbols::strings::minus) {
        inputField->addText(symbol);
    }
}

bool calc::InputProcessorText::isPreviousNumberDecimal() const
{
    if (!inputField->getText().empty()) {
        std::vector<int> symbolsIndexes;
        auto input         = std::string(inputField->getText()).erase(inputField->getText().length() - 1);
        auto exponentIndex = input.find_last_of('e');
        auto minusIndex    = input.find_last_of(symbols::strings::minus);
        if (minusIndex != exponentIndex + 1) {
            symbolsIndexes.push_back(minusIndex);
        }
        symbolsIndexes.push_back(input.find_last_of(symbols::strings::plus));
        symbolsIndexes.push_back(input.find_last_of(symbols::strings::division));
        symbolsIndexes.push_back(input.find_last_of(symbols::strings::multiplication));
        auto it = std::max_element(symbolsIndexes.begin(), symbolsIndexes.end());
        std::string lastNumber;

        if (*it == -1) {
            lastNumber = input.substr(0, std::string::npos);
        }
        else {
            lastNumber = input.substr(*it, std::string::npos);
        }
        return lastNumber.find(utils::translate("app_calculator_decimal_separator")) != std::string::npos;
    }
    return false;
}

bool calc::InputProcessorText::decimalLimitReached() const
{
    if (!isPreviousNumberDecimal())
        return false;

    const auto &txt          = std::string{inputField->getText()};
    const auto separator_pos = txt.find_last_of(utils::translate("app_calculator_decimal_separator"));

    if ((txt.size() - separator_pos) > DecimalDigitsLimit)
        return true;

    return false;
}

bool calc::InputProcessorText::inputContainsExponent() const
{
    return std::string{inputField->getText()}.find('e') != std::string::npos;
}

std::uint32_t calc::InputProcessorText::getPenultimate() const
{
    const auto &text = inputField->getText();
    const auto len   = text.length();

    if (len < 2)
        return 0;

    return text[len - 2];
}
