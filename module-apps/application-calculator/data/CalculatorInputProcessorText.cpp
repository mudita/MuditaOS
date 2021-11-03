// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "CalculatorInputProcessorText.hpp"
#include "CalculatorUtility.hpp"
#include <module-gui/gui/input/InputEvent.hpp>
#include <module-gui/gui/widgets/text/Text.hpp>
#include <i18n/i18n.hpp>
#include <gsl/assert>

calc::InputProcessorText::InputProcessorText(gsl::strict_not_null<gui::Text *> inputField) : inputField{inputField}
{}

bool calc::InputProcessorText::handle(const gui::InputEvent &event)
{
    if (clearInput || inputContainsExponent()) {
        clear();
    }

    if (event.isLongRelease() && event.is(gui::KeyCode::KEY_PND)) {
        clear();
        return true;
    }

    if (!event.isShortRelease()) {
        return false;
    }

    if (event.is(gui::KeyCode::KEY_0) && inputField->getText() == "0") {
        return true;
    }

    if (shouldRestoreInput(event)) {
        restoreHiddenInput();
        return true;
    }

    if (shouldHideInput(event)) {
        hideCurrentInput();
    }

    const auto lastChar                          = lastCharacter();
    const auto penultimateCharIsSymbol           = penultimateCharacterIsSymbol();
    const auto penultimateCharIsDecimalSeparator = penultimateCharacterIsDecimalSeparator();

    if (lastChar == symbols::codes::zero && penultimateCharIsSymbol && !penultimateCharIsDecimalSeparator &&
        event.is(gui::KeyCode::KEY_0)) {
        return true;
    }

    if (event.is(gui::KeyCode::KEY_UP)) {
        handleOperation(symbols::strings::plus);
        return true;
    }

    if (event.is(gui::KeyCode::KEY_DOWN)) {
        handleOperation(symbols::strings::minus);
        return true;
    }

    if (event.is(gui::KeyCode::KEY_LEFT)) {
        handleOperation(symbols::strings::multiplication);
        return true;
    }

    if (event.is(gui::KeyCode::KEY_RIGHT)) {
        handleOperation(symbols::strings::division);
        return true;
    }

    if (event.is(gui::KeyCode::KEY_LF)) {
        if (!isCurrentNumberDecimal()) {
            addSymbol(symbols::strings::decimal_separator_str());
        }
        return true;
    }

    if (lastChar == symbols::codes::zero && penultimateCharIsSymbol && !penultimateCharIsDecimalSeparator &&
        !event.is(gui::KeyCode::KEY_0) && !event.is(gui::KeyCode::KEY_PND) && !event.is(gui::KeyCode::KEY_ENTER)) {
        inputField->removeChar();
        return false;
    }

    if (!event.is(gui::KeyCode::KEY_0) && !event.is(gui::KeyCode::KEY_ENTER) && inputField->getText() == "0") {
        clear();
    }

    if (event.is(gui::KeyCode::KEY_ENTER)) {
        compute();
        return true;
    }

    if (prohibidInput(event)) {
        // Consume event to don't allow more decimals
        return true;
    }

    return false;
}

void calc::InputProcessorText::clear()
{
    inputField->clear();
    hiddenPartOfEquation.clear();
    clearInput = false;
}

std::optional<uint32_t> calc::InputProcessorText::lastCharacter() const
{
    const auto &txt = inputField->getText();

    if (txt.empty()) {
        return {};
    }

    return txt[txt.length() - 1];
}

bool calc::InputProcessorText::lastCharacterIsSymbol() const
{
    const auto &c = lastCharacter();
    return c ? isSymbol(*c) : false;
}

bool calc::InputProcessorText::lastCharacterIsOperation() const
{
    const auto &c = lastCharacter();
    return c ? isOperation(*c) : false;
}

std::optional<uint32_t> calc::InputProcessorText::penultimateCharacter() const
{
    const auto &text = inputField->getText();
    const auto len   = text.length();

    if (len < 2) {
        return {};
    }

    return text[len - 2];
}

bool calc::InputProcessorText::penultimateCharacterIsSymbol() const
{
    const auto &c = penultimateCharacter();
    return c ? isSymbol(*c) : false;
}

bool calc::InputProcessorText::penultimateCharacterIsDecimalSeparator() const
{
    const auto &c = penultimateCharacter();
    return c ? isDecimalSeparator(*c) : false;
}

bool calc::InputProcessorText::shouldComputeBeforeNextOperation() const
{
    if (!hasHiddenPart()) {
        return false;
    }

    if (inputField->getText().empty()) {
        return false;
    }

    return true;
}

void calc::InputProcessorText::handleOperation(const UTF8 &operation)
{
    if (shouldComputeBeforeNextOperation()) {
        compute();
    }

    addSymbol(operation);
}

void calc::InputProcessorText::addSymbol(const UTF8 &symbol)
{
    if (!inputField->getText().empty()) {

        if (lastCharacterIsSymbol() && symbol != symbols::strings::minus) {
            if (!penultimateCharacterIsSymbol() && inputField->getText().length() > 1) {
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

bool calc::InputProcessorText::shouldHideInput(const gui::InputEvent &event) const
{
    if (!lastCharacterIsOperation()) {
        return false;
    }

    if (!event.isDigit() && !event.is(gui::KeyCode::KEY_DOWN)) {
        return false;
    }

    if (inputField->getText() == symbols::strings::minus) {
        return false;
    }

    return true;
}

bool calc::InputProcessorText::shouldRestoreInput(const gui::InputEvent &event) const
{
    return event.is(gui::KeyCode::KEY_PND) && inputField->isEmpty() && hasHiddenPart();
}

void calc::InputProcessorText::hideCurrentInput()
{
    Expects(hiddenPartOfEquation.empty());

    hiddenPartOfEquation = inputField->getText();
    inputField->clear();
}

void calc::InputProcessorText::restoreHiddenInput()
{
    Expects(!hiddenPartOfEquation.empty());

    inputField->setText(hiddenPartOfEquation);
    hiddenPartOfEquation.clear();
}

bool calc::InputProcessorText::hasHiddenPart() const
{
    return !hiddenPartOfEquation.empty();
}

bool calc::InputProcessorText::isCurrentNumberDecimal() const
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
        return lastNumber.find(symbols::strings::decimal_separator_str()) != std::string::npos;
    }
    return false;
}

bool calc::InputProcessorText::inputContainsExponent() const
{
    return std::string{inputField->getText()}.find('e') != std::string::npos;
}

bool calc::InputProcessorText::prohibidInput(const gui::InputEvent &event) const
{
    if (!event.isDigit()) {
        return false;
    }

    if (charactedLimitReached() || decimalLimitReached()) {
        return true;
    }

    return false;
}

bool calc::InputProcessorText::charactedLimitReached() const
{
    return inputField->getText().length() >= limits::MaxInputLength;
}

bool calc::InputProcessorText::decimalLimitReached() const
{
    if (!isCurrentNumberDecimal()) {
        return false;
    }

    const auto &txt          = std::string{inputField->getText()};
    const auto separator_pos = txt.find_last_of(symbols::strings::decimal_separator_str());

    if ((txt.size() - separator_pos) > limits::MaxDecimalDigits) {
        return true;
    }

    return false;
}

void calc::InputProcessorText::compute()
{
    auto result = Calculator().calculate(hiddenPartOfEquation + inputField->getText());
    inputField->setText(result.value);
    hiddenPartOfEquation.clear();
    clearInput = result.isError;
}
