#include "InitializedFontManager.hpp"
#include <mutex>

namespace mockup
{
    auto guard = std::mutex();

    auto fontManager() -> gui::FontManager &
    {
        std::lock_guard<std::mutex> scoped(guard);
        auto &fontmanager = gui::FontManager::getInstance();
        if (!fontmanager.isInitialized()) {
            fontmanager.init("assets");
        }
        return fontmanager;
    }
} // namespace mockup
