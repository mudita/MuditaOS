// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <cstdint>
#include <map>
#include <string>
#include <vector>
#include <filesystem>

namespace gui
{
    class RawFont;
} // namespace gui

namespace gui
{
    /// System font provider
    /// Loads fonts from disks and build RawFonts to use
    class FontManager
    {
      public:
        FontManager(const FontManager &) = delete;
        auto operator=(const FontManager &) -> void = delete;

        virtual ~FontManager();

        auto init(const std::filesystem::path &baseDirectory) -> bool;
        auto clear() -> void;
        static auto getInstance() -> FontManager &;

        [[nodiscard]] auto getFont() const -> RawFont *;
        [[nodiscard]] auto getFont(const std::string &fontType) const -> RawFont *;
        [[nodiscard]] auto getFont(std::uint32_t num) const -> RawFont *;
        [[nodiscard]] auto getFontByName(std::string_view name) const -> RawFont *;

        [[nodiscard]] auto isInitialized() const
        {
            return initialized;
        }

        [[nodiscard]] auto getFontName(const std::string &font) const -> std::string;
        [[nodiscard]] auto getDefaultFontFamilyName() const -> std::string;

      protected:
        std::filesystem::path fontFolder;
        std::filesystem::path fontMapFile;
        std::vector<RawFont *> fonts;
        std::map<std::string, std::string> fontMap{};

        auto getFontsList() -> std::map<std::string, std::string>;

        auto loadFont(const std::string &font, const std::filesystem::path &path) -> RawFont *;
        auto loadFonts(const std::filesystem::path &baseDirectory) -> void;

        FontManager() = default;

      private:
        bool initialized{false};
        std::string fallbackFontName{};
        std::string defaultFontFamilyName{};
        std::string defaultFontName{};

        [[nodiscard]] auto find(std::string_view name) const -> RawFont *;
    };
} // namespace gui
