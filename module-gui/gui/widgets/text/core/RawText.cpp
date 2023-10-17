// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "RawText.hpp"
#include <DrawCommand.hpp>
#include <TextConstants.hpp>

namespace gui
{
    RawText::RawText(UTF8 text, RawFont *font, Color color) : Item()
    {
        this->text  = text;
        this->font  = font;
        this->color = color;

        widgetArea.w = this->font->getPixelWidth(text);
        widgetArea.h = this->font->info.line_height;
    }

    UTF8 RawText::stripNewlineToDraw()
    {
        auto textToDraw = text;
        if (textToDraw.length() > 0 && textToDraw[textToDraw.length() - 1] == text::newline) {
            textToDraw.removeChar(textToDraw.length() - 1);
        }

        return textToDraw;
    }

    void RawText::buildDrawListImplementation(std::list<Command> &commands)
    {
        if (font) {
            auto cmd = std::make_unique<DrawText>();

            cmd->str    = stripNewlineToDraw();
            cmd->fontID = font->id;
            cmd->color  = color;

            cmd->origin     = {drawArea.x, drawArea.y};
            cmd->width      = drawArea.w;
            cmd->height     = drawArea.h;
            cmd->textOrigin = {0, static_cast<Position>(this->font->info.base)};
            cmd->textHeight = widgetArea.h;

            cmd->areaX = widgetArea.x;
            cmd->areaY = widgetArea.y;
            cmd->areaW = widgetArea.w;
            cmd->areaH = widgetArea.h;

            commands.emplace_back(std::move(cmd));
        }
    }

} // namespace gui
