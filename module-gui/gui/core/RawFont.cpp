// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "RawFont.hpp"
#include "Common.hpp"
#include "Context.hpp"
#include "renderers/LineRenderer.hpp"
#include "renderers/RectangleRenderer.hpp"
#include "TextConstants.hpp"
#include <log/log.hpp>
#include "utf8/UTF8.hpp"
#include <cstring>
#include <utility>

namespace gui
{
    RawFont::~RawFont()
    {
        fallback_font = nullptr;
    }

    gui::Status RawFont::load(uint8_t *data)
    {
        std::uint32_t offset = 0;

        if (info.load(data, offset) != gui::Status::GUI_SUCCESS) {
            return gui::Status::GUI_FAILURE;
        }

        // number of glyphs in the font
        memcpy(&glyph_count, data + offset, sizeof(std::uint32_t));
        offset += sizeof(std::uint32_t);
        // offset to the beginning of the glyph data
        memcpy(&glyph_data_offset, data + offset, sizeof(std::uint32_t));
        offset += sizeof(std::uint32_t);
        // number of kerning pairs
        memcpy(&kern_count, data + offset, sizeof(std::uint32_t));
        offset += sizeof(std::uint32_t);
        // array of glyphs structures
        memcpy(&kern_data_offset, data + offset, sizeof(std::uint32_t));
        offset += sizeof(std::uint32_t);
        // offset to the beginning of the image data
        memcpy(&image_data_offset, data + offset, sizeof(std::uint32_t));
        offset += sizeof(std::uint32_t);
        // id of the font assigned by the font manager
        id = 1;

        // load glyphs
        std::uint32_t glyphOffset = glyph_data_offset;
        for (std::uint32_t i = 0; i < glyph_count; i++) {
            auto glyph = std::make_unique<FontGlyph>();
            glyph->load(data, glyphOffset);
            glyph->loadImage(data, glyph->glyph_offset);
            glyphs.insert(std::pair<std::uint32_t, std::unique_ptr<FontGlyph>>(glyph->id, std::move(glyph)));
        }

        // load kerning
        // first map contains index of the character and the map that holds values for kerning between
        // first and second character. In second map key is the value of the second character
        // and value is the kerning between first and second element.
        std::uint32_t kernOffset = kern_data_offset;
        for (std::uint32_t i = 0; i < kern_count; i++) {
            auto kern = std::make_unique<FontKerning>();
            kern->load(data, kernOffset);

            // find map using first element of kerning as a key
            const auto it = kerning.find(kern->first);

            // element wasn't found
            if (it == kerning.end()) {
                std::map<std::uint32_t, std::unique_ptr<FontKerning>> kernMap;
                const auto kern_first = kern->first;
                kernMap.emplace(std::make_pair(kern->second, std::move(kern)));

                // insert element to the first map
                kerning.emplace(std::make_pair(kern_first, std::move(kernMap)));
            }
            else {
                auto &kernMap = it->second;
                kernMap.emplace(std::make_pair(kern->second, std::move(kern)));
            }
        }

        createGlyphUnsupported();

        return gui::Status::GUI_SUCCESS;
    }

    int32_t RawFont::getKerning(std::uint32_t id1, std::uint32_t id2) const
    {
        if (id2 == none_char_id) {
            return 0;
        }
        // search for a map with kerning for given character (id1)
        const auto it1 = kerning.find(id1);

        // if there is no map with kerning for id1 return 0;
        if (it1 == kerning.end()) {
            return 0;
        }

        const auto &kernMap = it1->second;

        // otherwise search for id2 in kernMap and return value or 0 if it's not found
        const auto it2 = kernMap.find(id2);
        if (it2 == kernMap.end()) {
            return 0;
        }

        const auto &kern = it2->second;
        return kern->amount;
    }

    std::uint32_t RawFont::getCharCountInSpace(const UTF8 &str, const std::uint32_t space) const
    {
        std::uint32_t availableSpace = space;
        std::uint32_t count          = 0;
        std::uint32_t current        = 0;
        std::uint32_t previous       = none_char_id;

        for (std::uint32_t i = 0; i < str.length(); ++i, ++count) {
            current               = str[i];
            const auto char_pixel_width = getCharPixelWidth(current, previous);
            if (availableSpace < char_pixel_width) {
                return count;
            }
            availableSpace -= char_pixel_width;
            previous = current;
        }
        return count;
    }

    FontGlyph *RawFont::getGlyph(std::uint32_t glyph_id) const
    {
        auto glyph = this->findGlyph(glyph_id);
        if (glyph != nullptr) {
            return glyph;
        }
        glyph = findGlyphFallback(glyph_id);
        if (glyph != nullptr) {
            return glyph;
        }

        LOG_WARN("unsupported font glyph ID: %" PRIu32, glyph_id);
        return unsupported.get();
    }

    FontGlyph *RawFont::findGlyph(std::uint32_t glyph_id) const
    {
        const auto glyph_found = glyphs.find(glyph_id);
        if (glyph_found != glyphs.end()) {
            return glyph_found->second.get();
        }
        return nullptr;
    }

    FontGlyph *RawFont::findGlyphFallback(std::uint32_t glyph_id) const
    {
        if (fallback_font == nullptr) {
            return nullptr;
        }
        return fallback_font->findGlyph(glyph_id);
    }

    std::uint32_t RawFont::getPixelWidth(const UTF8 &str, const std::uint32_t start, const std::uint32_t count) const
    {
        if (count == 0) {
            return 0;
        }

        if (str.length() == 0) {
            return 0;
        }

        if ((start >= str.length()) || (start + count - 1 >= str.length())) {
            LOG_ERROR("incorrect string index provided");
            return 0;
        }

        // width of text in pixels
        std::uint32_t width     = 0;
        std::uint32_t idCurrent = 0;
        std::uint32_t idLast    = none_char_id;

        for (std::uint32_t i = 0; i < count; ++i) {
            idCurrent = str[start + i];
            width += getCharPixelWidth(idCurrent, idLast);
            idLast = idCurrent;
        }

        return width;
    }

    std::uint32_t RawFont::getPixelWidth(const UTF8 &str) const
    {
        return getPixelWidth(str, 0, str.length());
    }

    std::uint32_t RawFont::getCharPixelWidth(std::uint32_t charCode, std::uint32_t previousChar) const
    {
        if (charCode == text::newline) { // newline doesn't have width
            return 0;
        }
        const auto glyph = getGlyph(charCode);
        return glyph->xadvance + getKerning(charCode, previousChar);
    }

    std::uint32_t RawFont::getCharPixelHeight(std::uint32_t charCode)
    {
        const auto glyph = getGlyph(charCode);
        return glyph->height;
    }

    void RawFont::createGlyphUnsupported()
    {
        constexpr auto ptToPx                              = 0.75f;
        constexpr auto diagonalSpacingToRectangleEdgeRatio = 0.6f; // To the center of the line
        constexpr auto fallbackLineWidth                   = 1;
        constexpr auto modelChar = 'h'; // Just get any char, it is needed only to obtain line width

        unsupported                                    = std::make_unique<FontGlyph>();
        unsupported->height                            = info.size * ptToPx;
        unsupported->width                             = unsupported->height;
        unsupported->xoffset                           = 0;
        unsupported->yoffset                           = unsupported->height;

        const auto modelGlyph = glyphs.find(modelChar);
        if (modelGlyph != glyphs.end()) {
            const auto modelGlyphData = modelGlyph->second.get();
            unsupported->xoffset      = (modelGlyphData->xadvance - modelGlyphData->width) / 2;
        }

        if (unsupported->xoffset == 0) {
            unsupported->xoffset = fallbackLineWidth;
        }

        unsupported->xadvance =
            unsupported->width + (2 * unsupported->xoffset); // use xoffset as margins on the left/right of the glyph

        const auto diagonalHorizontalDistance =
            static_cast<Position>((unsupported->width / 2) * diagonalSpacingToRectangleEdgeRatio);
        const auto diagonalVerticalDistance =
            static_cast<Position>((unsupported->height / 2) * diagonalSpacingToRectangleEdgeRatio);

        /* Correction required to place the diagonals in the center of the rectangle
         * regardless of line width. Drawing line with width > 1 is done
         * by drawing multiple 1px lines in certain direction, so correction has to
         * be applied to compensate that. */
        const auto horizontalOffset = static_cast<Position>(std::ceil(unsupported->xoffset / 2.0));

        const Point rectangleOrigin      = {0, 0};
        const Point firstDiagonalOrigin  = {diagonalHorizontalDistance - horizontalOffset, diagonalVerticalDistance};
        const Point secondDiagonalOrigin = {unsupported->width - diagonalHorizontalDistance - horizontalOffset,
                                            diagonalVerticalDistance};

        /* The "length" in draw45deg() is not an actual length, it's length of the projection
         * of the line on one of the axes (no matter which one, as it is 45 degrees angle), i.e.
         * real length divided by square root of 2.
         * Both diagonals have the same length, so compute only one of them, based on height.
         * Computation based on width will yield the same result - we're in square. */
        const auto diagonalLength = unsupported->height - (2 * diagonalVerticalDistance) + 1;

        const gui::renderer::RectangleRenderer::DrawableStyle rectangleStyle = {
            .borderWidth = static_cast<Length>(unsupported->xoffset)};
        const gui::renderer::LineRenderer::DrawableStyle diagonalStyle = {
            .penWidth  = static_cast<Length>(unsupported->xoffset),
            .direction = renderer::LineExpansionDirection::Right};

        /* Render items */
        Context renderCtx(unsupported->width, unsupported->height);
        gui::renderer::RectangleRenderer::drawFlat(
            &renderCtx, rectangleOrigin, unsupported->width, unsupported->height, rectangleStyle);
        gui::renderer::LineRenderer::draw45deg(
            &renderCtx, firstDiagonalOrigin, diagonalLength, diagonalStyle, true); // Draw to the right
        gui::renderer::LineRenderer::draw45deg(
            &renderCtx, secondDiagonalOrigin, diagonalLength, diagonalStyle, false); // Draw to the left

        const auto glyphDataSize = unsupported->width * unsupported->height;
        unsupported->data        = new uint8_t[glyphDataSize];
        std::memcpy(unsupported->data, renderCtx.getData(), glyphDataSize);
    }

    void RawFont::setFallbackFont(RawFont *fallback)
    {
        if (fallback != this) {
            fallback_font = fallback;
        }
    }
} /* namespace gui */
