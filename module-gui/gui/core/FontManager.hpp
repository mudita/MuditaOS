// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <cstdint> // for uint32_t
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
        std::vector<RawFont *> fonts;
        std::vector<std::string> getFontsList();

        RawFont *loadFont(std::string filename);
        void loadFonts(std::string baseDirectory);

        FontManager() = default;

      public:
        FontManager(const FontManager &) = delete;
        void operator=(const FontManager &) = delete;

        bool init(std::string baseDirectory);
        void clear();
        static FontManager &getInstance();

        virtual ~FontManager();

        [[nodiscard]] auto getFont(std::string_view name = defaultFontName) const -> RawFont *;
        [[nodiscard]] auto getFont(uint32_t num) const -> RawFont *;
        [[nodiscard]] auto isInitialized() const
        {
            return initialized;
        }

        static constexpr auto defaultFontName{"gt_pressura_regular_27"};

      private:
        [[nodiscard]] auto find(std::string_view name) const -> RawFont *;
    };
}; // namespace gui
