#pragma once

#include <list>
#include <utility>
#include "TextBlock.hpp"
#include "TextBlockCursor.hpp"

namespace gui
{
    class TextLine;

    /// based on TextBlock - which is paragraph representation
    struct TextPart
    {
        TextPart(BlockCursor cursor, UTF8 text, Font *font) : cursor(cursor), text(std::move(text)), font(font)
        {}
        TextPart() = default;
        BlockCursor cursor;
        UTF8 text;
        Font *font = nullptr;
    };

    class TextDocument
    {
        friend BlockCursor;

        static const std::string newline;
        std::list<TextBlock> blocks;

      public:
        TextDocument(const std::list<TextBlock> blocks);
        ~TextDocument();
        void destroy();

        void append(std::list<TextBlock> &&blocks);
        void append(TextBlock &&text);
        void addNewline(BlockCursor &cursor, TextBlock::End eol);
        [[nodiscard]] auto getText() const -> UTF8;

        /// --- in progress
        BlockCursor getBlockCursor(unsigned int position);
        /// get part of TextBlock based on cursor
        TextPart getTextPart(BlockCursor cursor);

        /// needed for tests, alternatively could be mocked in test...
        [[nodiscard]] const std::list<TextBlock> &getBlocks() const;
        [[nodiscard]] const TextBlock *getBlock(BlockCursor *cursor) const;

        /// note just for creating line for easy current block under cursor access
        /// remove - use getTextPart instead?
        /// or remove getTextPart and always use cursor...
        const TextBlock &operator()(const BlockCursor &cursor) const;
        void removeBlock(unsigned int block_nr);
        void removeBlock(std::list<TextBlock>::iterator it);
        // TODO this is very unoptimal...
        bool isEmpty() const
        {
            return getText().length() == 0;
        }

      private:
        /// splits text block in document and returns two new blocks (in place of last one)
        auto split(BlockCursor &cursor) -> std::pair<TextBlock &, TextBlock &>;
    };
}; // namespace gui
