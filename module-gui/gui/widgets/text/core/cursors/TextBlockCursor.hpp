// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <TextConstants.hpp>
#include <core/TextBlock.hpp>

#include <list>

namespace gui
{

    class TextDocument;
    class TextBlock;
    class RawFont;

    /// element to:
    /// 1. get position in TextDocument and
    /// 2. work as iterator on TextDocument
    ///
    /// pos - points to curent character position pointed via cursor
    /// to add character we insert element in cursor position
    /// to remove character we remove element pointed via cursor
    class BlockCursor
    {
      protected:
        TextDocument *document = nullptr;
        [[nodiscard]] auto currentBlock() const -> std::list<TextBlock>::iterator;
        [[nodiscard]] auto blocksEnd() const -> std::list<TextBlock>::iterator;
        [[nodiscard]] auto blocksBegin() const -> std::list<TextBlock>::iterator;
        RawFont *default_font = nullptr;

      private:
        bool emptyNewLineAdded          = false;
        bool blockChanged               = false;
        unsigned int pos                = text::npos;
        unsigned int currentBlockNumber = text::npos;

      protected:
        [[nodiscard]] auto checkNpos() const -> bool;
        void resetNpos();
        [[nodiscard]] auto checkDocument() const -> bool;

      public:
        /// gets cursor pointing in position in block
        /// @note it does not select next block - to do so add another ctor based on operator+
        /// and check if this one is needed
        BlockCursor(TextDocument *document, unsigned int pos, unsigned int block_nr, RawFont *default_font);
        BlockCursor()          = default; /// bad cursor
        virtual ~BlockCursor() = default;

        [[nodiscard]] auto getPosition() const -> unsigned int
        {
            return pos;
        }

        [[nodiscard]] auto getDocument() const -> TextDocument *
        {
            return document;
        }

        [[nodiscard]] auto getBlockNumber() const -> unsigned int
        {
            return currentBlockNumber;
        }

        [[nodiscard]] auto checkPreviousBlockNewLine() const -> bool
        {
            if (currentBlockNumber != text::npos) {
                if (currentBlockNumber > 0) {
                    return (--currentBlock())->getEnd() == TextBlock::End::Newline;
                }
                else {
                    return false;
                }
            }
            else {
                return false;
            }
        }

        [[nodiscard]] auto checkAndInvalidateBlockChanged() -> bool
        {
            if (blockChanged) {
                blockChanged = false;
                return true;
            }
            return false;
        }

        [[nodiscard]] auto checkLastNewLine() -> bool
        {
            if (!emptyNewLineAdded && checkPreviousBlockNewLine() &&
                currentBlock()->length() + (currentBlock()->getEnd() != TextBlock::End::Newline ? 0 : -1) == 0) {

                emptyNewLineAdded = true;
                return true;
            }
            return false;
        }

        [[nodiscard]] auto checkPreviousBlockNoNewLine() const -> bool
        {
            if (currentBlockNumber != text::npos) {
                if (currentBlockNumber > 0) {
                    return (--currentBlock())->getEnd() == TextBlock::End::None;
                }
                else {
                    return false;
                }
            }
            else {
                return false;
            }
        }

        [[nodiscard]] auto checkCurrentBlockNoNewLine() const -> bool
        {
            if (currentBlockNumber != text::npos && currentBlock() != blocksEnd()) {
                return (currentBlock())->getEnd() == TextBlock::End::None;
            }
            else {
                return false;
            }
        }

        [[nodiscard]] auto atBegin() const -> bool
        {
            if (document == nullptr || checkNpos()) {
                return false;
            }
            return pos == 0 && currentBlockNumber == 0;
        }

        [[nodiscard]] auto atEnd() const -> bool;

        operator bool() const
        {
            return !checkNpos();
        }

        auto goToNextBlock() -> BlockCursor &;
        auto goToPreviousBlock() -> BlockCursor &;

        auto checkIfBlockFirstAndEmpty(std::list<TextBlock>::iterator block) -> bool;
        auto checkIfNextBlockEmpty(std::list<TextBlock>::iterator nextBlock) -> bool;

        auto operator+=(unsigned int) -> BlockCursor &;
        auto operator++() -> BlockCursor &;
        auto operator-=(unsigned int) -> BlockCursor &;
        auto operator--() -> BlockCursor &;

        // return if handled ( this is not i.e. at begin/end)
        virtual auto removeChar() -> bool;
        auto operator*() -> const TextBlock &;
        auto operator->() -> const TextBlock *;

        virtual void addChar(uint32_t utf_val);
        void addTextBlock(TextBlock &&);

        [[nodiscard]] auto getText() -> std::string;
        auto getUTF8Text() -> UTF8;

        /// iterable
        /// {
        auto begin() -> std::list<TextBlock>::iterator;
        auto end() -> std::list<TextBlock>::iterator;
        /// }
    };
} // namespace gui
