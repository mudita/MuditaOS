// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Item.hpp>
#include <RawFont.hpp>
#include <Color.hpp>
#include <utf8/UTF8.hpp>

namespace gui
{
    class RawText : public Item
    {
      private:
        Color color   = {0, 0};
        RawFont *font = nullptr;
        UTF8 text     = "";

        UTF8 stripNewlineToDraw();

      public:
        RawText(UTF8 text, RawFont *font, Color color);

        const UTF8 &getText() const noexcept
        {
            return text;
        }

        unsigned int getTextLength() const noexcept
        {
            return text.length();
        }

        RawFont *getFont() const noexcept
        {
            return font;
        }

        void buildDrawListImplementation(std::list<Command> &commands) override;
    };
} // namespace gui
