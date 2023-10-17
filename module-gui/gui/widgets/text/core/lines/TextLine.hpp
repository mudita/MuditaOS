// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <core/cursors/TextBlockCursor.hpp>
#include <core/RawText.hpp>

#include <BoxLayout.hpp>
#include <Common.hpp>

#include <numeric>
#include <list>

#include "ScopedParentDisown.hpp"

namespace gui
{
    namespace text
    {
        constexpr auto ellipsis_signs = "...";
    }

    enum class UnderlineDrawMode
    {
        WholeLine,
        Concurrent
    };

    struct UnderLineProperties
    {
        bool draw                           = false;
        Position underLinePadding           = 0;
        unsigned int thickness              = 1;
        Length underlineHeight              = 0;
        UnderlineDrawMode drawUnderlineMode = UnderlineDrawMode::WholeLine;
    };

    /// interface element for TextDocument->getLine() <-- Text
    class TextLine
    {
      protected:
        unsigned int shownLetterCount = 0;
        Length widthUsed              = 0;
        Length heightUsed             = 0;
        Length maxWidth               = 0;
        std::list<RawText *> lineContent;
        Rect *underline = nullptr;
        UnderLineProperties underLineProperties;
        TextBlock::End end                  = TextBlock::End::None;
        Position storedYOffset              = 0;
        bool lineEnd                        = false;
        bool lineVisible                    = true;
        unsigned int lineStartBlockNumber   = text::npos;
        unsigned int lineStartBlockPosition = text::npos;
        TextEllipsis drawnEllipsis          = TextEllipsis::None;

        void createUnderline(unsigned int maxWidth, unsigned int maxHeight);
        void updateUnderline(const short &x, const short &y);
        void setLineStartConditions(unsigned int startBlockNumber, unsigned int startBlockPosition);
        RawText *buildUITextPart(const UTF8 &text, const TextFormat *format);

        explicit TextLine(Length maxWidth) : maxWidth(maxWidth){};

      public:
        TextLine(TextLine &) = delete;
        TextLine(TextLine &&) noexcept;
        ~TextLine();

        /// number of letters in Whole TextLines
        [[nodiscard]] unsigned int length() const noexcept
        {
            return shownLetterCount;
        }

        /// count of elements in whole TextLine
        [[nodiscard]] unsigned int count() const noexcept
        {
            return std::accumulate(lineContent.begin(), lineContent.end(), 0U, [](const auto sum, const auto &content) {
                return sum + content->getTextLength();
            });
        }

        [[nodiscard]] bool empty() const noexcept
        {
            return ((end == TextBlock::End::Newline && count() == 1) || (end == TextBlock::End::None && count() == 0));
        }

        [[nodiscard]] Length width() const noexcept
        {
            return widthUsed;
        }

        [[nodiscard]] Length height() const noexcept
        {
            return heightUsed;
        }

        [[nodiscard]] TextBlock::End getEnd() const noexcept
        {
            return end;
        }

        [[nodiscard]] bool getLineEnd() const noexcept
        {
            return lineEnd;
        }

        [[nodiscard]] auto isVisible() const noexcept
        {
            return lineVisible;
        }

        void setVisible(bool val) noexcept
        {
            lineVisible = val;
        }

        [[nodiscard]] auto getLineStartBlockNumber() const noexcept
        {
            return lineStartBlockNumber;
        }

        [[nodiscard]] auto getLineStartBlockPosition() const noexcept
        {
            return lineStartBlockPosition;
        }

        [[nodiscard]] int32_t getX() const noexcept
        {
            return lineContent.front()->area().pos(Axis::X);
        }

        void setPosition(const short &x, const short &y);
        void setParent(Item *parent);
        [[nodiscard]] Length getWidth() const;
        [[nodiscard]] Length getWidthTo(unsigned int pos) const;
        void erase();
        /// align TextLine due to alignment axis in parent_width
        ///
        /// moves Text parts in Text. To not call n times callbacks on resize, call prior to setting parent
        void alignH(Alignment align, Length parent_length) const;
        void alignV(Alignment align, Length parent_length, Length lines_height);
        [[nodiscard]] const Item *getElement(unsigned int pos) const noexcept;
        [[nodiscard]] auto getText(unsigned int pos) const -> UTF8;
    };
} // namespace gui
