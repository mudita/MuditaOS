// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "utf8/UTF8.hpp"
#include "TextConstants.hpp"
#include "TextBlock.hpp"
#include <cstdio>
#include <stdint.h>
#include <string>
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
        [[nodiscard]] auto currentBlock() const -> std::_List_iterator<TextBlock>;
        [[nodiscard]] auto lastBlock() const -> std::_List_iterator<TextBlock>;
        [[nodiscard]] auto blocksEnd() const -> std::_List_iterator<TextBlock>;
        [[nodiscard]] auto blocksBegin() const -> std::_List_iterator<TextBlock>;
        RawFont *default_font = nullptr;

      private:
        unsigned int pos            = text::npos;
        unsigned int currentBlockNr = text::npos;
        unsigned int lastBlockNr    = text::npos;

      protected:
        [[nodiscard]] auto checkNpos() const -> bool;
        void resetNpos();
        [[nodiscard]] auto checkDocument() const -> bool;

      public:
        /// gets cursor pointing in position in block
        /// @note it does not select next block - to do so add another ctor based on operator+
        /// and check if this one is needed
        BlockCursor(TextDocument *document, unsigned int pos, unsigned int block_nr, RawFont *default_font);
        BlockCursor() = default; /// bad cursor

        [[nodiscard]] auto getPosition() const -> unsigned int
        {
            return pos;
        }

        [[nodiscard]] auto getDocument() const -> TextDocument *
        {
            return document;
        }

        [[nodiscard]] auto getBlockNr() const -> unsigned int
        {
            return currentBlockNr;
        }

        [[nodiscard]] auto getLastBlockNr() const -> unsigned int
        {
            return lastBlockNr;
        }

        [[nodiscard]] auto checkLastBlockNewLine() const -> bool
        {
            if (lastBlockNr != text::npos) {
                return lastBlock()->getEnd() == TextBlock::End::Newline;
            }
            else {
                return false;
            }
        }

        [[nodiscard]] auto checkCurrentBlockNewLine() const -> bool
        {
            if (currentBlockNr != text::npos) {
                return currentBlock()->getEnd() == TextBlock::End::Newline;
            }
            else {
                return false;
            }
        }

        auto clearLastBlockNr() -> void
        {
            lastBlockNr = text::npos;
        }

        [[nodiscard]] auto atBegin() const -> bool
        {
            if (document == nullptr || checkNpos()) {
                return false;
            }
            return pos == 0 && currentBlockNr == 0;
        }

        [[nodiscard]] auto atEnd() const -> bool;

        [[nodiscard]] auto atEol() -> bool;

        operator bool() const
        {
            return !checkNpos();
        }

        auto operator+=(unsigned int) -> BlockCursor &;
        auto operator++() -> BlockCursor &;
        auto operator-=(unsigned int) -> BlockCursor &;
        auto operator--() -> BlockCursor &;

        // return if handled ( this is not i.e. at begin/end)
        auto removeChar() -> bool;
        auto operator*() -> const TextBlock &;
        auto operator->() -> const TextBlock *;

        void addChar(uint32_t utf_val);
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
