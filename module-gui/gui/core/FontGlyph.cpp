// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "FontGlyph.hpp"
#include <cstring>
#include <new>

namespace gui
{
    FontGlyph::~FontGlyph()
    {
        delete[] data;
    }

    FontGlyph::FontGlyph(const FontGlyph *from)
    {
        this->id           = from->id;
        this->glyph_offset = from->glyph_offset;
        this->width        = from->width;
        this->height       = from->height;
        this->xoffset      = from->xoffset;
        this->yoffset      = from->yoffset;
        this->xadvance     = from->xadvance;
    }

    gui::Status FontGlyph::load(std::uint8_t *glyphData, std::uint32_t &offset)
    {
        // Character id
        std::memcpy(&id, &glyphData[offset], sizeof(ucode32));
        offset += sizeof(ucode32);

        // Offset in glyph data field
        std::memcpy(&glyph_offset, &glyphData[offset], sizeof(std::uint32_t));
        offset += sizeof(std::uint32_t);

        // Width of the character image in the texture
        std::memcpy(&width, &glyphData[offset], sizeof(std::uint16_t));
        offset += sizeof(std::uint16_t);

        // Height of the character image in the texture
        std::memcpy(&height, &glyphData[offset], sizeof(std::uint16_t));
        offset += sizeof(std::uint16_t);

        // How much the current position should be offset when copying the image from the texture to the screen
        std::memcpy(&xoffset, &glyphData[offset], sizeof(std::uint16_t));
        offset += sizeof(std::uint16_t);

        // How much the current position should be offset when copying the image from the texture to the screen
        std::memcpy(&yoffset, &glyphData[offset], sizeof(std::uint16_t));
        offset += sizeof(std::uint16_t);

        // How much the current position should be advanced after drawing the character
        std::memcpy(&xadvance, &glyphData[offset], sizeof(std::uint16_t));
        offset += sizeof(std::uint16_t);

        return gui::Status::GUI_SUCCESS;
    }

    gui::Status FontGlyph::loadImage(std::uint8_t *glyphData, std::uint32_t offset)
    {
        // Image data of the glyph
        data = new (std::nothrow) std::uint8_t[width * height];
        if (data == nullptr) {
            return Status::GUI_FAILURE;
        }

        std::memcpy(data, &glyphData[offset], width * height);

        // TODO convert image to the vector of vectors where every last vector contains pairs of
        // TODO offset - from column 0, length - number of pixels that should be drawn.
        // TODO First vector contains row and pointer to the vector with pairs

        return gui::Status::GUI_SUCCESS;
    }
} // namespace gui
