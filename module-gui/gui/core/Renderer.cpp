// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

extern "C"
{
#include "FreeRTOS.h"
#include "task.h"
}

// for memset
#include <string.h>
#include "Color.hpp"
#include "Renderer.hpp"
#include "Context.hpp"
#include "ImageManager.hpp"
#include "../Common.hpp"
// renderers
#include "renderers/LineRenderer.hpp"
#include "renderers/ArcRenderer.hpp"
#include "renderers/CircleRenderer.hpp"
#include "renderers/RectangleRenderer.hpp"
// utils
#include "log/log.hpp"

#include "PixMap.hpp"
#include "VecMap.hpp"
// module-utils
#include "utf8/UTF8.hpp"
#include <cmath>
#include <cassert>
#include <FontManager.hpp>
#include <FontGlyph.hpp>
#include <RawFont.hpp>
#include "Style.hpp"
#include "Common.hpp"

#if DEBUG_FONT == 1
#define log_warn_glyph(...) LOG_WARN(__VA_ARGS__)
#else
#define log_warn_glyph(...)
#endif

namespace gui
{
    void Renderer::drawLine(Context *ctx, CommandLine *cmd)
    {
        using renderer::LineRenderer;
        const Point p1{cmd->x1, cmd->y1};
        const Point p2{cmd->x2, cmd->y2};
        const auto color = cmd->color;
        LineRenderer::draw(ctx, p1, p2, color);
    }

    void Renderer::drawRectangle(Context *ctx, CommandRectangle *cmd)
    {
        using renderer::RectangleRenderer;

        // First, check if there is anything to draw
        if (cmd->w == 0 || cmd->h == 0) {
            return;
        }
        if (cmd->fillColor.alpha == Color::FullTransparent && cmd->borderColor.alpha == Color::FullTransparent) {
            return;
        }
        if (!cmd->filled && cmd->borderColor.alpha == Color::FullTransparent) {
            return;
        }

        Context *drawingContext = ctx;
        Point position;
        if (cmd->yaps & (RectangleYap::BottomLeft | RectangleYap::TopLeft)) {
            position.x += cmd->yapSize;
        }
        Length width  = cmd->areaW;
        Length height = cmd->areaH;
        if (cmd->yaps != RectangleYap::None) {
            width -= cmd->yapSize;
        }

        if (cmd->areaW == cmd->w && cmd->areaH == cmd->h) {
            position.x += cmd->x;
            position.y += cmd->y;
        }
        else {
            const auto x   = cmd->areaX < 0 ? cmd->x + cmd->areaX : cmd->x;
            const auto y   = cmd->areaY < 0 ? cmd->y + cmd->areaY : cmd->y;
            drawingContext = ctx->get(x, y, cmd->areaW, cmd->areaH);
        }

        Length radius = cmd->radius;
        if (radius == 0) {
            RectangleRenderer::drawFlat(
                drawingContext, position, width, height, RectangleRenderer::DrawableStyle::from(*cmd));
        }
        else {
            RectangleRenderer::draw(
                drawingContext, position, width, height, RectangleRenderer::DrawableStyle::from(*cmd));
        }

        if (drawingContext != ctx) {
            ctx->insertArea(cmd->x, cmd->y, cmd->areaX, cmd->areaY, cmd->w, cmd->h, drawingContext);
            delete drawingContext;
        }
    }

    void Renderer::drawArc(Context *ctx, CommandArc *cmd)
    {
        using renderer::ArcRenderer;
        const auto center{cmd->center};
        const auto radius{cmd->radius};
        const auto startAngle{cmd->start};
        const auto sweepAngle{cmd->sweep};
        const auto style = ArcRenderer::DrawableStyle::from(*cmd);
        ArcRenderer::draw(ctx, center, radius, startAngle, sweepAngle, style);
    }

    void Renderer::drawCircle(Context *ctx, CommandCircle *cmd)
    {
        using renderer::CircleRenderer;
        const auto center{cmd->center};
        const auto radius{cmd->radius};
        const auto style = CircleRenderer::DrawableStyle::from(*cmd);
        CircleRenderer::draw(ctx, center, radius, style);
    }

    void Renderer::drawChar(Context *context, const int16_t x, const int16_t y, FontGlyph *glyph, const Color color)
    {
        auto line_y_offset = (y - glyph->yoffset) * context->getW();
        auto *drawPtr      = context->getData() + x + line_y_offset;
        auto *glyphPtr     = glyph->data;
        assert(glyph->data);

        for (uint16_t yy = 0; yy < glyph->height; ++yy) {
            for (uint16_t xx = 0; xx < glyph->width; ++xx) {
                if (!context->addressInData(drawPtr + xx)) {
                    log_warn_glyph("drawing out of: %d vs %d", xx, context->getW() * context->getH());
                    return;
                }
                if (*(glyphPtr + xx) == 0) {
                    *(drawPtr + xx) = 0x0F & color.intensity;
                }
            }
            drawPtr += context->getW();
            glyphPtr += glyph->width;
        }
    }

    void Renderer::drawText(Context *ctx, CommandText *cmd)
    {

        // check if there are any characters to draw in the string provided with message.
        if (cmd->str.length() == 0) {
            return;
        }

        // get copy of original context using x,y of draw coordinates and original size of the widget
        Context *drawCtx;
        bool copyContext = false;
        int16_t wgtX = 0, wgtY = 0;
        // check if there is a need or making copy of context to use it as background
        if ((cmd->areaW == cmd->w) && (cmd->areaH == cmd->h)) {
            drawCtx = ctx;
            wgtX    = cmd->x;
            wgtY    = cmd->y;
        }
        else {
            copyContext = true;
            drawCtx     = ctx->get(cmd->x, cmd->y, cmd->areaW, cmd->areaH);
        }

        // retrieve font used to draw text
        FontManager &fontManager = FontManager::getInstance();
        RawFont *font            = fontManager.getFont(cmd->fontID);

        int16_t posX = cmd->tx;
        int16_t posY = cmd->ty;

        // draw every sign
        uint32_t idLast = 0, idCurrent = 0;
        for (uint32_t i = 0; i < cmd->str.length(); ++i) {
            idCurrent        = cmd->str[i]; // id stands for glued together utf-16 with no order bytes (0xFF 0xFE)
            FontGlyph *glyph = font->getGlyph(idCurrent);

            // do not start drawing outside of draw context.
            if ((wgtX + posX + glyph->xoffset >= drawCtx->getW()) || (wgtX + posX + glyph->xoffset < 0)) {
                LOG_FATAL("Drawing outside context's X boundary for glyph: %" PRIu32, glyph->id);
                return;
            }
            if ((wgtY + posY >= drawCtx->getH()) || (wgtY + posY < 0)) {
                LOG_FATAL("Drawing outside context's Y boundary for glyph: %" PRIu32, glyph->id);
                return;
            }

            int32_t kernValue = 0;
            if (i > 0) {
                kernValue = font->getKerning(idLast, idCurrent);
            }
            drawChar(drawCtx, wgtX + posX + glyph->xoffset + kernValue, wgtY + posY, glyph, cmd->color);
            posX += glyph->xadvance + kernValue;

            idLast = idCurrent;
        }

        // if drawing was performed in temporary context
        // reinsert drawCtx into bast context
        if (copyContext) {
            ctx->insert(cmd->x, cmd->y, drawCtx);
            delete drawCtx;
        }
    }

    void Renderer::drawImage(Context *ctx, CommandImage *cmd)
    {

        // retrive pixmap from the pixmap manager
        ImageMap *imageMap = ImageManager::getInstance().getImageMap(cmd->imageID);

        // if image is not found return;
        if (imageMap == nullptr)
            return;

        // get copy of original context using x,y of draw coordinates and original size of the widget
        Context *drawCtx = ctx->get(cmd->x, cmd->y, cmd->areaW, cmd->areaH);
        uint8_t *ctxData = drawCtx->getData();

        auto check_wh = [&](const std::string &name, auto w, auto h) {
            if (h > ctx->getH() || w > ctx->getW()) {
                LOG_WARN("image %s {w: %d,h %d} > context {w %d,h %d}",
                         name.c_str(),
                         w,
                         drawCtx->getW(),
                         h,
                         drawCtx->getH());
            }
        };

        if (imageMap->getType() == gui::ImageMap::Type::PIXMAP) {
            auto pixMap = dynamic_cast<PixMap *>(imageMap);
            assert(pixMap);
            uint32_t offsetImage   = 0;
            uint32_t offsetContext = 0;
            uint8_t *pixData       = pixMap->getData();
            check_wh(pixMap->getName(), pixMap->getWidth(), pixMap->getHeight());

            for (uint32_t row = 0; row < std::min(drawCtx->getH(), pixMap->getHeight()); row++) {
                memcpy(ctxData + offsetContext, pixData + offsetImage, std::min(drawCtx->getW(), pixMap->getWidth()));
                offsetImage += pixMap->getWidth();
                offsetContext += drawCtx->getW();
            }
        }
        else if (imageMap->getType() == gui::ImageMap::Type::VECMAP) {
            auto vecMap = dynamic_cast<VecMap *>(imageMap);
            assert(vecMap);
            uint32_t offsetContext    = 0;
            uint32_t offsetRowContext = 0;
            uint32_t imageOffset      = 0;
            uint8_t alphaColor        = vecMap->getAlphaColor();
            for (uint32_t row = 0; row < std::min(vecMap->getHeight(), drawCtx->getH()); row++) {
                check_wh(vecMap->getName(), imageMap->getWidth(), imageMap->getHeight());
                uint16_t vecCount = *(vecMap->getData() + imageOffset);
                imageOffset += sizeof(uint16_t);

                offsetRowContext = offsetContext;

                for (uint32_t vec = 0; vec < vecCount; ++vec) {

                    uint16_t vecOffset = *(vecMap->getData() + imageOffset);
                    imageOffset += sizeof(uint16_t);
                    uint16_t vecLength = *(vecMap->getData() + imageOffset);
                    imageOffset += sizeof(uint8_t);
                    uint8_t vecColor = *(vecMap->getData() + imageOffset);
                    imageOffset += sizeof(uint8_t);

                    offsetRowContext += vecOffset;
                    if (vecColor != alphaColor) {
                        memset(ctxData + offsetRowContext, vecColor, std::min(drawCtx->getW(), vecLength));
                    }
                    offsetRowContext += vecLength;
                }
                offsetContext += drawCtx->getW();
            }
        }

        // reinsert drawCtx into bast context
        ctx->insert(cmd->x, cmd->y, drawCtx);

        // remove draw context
        delete drawCtx;
    }

    void Renderer::render(Context *ctx, Commands &commands)
    {
        if (ctx == nullptr) {
            return;
        }
        for (auto &cmd : commands) {
            if (cmd == nullptr) {
                continue;
            }
            switch (cmd->id) {
            case DrawCommandID::GUI_DRAW_CLEAR:
                ctx->fill(15);
                break;
            case DrawCommandID::GUI_DRAW_LINE:
                drawLine(ctx, static_cast<CommandLine *>(cmd.get()));
                break;
            case DrawCommandID::GUI_DRAW_RECT:
                drawRectangle(ctx, static_cast<CommandRectangle *>(cmd.get()));
                break;
            case DrawCommandID::GUI_DRAW_ARC:
                drawArc(ctx, static_cast<CommandArc *>(cmd.get()));
                break;
            case DrawCommandID::GUI_DRAW_CIRCLE:
                drawCircle(ctx, static_cast<CommandCircle *>(cmd.get()));
                break;
            case DrawCommandID::GUI_DRAW_TEXT:
                drawText(ctx, static_cast<CommandText *>(cmd.get()));
                break;
            case DrawCommandID::GUI_DRAW_IMAGE:
                drawImage(ctx, static_cast<CommandImage *>(cmd.get()));
                break;
            default:
                break;
            };
        }
    }

} /* namespace gui */
