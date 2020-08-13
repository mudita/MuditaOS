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
    auto BlockCursor::currentBlock()
    {
        if (block_nr == text::npos) {
            return std::end(document->blocks);
        }
        return std::next(document->blocks.begin(), block_nr);
    }

    auto BlockCursor::blocksEnd()
    {
        return std::end(document->blocks);
    }

    auto BlockCursor::blocksBegin()
    {
        return std::begin(document->blocks);
    }

    BlockCursor::BlockCursor(TextDocument *document, unsigned int pos, unsigned int block_nr, RawFont *default_font)
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
        auto lastBlock = document->blocks.back();
        return block_nr == document->blocks.size() - 1 &&
               pos >= lastBlock.length() + (lastBlock.getEnd() != TextBlock::End::Newline ? last_char_inclusive : -1);
        ;
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

        auto block = std::next(document->blocks.begin(), block_nr);

        bool end_of_current_block_reached =
            pos >= std::next(document->blocks.begin(), block_nr)->length() +
                       (block->getEnd() != TextBlock::End::Newline ? last_char_inclusive : -1);
        ;
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
            auto block = std::next(document->blocks.begin(), block_nr);
            pos        = block->length() + last_char_inclusive;
            if (block->getEnd() == TextBlock::End::Newline) {
                pos -= 1;
            }
            return *this;
        }

        pos -= 1;

        return *this;
    } // namespace gui

    void BlockCursor::addChar(uint32_t utf_val)
    {
        if (document->blocks.size() == 0) {
            if (utf_val == text::newline) {

                /*
                 * Adding new line to empty document means adding new block with only new line and imidiatelly jumps to
                 * another block
                 */
                document->append(TextBlock("", default_font, TextBlock::End::Newline));
                document->append(TextBlock("", default_font, TextBlock::End::None));

                pos      = 0;
                block_nr = 1;

                return;
            }
            document->append(TextBlock("", default_font, TextBlock::End::None));
            block_nr = 0;
            pos      = 0;
        }
        auto block = currentBlock();
        if (block == blocksEnd()) {
            LOG_ERROR("add char to document with no text blocks shouldn't ever happen");
            return;
        }
        if (utf_val == text::newline) {
            document->addNewline(*this, TextBlock::End::Newline);
            return;
        }
        block->addChar(utf_val, pos);
    }

    void BlockCursor::addTextBlock(TextBlock &&textblock)
    {
        if (textblock.length() == 0) {
            return;
        }
        if (document->isEmpty()) {
            block_nr = 0;
            pos      = 0;
        }
        document->append(std::move(textblock));
    }

    bool BlockCursor::removeChar()
    {
        if (checkNpos()) {
            LOG_ERROR("cant remove from not initialized/empty cursor");
            return false;
        }

        auto block     = currentBlock();
        auto prevBlock = block;
        auto nextBlock = block;

        if (block != blocksBegin()) {
            prevBlock--;
        }
        if (block != blocksEnd()) {
            nextBlock++;
        }

        if (block == blocksEnd()) {
            LOG_ERROR("removing char from document with no TextBlocks");
            return false;
        }

        if (nextBlock != blocksEnd() && nextBlock->isEmpty() && block->getEnd() == TextBlock::End::Newline) {
            document->removeBlock(nextBlock);
            block->removeChar(pos);
            return true;
        }

        debug_cursor("From block: [%d], remove pos: [%d] block length [%d]", getBlockNr(), pos, block->length());
        block->removeChar(pos);

        if (block->isEmpty() && block != blocksEnd() && prevBlock->getEnd() != TextBlock::End::Newline) {
            debug_cursor("empty block removed");
            document->removeBlock(block);
        }

        return true;
    }

    const TextBlock &BlockCursor::operator*()
    {
        return *currentBlock();
    }
} // namespace gui
