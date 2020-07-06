#include "InitializedFontManager.hpp"

namespace mockup
{
    auto fontManager() -> gui::FontManager &
    {
        auto &fontmanager = gui::FontManager::getInstance();
        if (!fontmanager.isInitialized()) {
            fontmanager.init("assets");
        }
        return fontmanager;
    }
} // namespace mockup
