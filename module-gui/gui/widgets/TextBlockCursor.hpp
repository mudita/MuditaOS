#pragma once

#include "utf8/UTF8.hpp"
#include "TextConstants.hpp"
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
        auto currentBlock() const;
        auto blocksEnd() const;
        auto blocksBegin() const;
        RawFont *default_font = nullptr;

      private:
        unsigned int pos      = text::npos;
        unsigned int block_nr = text::npos;
        bool block_jump       = false;

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

        [[nodiscard]] auto getBlockNr() const -> unsigned int
        {
            return block_nr;
        }

        [[nodiscard]] auto atBegin() const -> bool
        {
            if (document == nullptr || checkNpos()) {
                return false;
            }
            return pos == 0 && block_nr == 0;
        }

        [[nodiscard]] auto atEnd() const -> bool;

        [[nodiscard]] auto atEol() const -> bool;

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
        void resetJumps();

        /// iterable
        /// {
        auto begin() -> std::list<TextBlock>::iterator;
        auto end() -> std::list<TextBlock>::iterator;
        /// }
    };
} // namespace gui
