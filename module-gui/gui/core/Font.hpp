/*
 * Font.hpp
 *
 *  Created on: 25 kwi 2019
 *      Author: robert
 */

#ifndef GUI_CORE_FONT_HPP_
#define GUI_CORE_FONT_HPP_

#include "../Common.hpp"
#include "utf8/UTF8.hpp"
#include <cstdint>
#include <fstream>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include "Ellipsis.hpp"

namespace gui
{

    class FontGlyph
    {
      public:
        FontGlyph() = default;
        FontGlyph(FontGlyph *);
        virtual ~FontGlyph();
        gui::Status load(uint8_t *data, uint32_t &offset);
        gui::Status loadImage(uint8_t *data, uint32_t offset);
        // character id
        uint16_t id = 0;
        // offset in glyph data field
        uint32_t glyph_offset = 0;
        // width of the character image in the texture
        uint16_t width = 0;
        // height of the character image in the texture
        uint16_t height = 0;
        // how much the current position should be offset when copying the image from the texture to the screen
        int16_t xoffset = 0;
        // how much the current position should be offset when copying the image from the texture to the screen
        int16_t yoffset = 0;
        // how much the current position should be advanced after drawing the character
        uint16_t xadvance = 0;
        // image data of the glyph
        uint8_t *data = 0;
    };

    class FontKerning
    {
      public:
        gui::Status load(uint8_t *data, uint32_t &offset);
        // utf16 id of the first character
        uint16_t first;
        // utf16 id of the following character
        uint16_t second;
        // distance in pixels between beginning of first character and beginning of second character
        int16_t amount;
    };

    class FontInfo
    {
      public:
        gui::Status load(uint8_t *data, uint32_t &offset);
        // name of the true type font. max 63 characters
        std::string face;
        // size of the true type font
        uint16_t size;
        // flag that informs if font is bold
        uint16_t bold;
        // flag that informs if font is italic
        uint16_t italic;
        // flag that informs if smoothing was turned on. 1 - smoothing was turned on.
        uint16_t smooth;
        // TODO additional space between characters????
        uint16_t char_spacing;
        // TODO additional space between lines
        uint16_t line_spacing;
        // distance in pixels between each line of text
        uint16_t line_height;
        // number of pixels from the absolute top of the line to the base of the characters
        uint16_t base;
        // width of the texture, normally used to scale the x pos of the character image
        uint16_t scale_w;
        // height of the texture, normally used to scale the y pos of the character image
        uint16_t scale_h;
    };

    class Font
    {
      public:
        virtual ~Font();

        gui::Status load(uint8_t *data);

        static const uint32_t none_char_id = std::numeric_limits<const uint32_t>().max();
        // structure holding detailed information about font
        FontInfo info;
        // number of glyphs in the fontno
        uint32_t glyph_count;
        // offset to the beginning of the glyph data
        uint32_t glyph_data_offset;
        // number of kerning pairs
        uint32_t kern_count;
        // array of glyphs structures
        uint32_t kern_data_offset;
        // offset to the beginning of the image data
        uint32_t image_data_offset;
        // id of the font asigned by the font manager
        uint32_t id;
        std::map<uint32_t, FontGlyph *> glyphs;
        std::map<uint32_t, std::map<uint32_t, FontKerning *> *> kerning;

        /// return glyph for selected code
        /// if code is not found - unsupportedGlyph returned
        std::unique_ptr<FontGlyph> getGlyph(uint32_t id) const;

        /**
         * @brief Returns kerning value for pair of the two characters.
         * @param id1 Code of the first character.
         * @param id2 Code of the second character - if none_char_id then return 0
         * @return Value of the kerning or 0 if pair was not found.
         */
        int32_t getKerning(uint32_t id1, uint32_t id2) const;
        /**
         * @brief Method calculates how many chars will fit specified width using current font.
         * @param str UTF8 string that will be used to calculate how many chars can fit provided space.
         * @param space Number of pixels in width availabale to calculate how many chars will fit.
         * @return number of chars that can fit provided space;
         */
        uint32_t getCharCountInSpace(const UTF8 &str, const uint32_t space) const;
        /**
         * @brief Calculates how many pixels will occupy selected part of the string.
         * @param str String used as a source of text.
         * @param start Starting index used to calculate number of occupied pixels in width.
         * @param count Number of characters that should be used during calculating pixels width.
         * @return Number of pixels in width occupied by selected part of the text.
         */
        uint32_t getPixelWidth(const UTF8 &str, const uint32_t start, const uint32_t count) const;
        /**
         * @brief Calculates how many pixels will occupy string.
         * @param str String used as a source of text.
         * @return Number of pixels in width occupied by string.
         */
        uint32_t getPixelWidth(const UTF8 &str) const;
        /**
         * @brief returns number of pixels occupied by character horizontally.
         *
         * if previous char is set - then tries to append kerning
         */
        uint32_t getCharPixelWidth(uint32_t charCode, uint32_t previousChar = none_char_id) const;
        /**
         * @brief Returns number of pixels occupied by the character vertically.
         */
        uint32_t getCharPixelHeight(uint32_t charCode);
        const std::string getName()
        {
            return info.face;
        }

        /// get elipsis text in selected width from input text
        /// this essentially means:
        /// return test with "..." if needed in text begin / end if needed
        UTF8 getTextWithElipsis(const UTF8 &text, uint32_t width, Ellipsis ellipsis) const;

        std::unique_ptr<FontGlyph> getGlyphUnsupported() const;

      private:
        /// set ellipsis on text in first parameter
        /// @note our UTF8 doesn't provide way to replace single character
        ///
        /// this is lazy done implementation - we just try to replace first/last 3 characters that would fit
        void setEllipsis(std::string &text, Ellipsis ellipsis) const;
    };

    class FontManager
    {
      protected:
        std::string fontFolder;
        std::vector<Font *> fonts;
        std::vector<std::string> getFontsList();

        Font *loadFont(std::string filename);
        void loadFonts(std::string baseDirectory);

        FontManager();

      public:
        FontManager(const FontManager &) = delete;
        void operator=(const FontManager &) = delete;

        bool init(std::string baseDirectory);
        void clear();
        static FontManager &getInstance();

        virtual ~FontManager();

        Font *getFont(uint32_t id);
        uint32_t getFontID(const std::string &name);
    };

} /* namespace gui */

#endif /* GUI_CORE_FONT_HPP_ */
