// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "RawFont.hpp"
#include "Common.hpp"        // for Status, Status::GUI_SUCCESS, Status::GU...
#include "Context.hpp"       // for Context
#include "DrawCommand.hpp"   // for DrawRectangle, DrawCommand (ptr only)
#include "FontKerning.hpp"   // for FontKerning
#include "Renderer.hpp"      // for Renderer
#include "TextConstants.hpp" // for newline
#include <log.hpp>           // for LOG_ERROR
#include "utf8/UTF8.hpp"     // for UTF8
#include <cstring>           // for memcpy
#include <utility>           // for pair

namespace gui
{

    RawFont::~RawFont()
    {
        if (fallback_font != nullptr) {
            fallback_font = nullptr;
        }
    }

    gui::Status RawFont::load(uint8_t *data)
    {
        uint32_t offset = 0;

        if (info.load(data, offset) != gui::Status::GUI_SUCCESS)
            return gui::Status::GUI_FAILURE;

        // number of glyphs in the font
        memcpy(&glyph_count, data + offset, sizeof(uint32_t));
        offset += sizeof(uint32_t);
        // offset to the beginning of the glyph data
        memcpy(&glyph_data_offset, data + offset, sizeof(uint32_t));
        offset += sizeof(uint32_t);
        // number of kerning pairs
        memcpy(&kern_count, data + offset, sizeof(uint32_t));
        offset += sizeof(uint32_t);
        // array of glyphs structures
        memcpy(&kern_data_offset, data + offset, sizeof(uint32_t));
        offset += sizeof(uint32_t);
        // offset to the beginning of the image data
        memcpy(&image_data_offset, data + offset, sizeof(uint32_t));
        offset += sizeof(uint32_t);
        // id of the font assigned by the font manager
        id = 1;

        // load glyphs
        uint32_t glyphOffset = glyph_data_offset;
        for (unsigned int i = 0; i < glyph_count; i++) {
            auto glyph = std::make_unique<FontGlyph>();
            glyph->load(data, glyphOffset);
            glyph->loadImage(data, glyph->glyph_offset);
            glyphs.insert(std::pair<uint32_t, std::unique_ptr<FontGlyph>>(glyph->id, std::move(glyph)));
        }

        // load kerning
        // first map contains index of the character and the map that holds values for kerning between
        // first and second character character. In second map key is the value of the second character
        // and value is the kerning between first and second element.
        uint32_t kernOffset = kern_data_offset;
        for (unsigned int i = 0; i < kern_count; i++) {
            auto kern = std::make_unique<FontKerning>();
            kern->load(data, kernOffset);

            // find map using first element of kerning as a key
            auto it = kerning.find(kern->first);

            // element wasn't found
            if (it == kerning.end()) {
                std::map<uint32_t, std::unique_ptr<FontKerning>> kernMap;
                auto kern_first = kern->first;
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

    int32_t RawFont::getKerning(uint32_t id1, uint32_t id2) const
    {
        if (id2 == none_char_id) {
            return 0;
        }
        // search for a map with kerning for given character (id1)
        auto it1 = kerning.find(id1);

        // if there is no map with kerning for id1 return 0;
        if (it1 == kerning.end())
            return 0;

        auto &kernMap = it1->second;

        // otherwise search for id2 in kernMap and return value or 0 if it's not found
        auto it2 = kernMap.find(id2);
        if (it2 == kernMap.end()) {
            return 0;
        }

        const std::unique_ptr<FontKerning> &kern = it2->second;
        return kern->amount;
    }

    uint32_t RawFont::getCharCountInSpace(const UTF8 &str, const uint32_t space) const
    {
        uint32_t availableSpace = space;
        uint32_t count          = 0;
        uint32_t current        = 0;
        uint32_t previous       = none_char_id;

        for (uint32_t i = 0; i < str.length(); ++i, ++count) {
            current               = str[i];
            auto char_pixel_width = getCharPixelWidth(current, previous);
            if (availableSpace < char_pixel_width) {
                return count;
            }
            availableSpace -= char_pixel_width;
            previous = current;
        }
        return count;
    }

    FontGlyph *RawFont::getGlyph(uint32_t glyph_id) const
    {
        auto glyph = this->findGlyph(glyph_id);
        if (glyph != nullptr) {
            return glyph;
        }
        glyph = findGlyphFallback(glyph_id);
        if (glyph != nullptr) {
            return glyph;
        }
        return unsupported.get();
    }

    FontGlyph *RawFont::findGlyph(uint32_t glyph_id) const
    {
        auto glyph_found = glyphs.find(glyph_id);
        if (glyph_found != glyphs.end()) {
            return glyph_found->second.get();
        }
        return nullptr;
    }

    FontGlyph *RawFont::findGlyphFallback(uint32_t glyph_id) const
    {
        if (fallback_font == nullptr) {
            return nullptr;
        }
        return fallback_font->findGlyph(glyph_id);
    }

    uint32_t RawFont::getPixelWidth(const UTF8 &str, const uint32_t start, const uint32_t count) const
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
        uint32_t width     = 0;
        uint32_t idCurrent = 0;
        uint32_t idLast    = none_char_id;

        for (uint32_t i = 0; i < count; ++i) {
            idCurrent = str[start + i];
            width += getCharPixelWidth(idCurrent, idLast);
            idLast = idCurrent;
        }

        return width;
    }

    uint32_t RawFont::getPixelWidth(const UTF8 &str) const
    {
        return getPixelWidth(str, 0, str.length());
    }

    uint32_t RawFont::getCharPixelWidth(uint32_t charCode, uint32_t previousChar) const
    {
        if (charCode == text::newline) { // newline doesn't have width
            return 0;
        }
        auto glyph = getGlyph(charCode);
        return glyph->xadvance + getKerning(charCode, previousChar);
    }

    uint32_t RawFont::getCharPixelHeight(uint32_t charCode)
    {
        FontGlyph *glyph = getGlyph(charCode);
        return glyph->height;
    }

    UTF8 RawFont::getTextWithElipsis(const UTF8 &text, uint32_t width, Ellipsis ellipsis) const
    {
        std::string result;
        auto w_dot        = getCharPixelWidth('.');
        auto text_fit_len = getCharCountInSpace(text, width);
        if (width < w_dot) // none will fit
        {
            return result;
        }
        if (text_fit_len == text.length()) // all will fit
        {
            return text;
        }
        if (ellipsis != Ellipsis::None) {
            auto char_offset = 0;
            if (ellipsis == Ellipsis::Left) {
                char_offset = text.length() - text_fit_len;
            }
            result = text.substr(char_offset, text_fit_len);
            setEllipsis(result, ellipsis);
        }
        else {
            result = text.substr(0, text_fit_len);
        }
        return result;
    }

    void RawFont::setEllipsis(std::string &text, Ellipsis ellipsis) const
    {
        auto set_dot = [&](auto begin, auto end) {
            auto dots_in_elipsis = 3;
            for (auto el = begin; el != end; ++el) {
                *el = '.';
                if (--dots_in_elipsis == 0) {
                    break;
                }
            }
        };
        switch (ellipsis) {
        case Ellipsis::Right:
            set_dot(text.rbegin(), text.rend());
            break;
        case Ellipsis::Left:
            set_dot(text.begin(), text.end());
            break;
        case Ellipsis::None:
            break;
        }
    }

    void RawFont::createGlyphUnsupported()
    {
        unsupported                                    = std::make_unique<FontGlyph>();
        const float pt_to_px                           = 0.75;
        unsupported->height                            = this->info.size * pt_to_px;
        const float typical_width_as_percent_of_height = 0.62;
        unsupported->width                             = unsupported->height * typical_width_as_percent_of_height;
        unsupported->xoffset                           = 0;
        unsupported->yoffset                           = 0;

        // generate a rectangle based on an existing letter. otherwise use some magic numbers ↑ to approximate the size
        // for the rectangle
        char baseChar = 'h'; // arbitrary choice. h as a representative character to get an idea of glyph size. if not
                             // found, then use magic numbers above
        auto baseCharFound = this->glyphs.find(baseChar);
        if (baseCharFound != this->glyphs.end()) {
            FontGlyph *baseGlyph = baseCharFound->second.get();
            unsupported->width   = baseGlyph->width;
            unsupported->height  = baseGlyph->height;
            unsupported->xoffset = (baseGlyph->xadvance - baseGlyph->width) / 2;
        }
        if (unsupported->xoffset == 0) {
            unsupported->xoffset = 1; // fallback margin.
        }
        unsupported->yoffset += unsupported->height;
        unsupported->xadvance =
            unsupported->width + (2 * unsupported->xoffset); // use xoffset as margins on the left/right of the glyph
        // populate with a bitmap (glyph)
        auto commandRect      = std::make_unique<DrawRectangle>();
        commandRect->origin   = {0, 0};
        commandRect->width    = unsupported->width;
        commandRect->height   = unsupported->height;
        commandRect->areaX    = 0;
        commandRect->areaY    = 0;
        commandRect->areaW    = unsupported->width;
        commandRect->areaH    = unsupported->height;
        commandRect->penWidth = unsupported->xoffset;

        auto renderCtx = std::make_unique<Context>(unsupported->width, unsupported->height);
        std::list<std::unique_ptr<gui::DrawCommand>> commands;
        commands.emplace_back(std::move(commandRect));
        Renderer().render(renderCtx.get(), commands);

        auto size         = unsupported->width * unsupported->height;
        unsupported->data = new uint8_t[size];
        std::memcpy(unsupported->data, renderCtx->getData(), size);
    }

    void RawFont::setFallbackFont(RawFont *fallback)
    {
        if (fallback != this) {
            fallback_font = fallback;
        }
    }
} /* namespace gui */
