// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "TextDocument.hpp"
#include <cassert>
#include <utility>
#include <TextFormat.hpp>

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

    void TextDocument::append(std::list<TextBlock> &&block_list)
    {
        for (auto &&el : block_list) {
            this->blocks.emplace_back(std::move(el));
        }
    }

    void TextDocument::append(TextBlock &&text)
    {
        blocks.emplace_back(std::move(text));
    }

    void TextDocument::addNewline(BlockCursor &cursor, TextBlock::End eol)
    {
        assert(cursor.getBlockNr() < blocks.size());
        auto [l_block, r_block] = split(cursor);
        l_block.setEnd(eol);

        // If we split last block in Document we set its end to None.
        if (cursor.getBlockNr() == blocks.size() - 2) {
            r_block.setEnd(TextBlock::End::None);
        }
    }

    auto TextDocument::getText() const -> UTF8
    {
        UTF8 output;
        if (blocks.size() != 0) {
            for (auto &el : blocks) {
                output += el.getText();
            }
        }

        return output;
    }

    auto TextDocument::getBlockCursor(unsigned int position) -> BlockCursor
    {
        unsigned int block_no      = 0;
        unsigned int loop_position = 0;

        for (auto &el : blocks) {

            if (el.length() == 0) {
                return BlockCursor(this, 0, block_no, el.getFormat()->getFont());
            }

            if (loop_position + el.length() > position) { // data found
                return BlockCursor(this, position - loop_position, block_no, el.getFormat()->getFont());
            }

            // data not found in block_number, early exit,
            loop_position += el.length();
            ++block_no;
        }
        // TODO ok... here we might want to return BlockCursor(this) <- but returning text::npos / empty/none block if
        // we wanted to return anything
        return BlockCursor();
    }

    auto TextDocument::getText(BlockCursor cursor) -> std::string
    {
        if (cursor) {
            return cursor.getText();
        }
        return "";
    }

    [[nodiscard]] auto TextDocument::getBlocks() const -> const std::list<TextBlock> &
    {
        return blocks;
    }

    [[nodiscard]] auto TextDocument::getBlock(BlockCursor *cursor) const -> const TextBlock *
    {
        if (cursor != nullptr && *cursor) {
            return &operator()(*cursor);
        }
        return nullptr;
    }

    auto TextDocument::operator()(const BlockCursor &cursor) const -> const TextBlock &
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

        auto text = to_split->getText(cursor.getPosition());
        auto font = to_split->getFormat()->getFont();
        auto end  = to_split->getEnd();

        auto newblock = TextBlock(text, font, end);

        to_split->setText(to_split->getText().substr(0, cursor.getPosition()));
        blocks.insert(std::next(to_split), std::move(newblock));

        return {*to_split, *(std::next(to_split))};
    }

} // namespace gui
