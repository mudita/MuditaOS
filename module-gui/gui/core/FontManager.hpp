// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <cstdint> // for uint32_t
#include <map>     // for map
#include <string>  // for string
#include <vector>  // for vector

namespace gui
{
    class RawFont;
} // namespace gui

namespace gui
{

    /// system font provider
    /// loads fonts from discs and build RawFonts to use
    class FontManager
    {
      private:
        bool initialized = false;

      protected:
        std::string fontFolder;
        std::string fontMapFile;
        std::vector<RawFont *> fonts;
        std::map<std::string, std::string> fontMap{};
        std::map<std::string, std::string> getFontsList();

        RawFont *loadFont(const std::string &font, const std::string &path);
        void loadFonts(std::string baseDirectory);

        FontManager() = default;

      public:
        FontManager(const FontManager &) = delete;
        void operator=(const FontManager &) = delete;

        bool init(std::string baseDirectory);
        void clear();
        static FontManager &getInstance();

        virtual ~FontManager();

        [[nodiscard]] auto getFont() const -> RawFont *;
        [[nodiscard]] auto getFont(const std::string &fontType) const -> RawFont *;
        [[nodiscard]] auto getFont(uint32_t num) const -> RawFont *;
        [[nodiscard]] auto getFontByName(std::string_view name) const -> RawFont *;
        [[nodiscard]] auto isInitialized() const
        {
            return initialized;
        }
        [[nodiscard]] auto getFontName(const std::string &font) const -> std::string;
        [[nodiscard]] auto getDefaultFontFamilyName() const -> std::string;

      private:
        std::string fallbackFontName{};
        std::string defaultFontFamilyName{};
        std::string defaultFontName{};

        [[nodiscard]] auto find(std::string_view name) const -> RawFont *;
    };
}; // namespace gui
