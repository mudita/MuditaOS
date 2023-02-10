// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

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
    class FontGlyph;
    class FontKerning;

    /// class representation and usage of RawFont straight from file
    class RawFont
    {
      public:
        virtual ~RawFont();

        gui::Status load(uint8_t *data);

        static const std::uint32_t none_char_id = std::numeric_limits<const std::uint32_t>().max();
        // structure holding detailed information about font
        FontInfo info;
        // number of glyphs in the font
        std::uint32_t glyph_count;
        // offset to the beginning of the glyph data
        std::uint32_t glyph_data_offset;
        // number of kerning pairs
        std::uint32_t kern_count;
        // array of glyphs structures
        std::uint32_t kern_data_offset;
        // offset to the beginning of the image data
        std::uint32_t image_data_offset;
        // id of the font assigned by the font manager
        std::uint32_t id;

        /// return glyph for selected code
        /// if code is not found in font, it is searched in the fallback font, if not found - unsupportedGlyph returned
        FontGlyph *getGlyph(std::uint32_t id) const;

        /**
         * @brief Returns kerning value for pair of the two characters.
         * @param id1 Code of the first character.
         * @param id2 Code of the second character - if none_char_id then return 0
         * @return Value of the kerning or 0 if pair was not found.
         */
        int32_t getKerning(std::uint32_t id1, std::uint32_t id2) const;
        /**
         * @brief Method calculates how many chars will fit specified width using current font.
         * @param str UTF8 string that will be used to calculate how many chars can fit provided space.
         * @param space Number of pixels in width availabale to calculate how many chars will fit.
         * @return number of chars that can fit provided space;
         */
        std::uint32_t getCharCountInSpace(const UTF8 &str, const std::uint32_t space) const;
        /**
         * @brief Calculates how many pixels will occupy selected part of the string.
         * @param str String used as a source of text.
         * @param start Starting index used to calculate number of occupied pixels in width.
         * @param count Number of characters that should be used during calculating pixels width.
         * @return Number of pixels in width occupied by selected part of the text.
         */
        std::uint32_t getPixelWidth(const UTF8 &str, const std::uint32_t start, const std::uint32_t count) const;
        /**
         * @brief Calculates how many pixels will occupy string.
         * @param str String used as a source of text.
         * @return Number of pixels in width occupied by string.
         */
        std::uint32_t getPixelWidth(const UTF8 &str) const;
        /**
         * @brief returns number of pixels occupied by character horizontally.
         *
         * if previous char is set - then tries to append kerning
         */
        std::uint32_t getCharPixelWidth(std::uint32_t charCode, std::uint32_t previousChar = none_char_id) const;
        /**
         * @brief Returns number of pixels occupied by the character vertically.
         */
        std::uint32_t getCharPixelHeight(std::uint32_t charCode);
        const std::string getName()
        {
            return info.face;
        }
        void setFallbackFont(RawFont *font);

      private:
        std::map<std::uint32_t, std::unique_ptr<FontGlyph>> glyphs;
        std::map<std::uint32_t, std::map<std::uint32_t, std::unique_ptr<FontKerning>>> kerning;
        /// if the fallback font is set it is used in case of a glyph being unsupported in the primary font
        RawFont *fallback_font = nullptr;
        /// the glyph used when requested glyph is unsupported in the font (and the fallback font if one is set)
        std::unique_ptr<FontGlyph> unsupported = nullptr;

        void createGlyphUnsupported();

        /// return glyph for selected code
        /// if code is not found - nullptr is returned
        FontGlyph *findGlyph(std::uint32_t id) const;
        /// return glyph for selected code
        /// if code is not found - nullptr is returned
        FontGlyph *findGlyphFallback(std::uint32_t id) const;
    };
} /* namespace gui */
