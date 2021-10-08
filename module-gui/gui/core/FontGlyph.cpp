// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "FontGlyph.hpp"
#include <cstring>

typedef uint32_t ucode32;

namespace gui
{
    FontGlyph::~FontGlyph()
    {
        if (data != nullptr)
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

    gui::Status FontGlyph::load(uint8_t *data, uint32_t &offset)
    {
        // character id
        memcpy(&id, data + offset, sizeof(ucode32));
        offset += sizeof(ucode32);
        // offset in glyph data field
        memcpy(&glyph_offset, data + offset, sizeof(uint32_t));
        offset += sizeof(uint32_t);
        // width of the character image in the texture
        memcpy(&width, data + offset, sizeof(uint16_t));
        offset += sizeof(uint16_t);
        // height of the character image in the texture
        memcpy(&height, data + offset, sizeof(uint16_t));
        offset += sizeof(uint16_t);
        // how much the current position should be offset when copying the image from the texture to the screen
        memcpy(&xoffset, data + offset, sizeof(uint16_t));
        offset += sizeof(uint16_t);
        // how much the current position should be offset when copying the image from the texture to the screen
        memcpy(&yoffset, data + offset, sizeof(uint16_t));
        offset += sizeof(uint16_t);
        // how much the current position should be advanced after drawing the character
        memcpy(&xadvance, data + offset, sizeof(uint16_t));
        offset += sizeof(uint16_t);

        return gui::Status::GUI_SUCCESS;
    }

    gui::Status FontGlyph::loadImage(uint8_t *data, uint32_t offset)
    {

        // image data of the glyph
        this->data = new uint8_t[width * height];

        memcpy(this->data, data + offset, width * height);

        // TODO convert image to the vector of vectors where every last vector contains pairs of
        // TODO offset - from column 0, length - number of pixels that should be drawn.
        // TODO First vector contains row and pointer to the vector with pairs

        return gui::Status::GUI_SUCCESS;
    }
} // namespace gui
