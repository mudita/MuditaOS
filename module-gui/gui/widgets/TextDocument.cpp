#include "TextDocument.hpp"
#include "log/log.hpp"
#include <cassert>
#include <utility>

namespace gui
{

    const std::string TextDocument::newline = "\n";

    TextDocument::TextDocument(const std::list<TextBlock> blocks) : blocks(std::move(blocks))
    {}

    TextDocument::~TextDocument()
    {
        destroy();
    }

    void TextDocument::destroy()
    {
        blocks.clear();
    }

    void TextDocument::append(std::list<TextBlock> &&blocks)
    {
        for (auto el : blocks) {
            this->blocks.emplace_back(el);
        }
    }

    void TextDocument::append(TextBlock &&text)
    {
        blocks.emplace_back(text);
    }

    void TextDocument::addNewline(BlockCursor &cursor, TextBlock::End eol)
    {
        assert(cursor.getBlockNr() < blocks.size());
        auto [l_block, r_block] = split(cursor);
        l_block.setEnd(eol);
    }

    UTF8 TextDocument::getText() const
    {
        UTF8 output;
        for (auto el : blocks) {
            output += el.getText();
        }

        return output;
    }

    BlockCursor TextDocument::getBlockCursor(unsigned int position)
    {
        unsigned int block_no      = 0;
        unsigned int loop_position = 0;
        for (auto el : blocks) {
            if (loop_position + el.length() > position) { // data found
                return BlockCursor(this, position - loop_position, block_no, el.getFont());
            }
            // data not found in block_number, early exit
            loop_position += el.length();
            ++block_no;
        }
        return BlockCursor();
    }

    TextPart TextDocument::getTextPart(BlockCursor cursor)
    {
        if (cursor) {
            auto block = std::next(blocks.begin(), cursor.getBlockNr());
            return TextPart(cursor, block->getText(cursor.getPosition()), block->getFont());
        }
        return TextPart();
    }

    [[nodiscard]] const std::list<TextBlock> &TextDocument::getBlocks() const
    {
        return blocks;
    }

    [[nodiscard]] const TextBlock *TextDocument::getBlock(BlockCursor *cursor) const
    {
        if (cursor != nullptr && *cursor) {
            return &operator()(*cursor);
        }
        return nullptr;
    }

    const TextBlock &TextDocument::operator()(const BlockCursor &cursor) const
    {
        assert(cursor.getBlockNr() < blocks.size());
        return *std::next(blocks.begin(), cursor.getBlockNr());
    }

    void TextDocument::removeBlock(unsigned int block_nr)
    {
        if (block_nr >= blocks.size() || block_nr == text::npos) {
            return;
        }
        blocks.erase(std::next(blocks.begin(), block_nr));
    }

    void TextDocument::removeBlock(std::list<TextBlock>::iterator it)
    {
        blocks.erase(it);
    }

    auto TextDocument::split(BlockCursor &cursor) -> std::pair<TextBlock &, TextBlock &>
    {
        auto to_split = std::next(blocks.begin(), cursor.getBlockNr());
        auto newblock = TextBlock(to_split->getText(cursor.getPosition()), to_split->getFont(), to_split->getEnd());
        to_split->setText(to_split->getText().substr(0, cursor.getPosition()));
        blocks.insert(std::next(to_split), std::move(newblock));
        return {*to_split, *(std::next(to_split))};
    }

} // namespace gui
