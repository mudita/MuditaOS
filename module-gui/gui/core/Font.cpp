/*
 * Font.cpp
 *
 *  Created on: 25 kwi 2019
 *      Author: robert
 */

#include <unistd.h>
#include <string.h>

#include <string>
#include "Font.hpp"

#include "utf8/UTF8.hpp"
#include "log/log.hpp"
// for loading files
#include "Context.hpp"
#include "DrawCommand.hpp"
#include "Renderer.hpp"
#include "vfs.hpp"
#include <cassert>

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
        memcpy(&id, data + offset, sizeof(uint16_t));
        offset += sizeof(uint16_t);
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

    gui::Status FontKerning::load(uint8_t *data, uint32_t &offset)
    {
        // utf16 id of the first character
        memcpy(&first, data + offset, sizeof(uint16_t));
        offset += sizeof(uint16_t);
        // utf16 id of the following character
        memcpy(&second, data + offset, sizeof(uint16_t));
        offset += sizeof(uint16_t);
        // distance in pixels between beginning of first character and beginning of second character
        memcpy(&amount, data + offset, sizeof(uint16_t));
        offset += sizeof(uint16_t);

        return gui::Status::GUI_SUCCESS;
    }

    gui::Status FontInfo::load(uint8_t *data, uint32_t &offset)
    {

        // read up to 63 chars of the fonts name
        char name[64] = {0};
        memcpy(name, data + offset, 63);
        offset += 64;
        face = std::string(name);

        // size of the true type font
        memcpy(&size, data + offset, sizeof(uint16_t));
        offset += sizeof(uint16_t);
        // flag that informs if font is bold
        memcpy(&bold, data + offset, sizeof(uint16_t));
        offset += sizeof(uint16_t);
        // flag that informs if font is italic
        memcpy(&italic, data + offset, sizeof(uint16_t));
        offset += sizeof(uint16_t);
        // flag that informs if smoothing was turned on. 1 - smoothing was turned on.
        memcpy(&smooth, data + offset, sizeof(uint16_t));
        offset += sizeof(uint16_t);
        // TODO additional space between characters????
        memcpy(&char_spacing, data + offset, sizeof(uint16_t));
        offset += sizeof(uint16_t);
        // TODO additional space between lines
        memcpy(&line_spacing, data + offset, sizeof(uint16_t));
        offset += sizeof(uint16_t);
        // distance in pixels between each line of text
        memcpy(&line_height, data + offset, sizeof(uint16_t));
        offset += sizeof(uint16_t);
        // number of pixels from the absolute top of the line to the base of the characters
        memcpy(&base, data + offset, sizeof(uint16_t));
        offset += sizeof(uint16_t);
        // width of the texture, normally used to scale the x pos of the character image
        memcpy(&scale_w, data + offset, sizeof(uint16_t));
        offset += sizeof(uint16_t);
        // height of the texture, normally used to scale the y pos of the character image
        memcpy(&scale_h, data + offset, sizeof(uint16_t));
        offset += sizeof(uint16_t);

        return gui::Status::GUI_SUCCESS;
    }

    Font::~Font()
    {
        for (std::map<uint32_t, FontGlyph *>::iterator it = glyphs.begin(); it != glyphs.end(); ++it)
            delete it->second;
        glyphs.clear();

        for (std::map<uint32_t, std::map<uint32_t, FontKerning *> *>::iterator fit = kerning.begin();
             fit != kerning.end();
             ++fit) {
            for (std::map<uint32_t, FontKerning *>::iterator sit = fit->second->begin(); sit != fit->second->end();
                 ++sit) {
                delete sit->second;
            }
            delete fit->second;
        }
        kerning.clear();
    }

    gui::Status Font::load(uint8_t *data)
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

        //	LOG_INFO("font: %s glyphs: %d, kern: %d", info.face.c_str(), glyph_count, kern_count );
        //	LOG_INFO("glyph offset: %d", glyph_data_offset );
        //	LOG_INFO("kerning offset: %d", kern_data_offset );
        //	LOG_INFO("image offset: %d", image_data_offset );

        // load glyphs
        uint32_t glyphOffset = glyph_data_offset;
        for (unsigned int i = 0; i < glyph_count; i++) {
            FontGlyph *glyph = new FontGlyph();
            glyph->load(data, glyphOffset);
            glyph->loadImage(data, glyph->glyph_offset);
            glyphs.insert(std::pair<uint32_t, FontGlyph *>(glyph->id, glyph));
        }

        // load kerning
        // first map contains index of the character and the map that holds values for kerning between
        // first and second character character. In second map key is the value of the second character
        // and value is the kerning between first and second element.
        uint32_t kernOffset = kern_data_offset;
        for (unsigned int i = 0; i < kern_count; i++) {
            FontKerning *kern = new FontKerning();
            kern->load(data, kernOffset);

            // find map using first element of kerning as a key
            auto it = kerning.find(kern->first);

            // element wasn't found
            std::map<uint32_t, FontKerning *> *kernMap;
            if (it == kerning.end()) {
                kernMap = new std::map<uint32_t, FontKerning *>();

                // insert element to the first map
                kerning.insert(std::pair<uint32_t, std::map<uint32_t, FontKerning *> *>(kern->first, kernMap));
            }
            else {
                kernMap = it->second;
            }
            kernMap->insert(std::pair<uint32_t, FontKerning *>(kern->second, kern));
        }

        return gui::Status::GUI_SUCCESS;
    }

    int32_t Font::getKerning(uint32_t id1, uint32_t id2) const
    {
        if (id2 == none_char_id) {
            return 0;
        }
        // search for a map with kerning for given character (id1)
        auto it1 = kerning.find(id1);

        // if there is no map with kerning for id1 return 0;
        if (it1 == kerning.end())
            return 0;

        std::map<uint32_t, FontKerning *> *kernMap = it1->second;

        // otherwise search for id2 in kernMap and return value or 0 if it's not found
        auto it2 = kernMap->find(id2);
        if (it2 == kernMap->end())
            return 0;

        FontKerning *kern = it2->second;
        return kern->amount;
    }

    uint32_t Font::getCharCountInSpace(const UTF8 &str, const uint32_t space) const
    {
        int32_t availableSpace = space;
        uint32_t count         = 0;
        uint32_t current       = 0;
        uint32_t previous      = none_char_id;

        for (uint32_t i = 0; i < str.length(); ++i, ++count) {
            current = str[i];

            availableSpace -= getCharPixelWidth(current, previous);
            if (availableSpace < 0) {
                return count;
            }
            previous = current;
        }
        return count;
    }

    std::unique_ptr<FontGlyph> Font::getGlyph(uint32_t id) const
    {
        auto glyph_found = glyphs.find(id);
        if (glyph_found != glyphs.end()) {
            return std::make_unique<FontGlyph>(glyph_found->second);
        }
        return getGlyphUnsupported();
    }

    uint32_t Font::getPixelWidth(const UTF8 &str, const uint32_t start, const uint32_t count) const
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
        uint32_t width  = 0;
        uint32_t idCurrent = 0;
        uint32_t idLast    = none_char_id;

        for (uint32_t i = 0; i < count; ++i) {
            idCurrent        = str[start + i];
            width += getCharPixelWidth(idCurrent, idLast);
            idLast = idCurrent;
        }

        return width;
    }

    uint32_t Font::getPixelWidth(const UTF8 &str) const
    {
        return getPixelWidth(str, 0, str.length());
    }

    uint32_t Font::getCharPixelWidth(uint32_t charCode, uint32_t previousChar) const
    {
        auto glyph = getGlyph(charCode);
        if (glyph != nullptr) {
            return glyph->xadvance + getKerning(charCode, previousChar);
        }

        return 0;
    }

    uint32_t Font::getCharPixelHeight(uint32_t charCode)
    {
        FontGlyph *glyph = glyphs.find(charCode)->second;

        if (glyph != nullptr)
            return glyph->height;

        return 0;
    }

    UTF8 Font::getTextWithElipsis(const UTF8 &text, uint32_t width, Ellipsis ellipsis) const
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

    void Font::setEllipsis(std::string &text, Ellipsis ellipsis) const
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

    std::unique_ptr<FontGlyph> Font::getGlyphUnsupported() const
    {
        std::unique_ptr<FontGlyph> unsupported = std::make_unique<FontGlyph>();

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
            FontGlyph *baseGlyph = baseCharFound->second;
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
        CommandRectangle *commandRect = new CommandRectangle();
        commandRect->x                = 0;
        commandRect->y                = 0;
        commandRect->w                = unsupported->width;
        commandRect->h                = unsupported->height;
        commandRect->areaX            = 0;
        commandRect->areaY            = 0;
        commandRect->areaW            = unsupported->width;
        commandRect->areaH            = unsupported->height;
        commandRect->penWidth         = unsupported->xoffset;

        Context *renderCtx                       = new Context(unsupported->width, unsupported->height);
        std::vector<gui::DrawCommand *> commands = {commandRect};
        Renderer().render(renderCtx, commands);

        unsupported->data = renderCtx->getData();

        return unsupported;
    }

    FontManager::FontManager()
    {}

    FontManager::~FontManager()
    {
        clear();
    }

    void FontManager::clear()
    {
        for (Font *font : fonts) {
            LOG_INFO("deleting font: %s", font->getName().c_str());
            delete font;
        }
        fonts.clear();
    }

    void FontManager::loadFonts(std::string baseDirectory)
    {
        fontFolder                         = baseDirectory + "/fonts";
        std::vector<std::string> fontFiles = getFontsList();

        for (std::string fontName : fontFiles) {
            LOG_INFO("Loading font: %s", fontName.c_str());
            loadFont(fontName);
        }
    }

    Font *FontManager::loadFont(std::string filename)
    {

        auto file = vfs.fopen(filename.c_str(), "rb");

        auto fileSize = vfs.filelength(file);

        char *fontData = new char[fileSize];
        if (fontData == nullptr) {
            vfs.fclose(file);
            LOG_ERROR(" Failed to allocate temporary font buffer");
            return nullptr;
        }

        // read data to buffer
        auto bytesRead = vfs.fread(fontData, 1, fileSize, file);

        // close file
        vfs.fclose(file);
        if (bytesRead != fileSize) {
            LOG_ERROR("Failed to read all file");
            delete[] fontData;
            return nullptr;
        }

        // allocate memory for new font
        Font *font = new Font();
        if (font->load(reinterpret_cast<uint8_t *>(fontData)) != gui::Status::GUI_SUCCESS) {
            delete font;
            delete[] fontData;
            return nullptr;
        }
        else {
            // set id and push it to vector
            font->id = fonts.size();
            fonts.push_back(font);
        }
        delete[] fontData;
        return font;
    }

    bool hasEnding(std::string const &fullString, std::string const &ending)
    {
        if (fullString.length() >= ending.length()) {
            return (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
        }
        else {
            return false;
        }
    }

    std::vector<std::string> FontManager::getFontsList()
    {

        std::vector<std::string> fontFiles;

        LOG_INFO("Scanning fonts folder: %s", fontFolder.c_str());
        auto dirList = vfs.listdir(fontFolder.c_str());

        for (vfs::DirectoryEntry ent : dirList) {
            if ((ent.attributes != vfs::FileAttributes::Directory) && hasEnding(ent.fileName, ".mpf")) {
                fontFiles.push_back(fontFolder + "/" + ent.fileName);
                // TODO remove commented code
                LOG_INFO("font: %s", (fontFolder + "/" + ent.fileName).c_str());
            }
        }

        LOG_INFO("Total number of images: %u", static_cast<unsigned int>(fontFiles.size()));

        return fontFiles;
    }

    bool FontManager::init(std::string baseDirectory)
    {
        // load fonts from specified folder
        loadFonts(baseDirectory);

        return true;
    }

    FontManager &FontManager::getInstance()
    {

        static FontManager instance;

        return instance;
    }

    Font *FontManager::getFont(uint32_t id)
    {
        if (id >= fonts.size()) {
            LOG_ERROR("Font not found! id: %" PRIu32, id);
            return nullptr;
        }
        return fonts[id];
    }
    uint32_t FontManager::getFontID(const std::string &name)
    {
        bool found = false;
        uint32_t i;
        for (i = 0; i < fonts.size(); i++) {
            if (name.compare(fonts[i]->info.face) == 0) {
                found = true;
                break;
            }
        }
        if (!found) {
            LOG_ERROR("=> font not found: %s", name.c_str());
            LOG_ERROR("Using default!");
            return 0;
        }
        return i;
    }

} /* namespace gui */
