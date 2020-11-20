// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "TextBlockCursor.hpp"
#include "TextBlock.hpp"
#include "TextDocument.hpp"
#include "TextParse.hpp"
#include "log/log.hpp"
#include <cassert>

static const int last_char_inclusive = 0; // if then -1 / else 0

#define debug_cursor(...)
// #define debug_cursor(...) LOG_DEBUG(__VA_ARGS__)

namespace gui
{
    auto BlockCursor::currentBlock() const -> std::_List_iterator<TextBlock>
    {
        if (currentBlockNr == text::npos) {
            return document->blocks.end();
        }
        return std::next(document->blocks.begin(), currentBlockNr);
    }

    auto BlockCursor::blocksEnd() const -> std::_List_iterator<TextBlock>
    {
        return std::end(document->blocks);
    }

    auto BlockCursor::blocksBegin() const -> std::_List_iterator<TextBlock>
    {
        return std::begin(document->blocks);
    }

    BlockCursor::BlockCursor(TextDocument *document, unsigned int pos, unsigned int block_nr, RawFont *default_font)
        : document(document), default_font(default_font)
    {
        this->pos            = text::npos;
        this->currentBlockNr = text::npos;
        if (checkDocument()) {
            this->currentBlockNr = block_nr < document->blocks.size() ? block_nr : document->blocks.size() - 1;
            auto block_len       = std::next(document->blocks.begin(), this->currentBlockNr)->length();
            this->pos            = pos < block_len ? pos : block_len + last_char_inclusive;
        }
    }

    [[nodiscard]] auto BlockCursor::checkNpos() const -> bool
    {
        return pos == text::npos || currentBlockNr == text::npos;
    }

    void BlockCursor::resetNpos()
    {
        if (pos == text::npos)
            pos = 0;
        if (currentBlockNr == text::npos)
            currentBlockNr = 0;
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

        return currentBlockNr == document->blocks.size() - 1 &&
               pos >= lastBlock.length() + (lastBlock.getEnd() != TextBlock::End::Newline ? last_char_inclusive : -1);
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

        bool endOfCurrentBlockReached = false;

        debug_cursor("Addition: Position: %d, Current block number %d", pos, currentBlockNr);

        size_t blockSize = currentBlock()->length();

        // Skip newline at end
        if (currentBlock()->getEnd() == TextBlock::End::Newline && (pos + 1) == blockSize) {
            endOfCurrentBlockReached = true;
        }
        else if (pos >= blockSize) {
            endOfCurrentBlockReached = true;
        }

        bool lastBlockReached = currentBlockNr + 1 == document->blocks.size();

        if (endOfCurrentBlockReached && lastBlockReached) {
            return *this;
        }

        if (endOfCurrentBlockReached) {
            return goToNextBlock();
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

        debug_cursor("Subtraction: Position: %d, Current block number %d", pos, currentBlockNr);

        if (pos == 0 && currentBlockNr == 0) {
            return *this;
        }

        if (pos == 0) {
            return goToPreviousBlock();
        }

        pos -= 1;

        return *this;
    } // namespace gui

    auto BlockCursor::goToNextBlock() -> BlockCursor &
    {
        if (currentBlockNr < document->getBlocks().size()) {
            blockChanged = true;
            currentBlockNr += 1;
            pos = 0;

            debug_cursor("Addition block changed: Position: %d, Current block number %d", pos, currentBlockNr);
        }

        return *this;
    }

    auto BlockCursor::goToPreviousBlock() -> BlockCursor &
    {
        if (currentBlockNr > 0) {
            blockChanged = true;
            currentBlockNr -= 1;
            pos = currentBlock()->length() +
                  (currentBlock()->getEnd() != TextBlock::End::Newline ? last_char_inclusive : -1);

            debug_cursor("Subtraction block changed: Position: %d, Current block number %d", pos, currentBlockNr);
        }
        return *this;
    }

    void BlockCursor::addChar(uint32_t utf_val)
    {
        if (document->blocks.size() == 0) {
            if (utf_val == text::newline) {

                /*
                 * Adding new line to empty document means adding new block with only new line and immediately jumps to
                 * another block
                 */
                document->append(TextBlock("", default_font, TextBlock::End::None));
                operator++();
                document->addNewline(*this, TextBlock::End::Newline);

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

        if (textblock.length() == 0 && document->blocks.empty() && textblock.getEnd() != TextBlock::End::Newline) {
            return;
        }
        else if (textblock.length() == 0 && ((--blocksEnd())->getEnd() != TextBlock::End::Newline)) {
            return;
        }

        if (document->isEmpty()) {
            resetNpos();
        }

        // If previously added block ended with newline remove empty added block.
        if (!document->blocks.empty()) {
            if ((--blocksEnd())->length() == ((--blocksEnd())->getEnd() == TextBlock::End::Newline ? 1 : 0)) {
                document->removeBlock(--blocksEnd());
            }
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

        if (nextBlock != blocksEnd() && nextBlock->isEmpty() && nextBlock != (--blocksEnd())) {
            debug_cursor("Next empty block removed");
            document->removeBlock(nextBlock);
        }

        debug_cursor("From block: [%d], remove pos: [%d] block length [%d], Block count [%lu]",
                     getBlockNr(),
                     pos,
                     block->length(),
                     document->blocks.size());

        block->removeChar(pos);

        if (block->isEmpty() && block != blocksEnd() && prevBlock->getEnd() != TextBlock::End::Newline) {
            debug_cursor("Current empty block removed");
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

    auto BlockCursor::begin() -> std::list<TextBlock>::iterator
    {
        return document == nullptr ? document->blocks.end() : document->blocks.begin();
    }

    auto BlockCursor::end() -> std::list<TextBlock>::iterator
    {
        return document->blocks.end();
    }

} // namespace gui
