// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "CalculatorInputProcessor.hpp"
#include <gsl/pointers>

namespace gui
{
    class Text;
}

namespace calc
{
    class InputProcessorText : public InputProcessor
    {
      public:
        InputProcessorText(gsl::strict_not_null<gui::Text *> inputField);

        bool handle(const gui::InputEvent &event) override;
        void clear() override;

      private:
        std::optional<std::uint32_t> lastCharacter() const;
        bool lastCharacterIsSymbol() const;
        bool lastCharacterIsOperation() const;

        std::optional<std::uint32_t> penultimateCharacter() const;
        bool penultimateCharacterIsSymbol() const;
        bool penultimateCharacterIsDecimalSeparator() const;

        bool shouldComputeBeforeNextOperation() const;
        void handleOperation(const UTF8 &operation);
        void addSymbol(const UTF8 &symbol);

        bool shouldHideInput(const gui::InputEvent &event) const;
        bool shouldRestoreInput(const gui::InputEvent &event) const;
        void hideCurrentInput();
        void restoreHiddenInput();
        bool hasHiddenPart() const;

        bool isCurrentNumberDecimal() const;
        bool inputContainsExponent() const;

        bool prohibidInput(const gui::InputEvent &event) const;
        bool charactedLimitReached() const;
        bool decimalLimitReached() const;

        void compute();

        gui::Text *inputField{nullptr};
        bool clearInput{false};
        UTF8 hiddenPartOfEquation{};
    };

} // namespace calc
