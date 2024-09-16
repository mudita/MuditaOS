// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include "../Common.hpp"
#include "FontInfo.hpp"
#include "utf8/UTF8.hpp"
#include <cstdint>
#include <limits>
#include <map>
#include <memory>
#include <string>
#include "FontGlyph.hpp"
#include "FontKerning.hpp"

namespace gui
{
    /// Class representation and usage of RawFont straight from file
    class RawFont
    {
      public:
        virtual ~RawFont();

        auto load(std::uint8_t *data) -> gui::Status;

        static constexpr auto noneCharId = std::numeric_limits<std::uint32_t>::max();

        // Structure holding detailed information about font
        FontInfo info;
        // Number of glyphs in the font
        std::uint32_t glyphCount;
        // Offset to the beginning of the glyph data
        std::uint32_t glyphDataOffset;
        // Number of kerning pairs
        std::uint32_t kernCount;
        // Array of glyphs structures
        std::uint32_t kernDataOffset;
        // Offset to the beginning of the image data
        std::uint32_t imageDataOffset;
        // Id of the font assigned by the font manager
        std::uint32_t id;

        /**
         * @brief Return glyph for selected code.
         *        If code is not found in font, it is searched in the fallback font,
         *        if not found - unsupportedGlyph is returned.
         * @param id Code of the character to find glyph for.
         * @return Pointer to FontGlyph representing the character or unsupportedGlyph if glyph not found.
         */
        [[nodiscard]] auto getGlyph(std::uint32_t id) const -> FontGlyph *;

        /**
         * @brief Returns kerning value for pair of the two characters.
         * @param id1 Code of the first character.
         * @param id2 Code of the second character - if none_char_id then return 0
         * @return Value of the kerning or 0 if pair was not found.
         */
        [[nodiscard]] auto getKerning(std::uint32_t id1, std::uint32_t id2) const -> std::int32_t;

        /**
         * @brief Method calculates how many chars will fit specified width using current font.
         * @param str UTF8 string that will be used to calculate how many chars can fit provided space.
         * @param space Number of pixels in width available to calculate how many chars will fit.
         * @return number of chars that can fit provided space;
         */
        [[nodiscard]] auto getCharCountInSpace(const UTF8 &str, std::uint32_t availableSpace) const -> std::uint32_t;

        /**
         * @brief Calculates how many pixels will occupy selected part of the string.
         * @param str String used as a source of text.
         * @param start Starting index used to calculate number of occupied pixels in width.
         * @param count Number of characters that should be used during calculating pixels width.
         * @return Number of pixels in width occupied by selected part of the text.
         */
        [[nodiscard]] auto getPixelWidth(const UTF8 &str, std::uint32_t start, std::uint32_t count) const
            -> std::uint32_t;

        /**
         * @brief Calculates how many pixels will occupy string.
         * @param str String used as a source of text.
         * @return Number of pixels in width occupied by string.
         */
        [[nodiscard]] auto getPixelWidth(const UTF8 &str) const -> std::uint32_t;

        /**
         * @brief returns number of pixels occupied by character horizontally.
         *
         * if previous char is set - then tries to append kerning
         */
        [[nodiscard]] auto getCharPixelWidth(std::uint32_t charCode, std::uint32_t previousChar = noneCharId) const
            -> std::uint32_t;

        /**
         * @brief Returns number of pixels occupied by the character vertically.
         */
        [[nodiscard]] auto getCharPixelHeight(std::uint32_t charCode) const -> std::uint32_t;

        /**
         * @brief Sets fallback font.
         */
        auto setFallbackFont(RawFont *font) -> void;

        [[nodiscard]] auto getName() const -> std::string
        {
            return info.face;
        }

      private:
        std::map<std::uint32_t, std::unique_ptr<FontGlyph>> glyphs;
        std::map<std::uint32_t, std::map<std::uint32_t, std::unique_ptr<FontKerning>>> kerning;
        /// If the fallback font is set it is used in case of a glyph being unsupported in the primary font
        RawFont *fallbackFont = nullptr;
        /// The glyph used when requested glyph is unsupported in the font (and the fallback font if one is set)
        std::unique_ptr<FontGlyph> unsupported = nullptr;

        void createGlyphUnsupported();

        /// Return glyph for selected code
        /// If code is not found - nullptr is returned
        [[nodiscard]] auto findGlyph(std::uint32_t glyph_id) const -> FontGlyph *;

        /// Return glyph for selected code
        /// If code is not found - nullptr is returned
        [[nodiscard]] auto findGlyphFallback(std::uint32_t glyph_id) const -> FontGlyph *;
    };
} // namespace gui
