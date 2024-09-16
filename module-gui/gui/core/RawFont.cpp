// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

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
        fallbackFont = nullptr;
    }

    auto RawFont::load(std::uint8_t *data) -> gui::Status
    {
        std::uint32_t offset = 0;

        if (info.load(data, offset) != gui::Status::GUI_SUCCESS) {
            return gui::Status::GUI_FAILURE;
        }

        // Number of glyphs in the font
        std::memcpy(&glyphCount, &data[offset], sizeof(std::uint32_t));
        offset += sizeof(std::uint32_t);

        // Offset to the beginning of the glyph data
        std::memcpy(&glyphDataOffset, &data[offset], sizeof(std::uint32_t));
        offset += sizeof(std::uint32_t);

        // Number of kerning pairs
        std::memcpy(&kernCount, &data[offset], sizeof(std::uint32_t));
        offset += sizeof(std::uint32_t);

        // Array of glyphs structures
        std::memcpy(&kernDataOffset, &data[offset], sizeof(std::uint32_t));
        offset += sizeof(std::uint32_t);

        // Offset to the beginning of the image data
        std::memcpy(&imageDataOffset, &data[offset], sizeof(std::uint32_t));
        offset += sizeof(std::uint32_t);

        // Id of the font assigned by the font manager
        id = 1;

        // Load glyphs
        auto glyphOffset = glyphDataOffset;
        for (auto i = 0U; i < glyphCount; i++) {
            auto glyph = std::make_unique<FontGlyph>();
            glyph->load(data, glyphOffset);
            glyph->loadImage(data, glyph->glyph_offset);
            glyphs.emplace(glyph->id, std::move(glyph));
        }

        // Load kerning
        // First map contains index of the character and the map that holds values for kerning between
        // first and second character. In second map key is the value of the second character
        // and value is the kerning between first and second element.
        auto kernOffset = kernDataOffset;
        for (auto i = 0U; i < kernCount; i++) {
            auto kern = std::make_unique<FontKerning>();
            kern->load(data, kernOffset);

            // Find map using first element of kerning as a key
            const auto it = kerning.find(kern->first);

            // Element wasn't found
            if (it == kerning.end()) {
                std::map<std::uint32_t, std::unique_ptr<FontKerning>> kernMap;
                const auto kern_first = kern->first;
                kernMap.emplace(kern->second, std::move(kern));

                // insert element to the first map
                kerning.emplace(kern_first, std::move(kernMap));
            }
            else {
                auto &kernMap = it->second;
                kernMap.emplace(kern->second, std::move(kern));
            }
        }

        createGlyphUnsupported();

        return gui::Status::GUI_SUCCESS;
    }

    auto RawFont::getKerning(std::uint32_t id1, std::uint32_t id2) const -> std::int32_t
    {
        if (id2 == noneCharId) {
            return 0;
        }

        // Search for a map with kerning for given character (id1)
        const auto it1 = kerning.find(id1);

        // If there is no map with kerning for id1 return 0;
        if (it1 == kerning.end()) {
            return 0;
        }

        const auto &kernMap = it1->second;

        // Otherwise search for id2 in kernMap and return value or 0 if it's not found
        const auto it2 = kernMap.find(id2);
        if (it2 == kernMap.end()) {
            return 0;
        }

        const auto &kern = it2->second;
        return kern->amount;
    }

    auto RawFont::getCharCountInSpace(const UTF8 &str, std::uint32_t availableSpace) const -> std::uint32_t
    {
        std::uint32_t count          = 0;
        std::uint32_t usedSpace      = 0;
        auto previousChar            = noneCharId;

        while (count < str.length()) {
            const auto currentChar = str[count];
            usedSpace += getCharPixelWidth(currentChar, previousChar);
            if (availableSpace < usedSpace) {
                return ((count > 0) ? (count - 1) : 0);
            }
            previousChar = currentChar;
            ++count;
        }
        return count;
    }

    auto RawFont::getGlyph(std::uint32_t glyph_id) const -> FontGlyph *
    {
        auto glyph = findGlyph(glyph_id);
        if (glyph != nullptr) {
            return glyph;
        }
        glyph = findGlyphFallback(glyph_id);
        if (glyph != nullptr) {
            return glyph;
        }

        LOG_WARN("Unsupported font glyph ID: %" PRIu32, glyph_id);
        return unsupported.get();
    }

    auto RawFont::findGlyph(std::uint32_t glyph_id) const -> FontGlyph *
    {
        const auto glyph_found = glyphs.find(glyph_id);
        if (glyph_found != glyphs.end()) {
            return glyph_found->second.get();
        }
        return nullptr;
    }

    auto RawFont::findGlyphFallback(std::uint32_t glyph_id) const -> FontGlyph *
    {
        if (fallbackFont == nullptr) {
            return nullptr;
        }
        return fallbackFont->findGlyph(glyph_id);
    }

    auto RawFont::getPixelWidth(const UTF8 &str, const std::uint32_t start, const std::uint32_t count) const
        -> std::uint32_t
    {
        if (count == 0) {
            return 0;
        }

        if (str.empty()) {
            return 0;
        }

        if ((start >= str.length()) || ((start + count - 1) >= str.length())) {
            LOG_ERROR("Incorrect string index provided");
            return 0;
        }

        // width of text in pixels
        std::uint32_t width     = 0;
        std::uint32_t idCurrent = 0;
        std::uint32_t idLast    = noneCharId;

        for (auto i = 0U; i < count; ++i) {
            idCurrent = str[start + i];
            width += getCharPixelWidth(idCurrent, idLast);
            idLast = idCurrent;
        }

        return width;
    }

    auto RawFont::getPixelWidth(const UTF8 &str) const -> std::uint32_t
    {
        return getPixelWidth(str, 0, str.length());
    }

    auto RawFont::getCharPixelWidth(std::uint32_t charCode, std::uint32_t previousChar) const -> std::uint32_t
    {
        if (charCode == text::newline) { // newline doesn't have width
            return 0;
        }
        const auto glyph = getGlyph(charCode);
        return glyph->xadvance + getKerning(charCode, previousChar);
    }

    auto RawFont::getCharPixelHeight(std::uint32_t charCode) const -> std::uint32_t
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
            fallbackFont = fallback;
        }
    }
} // namespace gui
