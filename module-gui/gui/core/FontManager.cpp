// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "FontManager.hpp"
#include "Common.hpp"   // for Status, Status::GUI_SUCCESS
#include "FontInfo.hpp" // for FontInfo
#include "RawFont.hpp"  // for RawFont
#include <log/log.hpp>  // for LOG_ERROR, LOG_INFO, LOG_WARN
#include <Utils.hpp>
#include <json11.hpp>
#include <filesystem>
#include <fstream>
#include <cstdio>

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
        fontFolder     = baseDirectory + "/fonts";
        fontMapFile    = fontFolder + "/fontmap.json";
        auto fontFiles = getFontsList();

        for (const auto &font : fontFiles) {
            loadFont(font.first, font.second);
        }
    }

    RawFont *FontManager::loadFont(const std::string &fontType, const std::string &path)
    {

        auto file = std::fopen(path.c_str(), "rb");
        if (file == nullptr) {
            LOG_ERROR("Failed to open file: %s", path.c_str());
            return nullptr;
        }

        const auto fileSize = std::filesystem::file_size(path);

        std::vector<uint8_t> fontData(fileSize, 0);

        std::ifstream input(path, std::ios::in | std::ifstream::binary);
        if (not input.is_open()) {
            return nullptr;
        }

        if (not input.read(reinterpret_cast<char *>(&fontData[0]), fontData.size())) {
            std::fclose(file);
            return nullptr;
        }

        const auto bytesRead = input.gcount();

        // close file
        std::fclose(file);
        if (static_cast<uintmax_t>(bytesRead) != fileSize) {
            LOG_ERROR("Failed to read all file");
            return nullptr;
        }

        // allocate memory for new font
        RawFont *rawfont = new RawFont();
        if (!rawfont) {
            return nullptr;
        }
        if (rawfont->load(&fontData[0]) != gui::Status::GUI_SUCCESS) {
            delete rawfont;
            return nullptr;
        }
        else {
            // set id and push it to vector
            rawfont->id = fonts.size();
            fonts.push_back(rawfont);
            fontMap.insert({fontType, rawfont->getName()});
        }
        return rawfont;
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

    std::map<std::string, std::string> FontManager::getFontsList()
    {
        auto fd = std::fopen(fontMapFile.c_str(), "r");
        if (fd == nullptr) {
            LOG_FATAL("Error during opening file %s", fontMapFile.c_str());
            return {};
        }

        uint32_t fsize     = std::filesystem::file_size(fontMapFile);
        auto fontmapString = std::make_unique<char[]>(fsize + 1);
        memset(fontmapString.get(), 0, fsize + 1);
        std::fread(fontmapString.get(), 1, fsize, fd);
        std::fclose(fd);

        json11::Json fontmapJson;
        std::string err;
        fontmapJson = json11::Json::parse(fontmapString.get(), err);
        if (!err.empty()) {
            LOG_ERROR("Failed parsing device string!");
            throw std::invalid_argument("Can't parse the file!");
        }

        auto fontmapObjects = fontmapJson.object_items();
        const auto infoJson = fontmapObjects["info"];
        defaultFontName     = infoJson["default_font_name"].string_value();
        defaultFontTypeName = infoJson["default_font_type_name"].string_value();

        const auto styleJson = fontmapObjects["style"];
        std::map<std::string, std::string> fontFiles;

        for (const auto &entry : styleJson.object_items()) {
            auto fontName = entry.second.string_value();
            if (!std::filesystem::is_regular_file(fontFolder + "/" + fontName)) {
                LOG_ERROR("Could not find font: %s", fontName.c_str());
            }
            else {
                LOG_INFO("Add font to list: %s - %s", entry.first.c_str(), fontName.c_str());
                fontFiles.insert({entry.first, fontFolder + "/" + fontName});
            }
        }
        LOG_INFO("Total number of fonts: %u", static_cast<unsigned int>(fontFiles.size()));
        return fontFiles;
    }

    auto FontManager::getFontName(const std::string &fontType) const -> std::string
    {
        return getFont(fontType)->getName();
    }

    bool FontManager::init(std::string baseDirectory)
    {
        loadFonts(baseDirectory);

        auto fallback_font = find(getFontName(defaultFontName));
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

    [[nodiscard]] auto FontManager::getFontByName(std::string_view name) const -> RawFont *
    {
        auto font = find(name);
        // default return first font
        if (font == nullptr && not fonts.empty()) {
#if DEBUG_MISSING_ASSETS == 1
            LOG_ERROR("=> font not found: %s using default", name.data());
#endif
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

    [[nodiscard]] auto FontManager::getFont(const std::string &fontType) const -> RawFont *
    {
        auto fontPath = fontMap.find(fontType);
        if (fontPath != fontMap.end()) {
            auto rawFont = find(fontPath->second);
            if (rawFont) {
                return rawFont;
            }
        }
        if (not fonts.empty()) {
#if DEBUG_MISSING_ASSETS == 1
            LOG_ERROR("=> font not found: %s using default", fontType.c_str());
#endif
            return fonts[0];
        }
        return nullptr;
    }

    [[nodiscard]] auto FontManager::getFont() const -> RawFont *
    {
        return getFont(defaultFontName);
    }

    auto FontManager::getDefaultFontName() const -> std::string
    {
        return defaultFontName;
    }
    auto FontManager::getDefaultFontTypeName() const -> std::string
    {
        return defaultFontTypeName;
    }

    auto FontManager::find(std::string_view name) const -> RawFont *
    {
        for (const auto &font : fonts) {
            if (name.compare(font->info.face) == 0) {
                return font;
            }
        }
        return nullptr;
    }

}; // namespace gui
