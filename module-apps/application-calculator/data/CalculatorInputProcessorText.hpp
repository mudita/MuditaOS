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
        void writeEquation(bool lastCharIsSymbol, const UTF8 &symbol);

        bool isPreviousNumberDecimal() const;
        bool decimalLimitReached() const;

        bool inputContainsExponent() const;

        std::uint32_t getPenultimate() const;

        gui::Text *inputField{nullptr};
        bool clearInput{false};
    };

} // namespace calc
