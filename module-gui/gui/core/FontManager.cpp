#include "FontManager.hpp"
#include "Common.hpp"   // for Status, Status::GUI_SUCCESS
#include "FontInfo.hpp" // for FontInfo
#include "RawFont.hpp"  // for RawFont
#include "log/log.hpp"  // for LOG_ERROR, LOG_INFO, LOG_WARN
#include "vfs.hpp"      // for vfs, vfs::DirectoryEntry, vfs::FileAttributes

namespace style::window::font
{
    const inline std::string default_fallback_font = "dejavu_sans_bold_27";
}

namespace gui
{

    FontManager::~FontManager()
    {
        clear();
    }

    void FontManager::clear()
    {
        for (RawFont *font : fonts) {
            delete font;
        }
        fonts.clear();
    }

    void FontManager::loadFonts(std::string baseDirectory)
    {
        fontFolder                         = baseDirectory + "/fonts";
        std::vector<std::string> fontFiles = getFontsList();

        for (std::string fontName : fontFiles) {
            loadFont(fontName);
        }
    }

    RawFont *FontManager::loadFont(std::string filename)
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
        RawFont *font = new RawFont();
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
            }
        }

        LOG_INFO("Total number of fonts: %u", static_cast<unsigned int>(fontFiles.size()));
        return fontFiles;
    }

    bool FontManager::init(std::string baseDirectory)
    {
        loadFonts(baseDirectory);

        auto fallback_font = find(style::window::font::default_fallback_font);
        if (fallback_font != nullptr) {
            for (auto font : fonts) {
                font->setFallbackFont(fallback_font);
            }
        }
        initialized = true;
        return initialized;
    }

    FontManager &FontManager::getInstance()
    {

        static FontManager instance;
        if (!instance.initialized) {
            LOG_WARN("Using uninitialized font manager will result in no font loaded");
        }
        return instance;
    }

    [[nodiscard]] auto FontManager::getFont(const std::string &name) const -> RawFont *
    {
        auto font = find(name);
        // default return first font
        if (font == nullptr && fonts.size() > 0) {
            LOG_ERROR("=> font not found: %s using default", name.c_str());
            return fonts[0];
        }
        return font;
    }

    [[nodiscard]] auto FontManager::getFont(uint32_t num) const -> RawFont *
    {
        if (fonts.size() == 0) {
            return nullptr;
        }
        if (num > fonts.size()) {
            return fonts[0];
        }
        return fonts[num];
    }

    auto FontManager::find(const std::string &name) const -> RawFont *
    {
        for (auto &font : fonts) {
            if (name.compare(font->info.face) == 0) {
                return font;
            }
        }
        return nullptr;
    }
}; // namespace gui
