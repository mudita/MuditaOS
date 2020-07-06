#include "TextBlockCursor.hpp"
#include "TextBlock.hpp"
#include "TextDocument.hpp"
#include "TextParse.hpp"
#include "log/log.hpp"
#include <algorithm>
#include <cassert>
#include <iterator>

static const int last_char_inclusive = 0; // if then -1 / else 0

#define debug_cursor(...)
// #define debug_cursor(...) LOG_DEBUG(__VA_ARGS__)

namespace gui
{
    auto BlockCursor::curentBlock()
    {
        return std::next(document->blocks.begin(), block_nr);
    }

    auto BlockCursor::blocksEnd()
    {
        return std::end(document->blocks);
    }

    BlockCursor::BlockCursor(TextDocument *document, unsigned int pos, unsigned int block_nr, Font *default_font)
        : document(document), default_font(default_font)
    {
        this->pos      = text::npos;
        this->block_nr = text::npos;
        if (checkDocument()) {
            this->block_nr = block_nr < document->blocks.size() ? block_nr : document->blocks.size() - 1;
            auto block_len = std::next(document->blocks.begin(), this->block_nr)->length();
            this->pos      = pos < block_len ? pos : block_len + last_char_inclusive;
        }
    }

    [[nodiscard]] auto BlockCursor::checkNpos() const -> bool
    {
        return pos == text::npos || block_nr == text::npos;
    }

    void BlockCursor::resetNpos()
    {
        if (pos == text::npos)
            pos = 0;
        if (block_nr == text::npos)
            block_nr = 0;
    }

    [[nodiscard]] auto BlockCursor::checkDocument() const -> bool
    {
        return document != nullptr && document->blocks.size() > 0;
    }

    auto BlockCursor::atEnd() const -> bool
    {
        if (!checkDocument() || checkNpos()) {
            return false;
        }
        return block_nr == document->blocks.size() - 1 && pos == document->blocks.back().length() + last_char_inclusive;
    }

    auto BlockCursor::operator+=(unsigned int val) -> BlockCursor &
    {
        // just use operator ++
        for (unsigned int i = 0; i < val; ++i, ++*this) {}
        return *this;
    }

    BlockCursor &BlockCursor::operator++()
    {
        if (!checkDocument()) {
            return *this;
        }
        else {
        }

        resetNpos();

        bool end_of_current_block_reached =
            pos >= std::next(document->blocks.begin(), block_nr)->length() + last_char_inclusive;
        bool last_document_reached = block_nr + 1 == document->blocks.size();

        if (end_of_current_block_reached && last_document_reached) {
            return *this;
        }

        if (end_of_current_block_reached) {
            block_nr += 1;
            pos = 0;
            return *this;
        }

        pos += 1;
        return *this;
    }

    auto BlockCursor::operator-=(unsigned int val) -> BlockCursor &
    {
        // just use operator --
        for (unsigned int i = 0; i < val; ++i, --*this) {}
        return *this;
    }

    BlockCursor &BlockCursor::operator--()
    {
        if (!checkDocument()) {
            return *this;
        }

        resetNpos();

        if (pos == 0 && block_nr == 0) {
            return *this;
        }

        if (pos == 0) {
            block_nr -= 1;
            pos = std::next(document->blocks.begin(), block_nr)->length() + last_char_inclusive;
            return *this;
        }

        pos -= 1;
        return *this;
    }

    void BlockCursor::addChar(uint32_t utf_val)
    {
        if (document->blocks.size() == 0) {
            if (utf_val == text::newline) {
                document->append(TextBlock("", default_font, TextBlock::End::Newline));
                block_nr = 0;
                pos      = 0;
                return;
            }
            document->append(TextBlock("", default_font, TextBlock::End::None));
            block_nr = 0;
            pos      = 0;
        }
        auto block = curentBlock();
        if (block == blocksEnd()) {
            LOG_ERROR("add char to document with no text blocks shouldn't ever happen");
            return;
        }
        if (utf_val == text::newline) {
            document->addNewline(*this, TextBlock::End::Newline);
            // now we point to new block after newline was added
            ++block_nr;
            pos = 0;
            return;
        }
        block->addChar(utf_val, pos);
    }

    bool BlockCursor::removeChar()
    {
        auto block = curentBlock();
        if (block == blocksEnd()) {
            LOG_ERROR("removing char from document with no TextBlocks");
            return false;
        }

        debug_cursor("From block: [%d], remove pos: [%d] block length [%d]", getBlockNr(), pos, block->length());
        if (block_nr == text::npos) {
            return false;
        }

        block->removeChar(pos);
        if (block->isEmpty()) {
            debug_cursor("empty block removed");
            document->removeBlock(block);
        }
        return true;
    }
} // namespace gui
