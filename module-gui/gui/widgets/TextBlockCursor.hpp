#pragma once

#include "TextConstants.hpp"
#include <cstdio>
#include <stdint.h>

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
        auto currentBlock();
        auto blocksEnd();
        auto blocksBegin();
        RawFont *default_font = nullptr;

      private:
        unsigned int pos      = text::npos;
        unsigned int block_nr = text::npos;

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

        operator bool() const
        {
            return !checkNpos();
        }

        auto operator+=(unsigned int) -> BlockCursor &;
        auto operator++() -> BlockCursor &;
        auto operator-=(unsigned int) -> BlockCursor &;
        auto operator--() -> BlockCursor &;
        void addChar(uint32_t utf_val);
        void addTextBlock(TextBlock &&);
        // return if handled ( this is not i.e. at begin/end)
        bool removeChar();
        const TextBlock &operator*();
    };
} // namespace gui
