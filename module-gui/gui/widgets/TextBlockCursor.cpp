#include "TextBlockCursor.hpp"
#include "TextBlock.hpp"
#include "TextDocument.hpp"
#include "TextParse.hpp"
#include "log/log.hpp"

static const int last_char_inclusive = 0; // if then -1 / else 0

#define debug_cursor(...)
// #define debug_cursor(...) LOG_DEBUG(__VA_ARGS__)

namespace gui
{
    auto BlockCursor::currentBlock() const
    {
        if (block_nr == text::npos) {
            return document->blocks.end();
        }
        return std::next(document->blocks.begin(), block_nr);
    }

    auto BlockCursor::blocksEnd() const
    {
        return std::end(document->blocks);
    }

    auto BlockCursor::blocksBegin() const
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

    auto BlockCursor::atEol() const -> bool
    {
        if (!checkDocument() || checkNpos()) {
            return false;
        }

        auto block = currentBlock();
        // check if we have jumped to new block because of new line
        if (pos == 0 && block != blocksBegin() && block_jump) {
            return (--block)->getEnd() == TextBlock::End::Newline;
        }

        return pos == block->length() && block->getEnd() == TextBlock::End::Newline;
    }

    auto BlockCursor::operator+=(unsigned int val) -> BlockCursor &
    {
        // just use operator ++
        for (unsigned int i = 0; i < val; i++) {
            ++*this;
        }

        return *this;
    }

    BlockCursor &BlockCursor::operator++()
    {
        if (!checkDocument()) {
            return *this;
        }

        resetNpos();

        auto block = std::next(document->blocks.begin(), block_nr);

        size_t block_size = std::next(document->blocks.begin(), block_nr)->length();
        block_size += (block->getEnd() != TextBlock::End::Newline ? last_char_inclusive : -1);

        bool end_of_current_block_reached = pos >= block_size;
        bool last_document_reached        = block_nr + 1 == document->blocks.size();

        if (end_of_current_block_reached && last_document_reached) {
            return *this;
        }

        block_jump = end_of_current_block_reached;

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
            pos = currentBlock()->length() + last_char_inclusive;
            if (atEol()) {
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
            resetNpos();
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
            resetNpos();
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

    auto BlockCursor::getText() -> std::string
    {
        if (currentBlock() == blocksEnd()) {
            return "";
        }
        return currentBlock()->getText(getPosition());
    }

    auto BlockCursor::getUTF8Text() -> UTF8
    {
        if (currentBlock() == blocksEnd()) {
            return "";
        }
        return currentBlock()->getText(getPosition());
    }

    const TextBlock *BlockCursor::operator->()
    {
        return &*currentBlock();
    }

    void BlockCursor::resetJumps()
    {
        block_jump = false;
    }

    auto BlockCursor::begin() -> std::list<TextBlock>::iterator
    {
        return document == nullptr ? document->blocks.end() : document->blocks.begin();
    }

    auto BlockCursor::end() -> std::list<TextBlock>::iterator
    {
        return document->blocks.end();
    }
} // namespace gui
