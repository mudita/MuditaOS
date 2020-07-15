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

        [[nodiscard]] auto getFont(const std::string &name) const -> RawFont *;
        [[nodiscard]] auto getFont(uint32_t num) const -> RawFont *;
        [[nodiscard]] auto isInitialized() const
        {
            return initialized;
        }

      private:
        [[nodiscard]] auto find(const std::string &name) const -> RawFont *;
    };
}; // namespace gui
