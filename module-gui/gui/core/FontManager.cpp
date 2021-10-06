// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "FontManager.hpp"
#include "Common.hpp"   // for Status, Status::GUI_SUCCESS
#include "FontInfo.hpp" // for FontInfo
#include "RawFont.hpp"  // for RawFont
#include <log/log.hpp>  // for LOG_ERROR, LOG_INFO, LOG_WARN
#include <Utils.hpp>
#include <filesystem>
#include <cstdio>

namespace style::window::font
{
    inline constexpr auto default_fallback_font = "dejavu_sans_bold_27";
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

        auto file = std::fopen(filename.c_str(), "rb");
        if (file == nullptr) {
            return nullptr;
        }

        auto fileSize = std::filesystem::file_size(filename);
        std::rewind(file);

        char *fontData = new char[fileSize];
        if (fontData == nullptr) {
            std::fclose(file);
            LOG_ERROR(" Failed to allocate temporary font buffer");
            return nullptr;
        }

        // read data to buffer
        auto bytesRead = std::fread(fontData, 1, fileSize, file);

        // close file
        std::fclose(file);
        if (static_cast<uintmax_t>(bytesRead) != fileSize) {
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

        for (const auto &entry : std::filesystem::directory_iterator(fontFolder)) {
            if (!std::filesystem::is_directory(entry) && entry.path().extension() == ".mpf") {
                fontFiles.push_back(entry.path().string());
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

    [[nodiscard]] auto FontManager::getFont(std::string_view name) const -> RawFont *
    {
        auto font = find(name);
        // default return first font
        if (font == nullptr && fonts.size() > 0) {
            LOG_ERROR("=> font not found: %s using default", name.data());
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
