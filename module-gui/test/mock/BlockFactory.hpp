// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "mock/InitializedFontManager.hpp"
#include <module-gui/gui/widgets/TextBlock.hpp>
#include <list>
#include <map>

namespace mockup
{
    class BlockFactory
    {

      public:
        enum class Type
        {
            Block0,       /// one hard newline
            Block1,       /// one hard newline - different text
            NoneBlock0,   // hard line at the end
            NoneBlock1,   // hard line in the middle
            VariableSize, // block with variable sizes elements
        };

        static auto get() -> BlockFactory *
        {
            static BlockFactory *pt;
            if (pt == nullptr) {
                pt = new BlockFactory();
            }
            return pt;
        }

        std::map<Type, std::list<gui::TextBlock>> blocks = {
            {Type::Block0,
             {
                 gui::TextBlock("Line 0", mockup::fontManager().getFont(0), gui::TextBlock::End::None),
                 gui::TextBlock("word 1", mockup::fontManager().getFont(0), gui::TextBlock::End::None),
                 gui::TextBlock("word 2", mockup::fontManager().getFont(0), gui::TextBlock::End::None),
                 gui::TextBlock("word 3", mockup::fontManager().getFont(0), gui::TextBlock::End::None),
                 gui::TextBlock("word 3", mockup::fontManager().getFont(0), gui::TextBlock::End::Newline),
             }},

            {Type::Block1,
             {
                 gui::TextBlock("Line 0", mockup::fontManager().getFont(1), gui::TextBlock::End::None),
                 gui::TextBlock(" word 1", mockup::fontManager().getFont(1), gui::TextBlock::End::None),
                 gui::TextBlock(" word 2", mockup::fontManager().getFont(1), gui::TextBlock::End::None),
                 gui::TextBlock(" word 3", mockup::fontManager().getFont(1), gui::TextBlock::End::None),
                 gui::TextBlock(" word 3", mockup::fontManager().getFont(1), gui::TextBlock::End::Newline),
             }},

            {Type::NoneBlock0,
             {
                 gui::TextBlock("None line 0", nullptr, gui::TextBlock::End::None),
                 gui::TextBlock("None line 1", nullptr, gui::TextBlock::End::Newline),
             }},

            {Type::NoneBlock1,
             {
                 gui::TextBlock("None line 0", nullptr, gui::TextBlock::End::Newline),
                 gui::TextBlock("None line 1", nullptr, gui::TextBlock::End::None),
             }},
            {Type::VariableSize,
             {
                 gui::TextBlock("Line 0", mockup::fontManager().getFont(1), gui::TextBlock::End::None),
                 gui::TextBlock(" word 1", mockup::fontManager().getFont(2), gui::TextBlock::End::None),
                 gui::TextBlock(" word 2", mockup::fontManager().getFont(3), gui::TextBlock::End::None),
                 gui::TextBlock(" word 3", mockup::fontManager().getFont(4), gui::TextBlock::End::None),
                 gui::TextBlock(" word 3", mockup::fontManager().getFont(5), gui::TextBlock::End::Newline),
             }}};
    };

    inline auto getBlock(BlockFactory::Type type)
    {
        return BlockFactory::get()->blocks[type];
    }
} // namespace mockup
