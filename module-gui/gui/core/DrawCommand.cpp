// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "DrawCommand.hpp"
#include "Common.hpp"

// gui components
#include "Color.hpp"
#include "Context.hpp"
#include "ImageManager.hpp"
// renderers
#include "renderers/LineRenderer.hpp"
#include "renderers/ArcRenderer.hpp"
#include "renderers/CircleRenderer.hpp"
#include "renderers/RectangleRenderer.hpp"
#include <renderers/PixelRenderer.hpp>
// text rendering
#include "FontManager.hpp"
#include "RawFont.hpp"
// utils
#include <log/log.hpp>
// module-utils
#include <cassert>

#if DEBUG_FONT == 1
#define log_warn_glyph(...) LOG_WARN(__VA_ARGS__)
#else
#define log_warn_glyph(...)
#endif

namespace gui
{
    void Clear::draw(Context *ctx) const
    {
        ctx->fill(renderer::PixelRenderer::getColor(gui::ColorFullWhite.intensity));
    }

    void DrawLine::draw(Context *ctx) const
    {
        renderer::LineRenderer::draw(ctx, start, end, color);
    }

    void DrawRectangle::draw(Context *ctx) const
    {
        using renderer::RectangleRenderer;

        // First, check if there is anything to draw
        if (width == 0 || height == 0) {
            return;
        }
        if (fillColor.alpha == Color::FullTransparent && borderColor.alpha == Color::FullTransparent) {
            return;
        }
        if (!filled && borderColor.alpha == Color::FullTransparent) {
            return;
        }

        Context tempContext;
        Context *drawingContext = ctx;
        Point position(0, 0);

        if (yaps & (RectangleYap::BottomLeft | RectangleYap::TopLeft)) {
            position.x += yapSize;
        }

        Length adjustedWidth  = areaW;
        Length adjustedHeight = areaH;
        if (yaps != RectangleYap::None) {
            adjustedWidth -= yapSize;
        }

        if (areaW == width && areaH == height) {
            position.x += origin.x;
            position.y += origin.y;
        }
        else {
            const auto xCtx = areaX < 0 ? origin.x + areaX : origin.x;
            const auto yCtx = areaY < 0 ? origin.y + areaY : origin.y;
            tempContext     = ctx->get(xCtx, yCtx, areaW, areaH);
            drawingContext  = &tempContext;
        }

        if (radius == 0) {
            RectangleRenderer::drawFlat(
                drawingContext, position, adjustedWidth, adjustedHeight, RectangleRenderer::DrawableStyle::from(*this));
        }
        else {
            RectangleRenderer::draw(
                drawingContext, position, adjustedWidth, adjustedHeight, RectangleRenderer::DrawableStyle::from(*this));
        }

        if (drawingContext != ctx) {
            ctx->insertArea(origin.x, origin.y, areaX, areaY, width, height, tempContext);
        }
    }

    void DrawArc::draw(Context *ctx) const
    {
        renderer::ArcRenderer::draw(
            ctx, center, radius, start, sweep, renderer::ArcRenderer::DrawableStyle::from(*this));
    }

    void DrawCircle::draw(Context *ctx) const
    {
        renderer::CircleRenderer::draw(ctx, center, radius, renderer::CircleRenderer::DrawableStyle::from(*this));
    }

    void DrawText::drawChar(Context *ctx, const Point glyphOrigin, FontGlyph *glyph) const
    {
        auto *glyphPtr = glyph->data - glyphOrigin.x;
        assert(glyph->data);

        Point position           = glyphOrigin;
        const Position glyphMaxY = glyphOrigin.y - glyph->yoffset + glyph->height;
        const Position glyphMaxX = glyphOrigin.x + glyph->width;

        for (position.y = glyphOrigin.y - glyph->yoffset; position.y < glyphMaxY; ++position.y) {
            for (position.x = glyphOrigin.x; position.x < glyphMaxX; ++position.x) {
                if (!ctx->hasPixel(position)) {
                    log_warn_glyph(
                        "drawing out of: {x=%d,y=%d} vs {w=%d,h=%d}", position.x, position.y, ctx->getW(), ctx->getH());
                    return;
                }

                if (*(glyphPtr + position.x) == ColorFullBlack.intensity) {
                    renderer::PixelRenderer::draw(ctx, position, color);
                }
            }
            glyphPtr += glyph->width;
        }
    }

    void DrawText::draw(Context *ctx) const
    {
        // check if there are any characters to draw in the string provided with message.
        if (str.length() == 0) {
            return;
        }

        // retrieve font used to draw text
        const auto font = FontManager::getInstance().getFont(fontID);

        // draw every sign
        uint32_t idLast = 0, idCurrent = 0;
        Point position = textOrigin;

        for (uint32_t i = 0; i < str.length(); ++i) {
            idCurrent        = str[i]; // id stands for glued together utf-16 with no order bytes (0xFF 0xFE)
            const auto glyph = font->getGlyph(idCurrent);

            const auto xDrawingPosition = origin.x + position.x + glyph->xoffset;
            const auto yDrawingPosition = origin.y + position.y;

            // do not start drawing outside of draw context.
            if ((xDrawingPosition >= ctx->getW()) || (xDrawingPosition < 0)) {
                LOG_ERROR("Drawing outside context's X boundary for glyph: %" PRIu32, glyph->id);
                return;
            }
            if ((yDrawingPosition >= ctx->getH()) || (yDrawingPosition < 0)) {
                LOG_ERROR("Drawing outside context's Y boundary for glyph: %" PRIu32, glyph->id);
                return;
            }

            int32_t kernValue = 0;
            if (i > 0) {
                kernValue = font->getKerning(idLast, idCurrent);
            }

            drawChar(ctx, {xDrawingPosition + kernValue, yDrawingPosition}, glyph);
            position.x += glyph->xadvance + kernValue;

            idLast = idCurrent;
        }
    }

    inline void DrawImage::checkImageSize(Context *ctx, ImageMap *image) const
    {
        if (image->getHeight() > ctx->getH() || image->getWidth() > ctx->getW()) {
            LOG_WARN("image %s {w: %d,h %d} > context {w %d,h %d}",
                     image->getName().c_str(),
                     image->getWidth(),
                     ctx->getW(),
                     image->getHeight(),
                     ctx->getH());
        }
    }

    void DrawImage::drawPixMap(Context *ctx, PixMap *pixMap) const
    {
        uint32_t offsetImage   = 0;
        uint32_t offsetContext = 0;
        uint8_t *pixData       = pixMap->getData();
        const auto ctxData     = ctx->getData();
        checkImageSize(ctx, pixMap);

        for (uint32_t row = 0; row < std::min(ctx->getH(), pixMap->getHeight()); row++) {
            std::memcpy(ctxData + offsetContext, pixData + offsetImage, std::min(ctx->getW(), pixMap->getWidth()));
            offsetImage += pixMap->getWidth();
            offsetContext += ctx->getW();
        }
    }

    void DrawImage::drawVecMap(Context *ctx, VecMap *vecMap) const
    {
        uint32_t offsetContext    = 0;
        uint32_t offsetRowContext = 0;
        uint32_t imageOffset      = 0;
        uint8_t alphaColor        = vecMap->getAlphaColor();

        for (uint32_t row = 0; row < std::min(vecMap->getHeight(), ctx->getH()); row++) {
            checkImageSize(ctx, vecMap);
            uint16_t vecCount = *(vecMap->getData() + imageOffset);
            imageOffset += sizeof(uint16_t);

            const auto ctxData = ctx->getData();
            offsetRowContext   = offsetContext;

            for (uint32_t vec = 0; vec < vecCount; ++vec) {

                uint16_t vecOffset = *(vecMap->getData() + imageOffset);
                imageOffset += sizeof(uint16_t);
                uint16_t vecLength = *(vecMap->getData() + imageOffset);
                imageOffset += sizeof(uint8_t);
                uint8_t vecColor = *(vecMap->getData() + imageOffset);
                imageOffset += sizeof(uint8_t);

                offsetRowContext += vecOffset;
                if (vecColor != alphaColor) {
                    std::memset(ctxData + offsetRowContext,
                                renderer::PixelRenderer::getColor(vecColor),
                                std::min(ctx->getW(), vecLength));
                }
                offsetRowContext += vecLength;
            }
            offsetContext += ctx->getW();
        }
    }

    void DrawImage::draw(Context *ctx) const
    {
        // retrieve pixmap from the pixmap manager
        ImageMap *imageMap = ImageManager::getInstance().getImageMap(imageID);

        // if image is not found return;
        if (imageMap == nullptr) {
            return;
        }

        // get copy of original context using x,y of draw coordinates and original size of the widget
        Context drawCtx = ctx->get(origin.x, origin.y, areaW, areaH);

        if (imageMap->getType() == gui::ImageMap::Type::PIXMAP) {
            auto pixMap = dynamic_cast<PixMap *>(imageMap);
            assert(pixMap);
            drawPixMap(&drawCtx, pixMap);
        }
        else if (imageMap->getType() == gui::ImageMap::Type::VECMAP) {
            auto vecMap = dynamic_cast<VecMap *>(imageMap);
            assert(vecMap);
            drawVecMap(&drawCtx, vecMap);
        }

        // reinsert drawCtx into bast context
        ctx->insert(origin.x, origin.y, drawCtx);
    }
} /* namespace gui */
