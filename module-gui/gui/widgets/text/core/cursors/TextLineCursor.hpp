// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "TextCursor.hpp"

namespace gui
{
    class Text;
    class TextDocument;

    class TextLineCursor : public TextCursor
    {
      protected:
        auto checkNextLineDocumentEnd(unsigned int selectedLineNumber) -> bool;
        void handleDownNavigation(unsigned int selectedLineNumber, unsigned int selectedLineCursorPosition);
        void handleUpNavigation(unsigned int selectedLineNumber, unsigned int selectedLineCursorPosition);

        [[nodiscard]] bool handleNextLine() override;
        [[nodiscard]] bool handlePreviousLine() override;

      public:
        explicit TextLineCursor(gui::Text *parent, unsigned int pos = text::npos, unsigned int block = text::npos);
        TextLineCursor() = delete;

        auto displayPreviousLine() -> bool;
        auto displayNextLine() -> bool;

        auto moveCursor(NavigationDirection direction) -> Move override;
        auto moveCursor(NavigationDirection direction, unsigned int n) -> Move override;

        bool removeChar() final;
    };

} // namespace gui
