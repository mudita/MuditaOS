// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "FontManager.hpp"
#include "Common.hpp"
#include "FontInfo.hpp"
#include "RawFont.hpp"
#include <log/log.hpp>
#include <json11.hpp>
#include <fstream>
#include <cstring>

namespace
{
    auto getFileSize(const std::string &path) -> std::uintmax_t
    {
        try {
            return std::filesystem::file_size(path);
        }
        catch (const std::filesystem::filesystem_error &e) {
            return 0;
        }
    }
} // namespace

namespace gui
{
    FontManager::~FontManager()
    {
        clear();
    }

    auto FontManager::init(const std::filesystem::path &baseDirectory) -> bool
    {
        loadFonts(baseDirectory);

        const auto fallbackFont = find(fallbackFontName);
        if (fallbackFont == nullptr) {
            return false;
        }

        for (auto &font : fonts) {
            font->setFallbackFont(fallbackFont);
        }

        initialized = true;
        return initialized;
    }

    auto FontManager::clear() -> void
    {
        for (auto font : fonts) {
            LOG_INFO("Deleting font '%s'", font->getName().c_str());
            delete font;
        }
        fonts.clear();
    }

    auto FontManager::getInstance() -> FontManager &
    {
        static FontManager instance;
        if (!instance.initialized) {
            LOG_WARN("Using uninitialized font manager will result in no fonts loaded");
        }
        return instance;
    }

    auto FontManager::getFont() const -> RawFont *
    {
        return getFont(defaultFontName);
    }

    auto FontManager::getFont(std::uint32_t num) const -> RawFont *
    {
        if (fonts.empty()) {
            return nullptr;
        }
        if (num > fonts.size()) {
            return fonts[0];
        }
        return fonts[num];
    }

    auto FontManager::getFont(const std::string &fontType) const -> RawFont *
    {
        const auto fontPath = fontMap.find(fontType);
        if (fontPath != fontMap.end()) {
            auto rawFont = find(fontPath->second);
            if (rawFont != nullptr) {
                return rawFont;
            }
        }
        if (!fonts.empty()) {
#if DEBUG_MISSING_ASSETS == 1
            LOG_ERROR("=> font not found: %s, using default", fontType.c_str());
#endif
            return fonts[0];
        }
        return nullptr;
    }

    auto FontManager::getFontByName(std::string_view name) const -> RawFont *
    {
        const auto font = find(name);

        // Default return first font
        if ((font == nullptr) && !fonts.empty()) {
#if DEBUG_MISSING_ASSETS == 1
            LOG_ERROR("=> font not found: %s, using default", std::string(name).c_str());
#endif
            return fonts[0];
        }
        return font;
    }

    auto FontManager::getFontName(const std::string &fontType) const -> std::string
    {
        return getFont(fontType)->getName();
    }

    auto FontManager::getDefaultFontFamilyName() const -> std::string
    {
        return defaultFontFamilyName;
    }

    auto FontManager::loadFont(const std::string &fontType, const std::filesystem::path &path) -> RawFont *
    {
        const auto fileSize = getFileSize(path);
        if (fileSize == 0) {
            return nullptr;
        }

        auto fontData = std::make_unique<std::uint8_t[]>(fileSize);

        std::ifstream input(path, std::ios::in | std::ifstream::binary);
        if (!input.is_open()) {
            return nullptr;
        }
        if (!input.read(reinterpret_cast<char *>(fontData.get()), static_cast<std::streamsize>(fileSize))) {
            return nullptr;
        }
        const auto bytesRead = static_cast<std::uintmax_t>(input.gcount());
        if (bytesRead != fileSize) {
            LOG_FATAL("Failed to read from file, expected %" PRIuMAX "B, got %" PRIuMAX "B", fileSize, bytesRead);
            return nullptr;
        }

        // Allocate memory for new font
        auto rawFont = new (std::nothrow) RawFont();
        if (rawFont == nullptr) {
            return nullptr;
        }

        if (rawFont->load(fontData.get()) != gui::Status::GUI_SUCCESS) {
            delete rawFont;
            return nullptr;
        }

        // Set id and push it to vector
        rawFont->id = fonts.size();
        fonts.push_back(rawFont);
        fontMap.emplace(fontType, rawFont->getName());
        return rawFont;
    }

    auto FontManager::getFontsList() -> std::map<std::string, std::string>
    {
        const auto fileSize = getFileSize(fontMapFile);
        if (fileSize == 0) {
            LOG_FATAL("File size is zero!");
            return {};
        }

        auto fontmapString = std::make_unique<char[]>(fileSize + 1);
        std::memset(fontmapString.get(), 0, fileSize + 1);

        std::ifstream input(fontMapFile, std::ios::in);
        if (!input.is_open()) {
            LOG_FATAL("Failed to open file '%s'", fontMapFile.c_str());
            return {};
        }
        if (!input.read(fontmapString.get(), static_cast<std::streamsize>(fileSize))) {
            return {};
        }
        const auto bytesRead = static_cast<std::uintmax_t>(input.gcount());
        if (bytesRead != fileSize) {
            LOG_FATAL("Failed to read from file '%s', expected %" PRIuMAX "B, got %" PRIuMAX "B",
                      fontMapFile.c_str(),
                      fileSize,
                      bytesRead);
            return {};
        }

        json11::Json fontmapJson;
        std::string err;
        fontmapJson = json11::Json::parse(fontmapString.get(), err);
        if (!err.empty()) {
            LOG_FATAL("Failed to parse font map string!");
            return {};
        }

        auto fontmapObjects   = fontmapJson.object_items();
        const auto &infoJson  = fontmapObjects["info"];
        fallbackFontName      = infoJson["fallback_font"].string_value();
        defaultFontFamilyName = infoJson["default_font_family"].string_value();
        defaultFontName       = infoJson["default_font"].string_value();

        const auto &styleJson = fontmapObjects["style"];
        std::map<std::string, std::string> fontFiles;

        for (const auto &entry : styleJson.object_items()) {
            const auto &fontName = entry.second.string_value();
            if (!std::filesystem::is_regular_file(fontFolder / fontName)) {
                LOG_WARN("Could not find font '%s'", fontName.c_str());
            }
            else {
                LOG_INFO("Add font to list: '%s' - '%s'", entry.first.c_str(), fontName.c_str());
                fontFiles.emplace(entry.first, fontFolder / fontName);
            }
        }
        LOG_INFO("Total number of fonts: %zu", fontFiles.size());
        return fontFiles;
    }

    auto FontManager::loadFonts(const std::filesystem::path &baseDirectory) -> void
    {
        fontFolder  = baseDirectory / "fonts";
        fontMapFile = fontFolder / "fontmap.json";

        const auto &fontFiles = getFontsList();
        for (const auto &font : fontFiles) {
            loadFont(font.first, font.second);
        }
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
} // namespace gui
