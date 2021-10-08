// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "TextDocument.hpp"
#include <cassert>
#include <utility>
#include <TextFormat.hpp>

namespace gui
{

    const std::string TextDocument::newline = "\n";

    TextDocument::TextDocument(const std::list<TextBlock> &blocks) : blocks(std::move(blocks))
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
        assert(cursor.getBlockNumber() < blocks.size());
        auto [l_block, r_block] = split(cursor);
        l_block.setEnd(eol);

        // If we split last block in Document we set its end to None.
        if (cursor.getBlockNumber() == blocks.size() - 2) {
            r_block.setEnd(TextBlock::End::None);
        }
    }

    auto TextDocument::getText() const -> UTF8
    {
        UTF8 output;
        if (!blocks.empty()) {
            for (auto &el : blocks) {
                output += el.getText();
            }
        }

        return output;
    }

    auto TextDocument::getBlockCursor(unsigned int position) -> BlockCursor
    {
        unsigned int blockNumber  = 0;
        unsigned int loopPosition = 0;

        for (auto &el : blocks) {

            if (el.length() == 0) {
                return BlockCursor(this, 0, blockNumber, el.getFormat()->getFont());
            }

            if (loopPosition + el.length() > position) { // data found
                return BlockCursor(this, position - loopPosition, blockNumber, el.getFormat()->getFont());
            }

            // data not found in block_number, early exit,
            loopPosition += el.length();
            ++blockNumber;
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
        assert(cursor.getBlockNumber() < blocks.size());
        return *std::next(blocks.begin(), cursor.getBlockNumber());
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
        auto toSplit = std::next(blocks.begin(), cursor.getBlockNumber());

        auto text = toSplit->getText(cursor.getPosition());
        auto font = toSplit->getFormat()->getFont();
        auto end  = toSplit->getEnd();

        auto newBlock = TextBlock(text, font, end);

        toSplit->setText(toSplit->getText().substr(0, cursor.getPosition()));
        blocks.insert(std::next(toSplit), std::move(newBlock));

        return {*toSplit, *(std::next(toSplit))};
    }

} // namespace gui
