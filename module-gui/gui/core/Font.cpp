#include "Font.hpp"
#include "FontManager.hpp" // for FontManager

namespace gui
{

    Font::Font(std::string name, unsigned int size, Weight weight)
    {
        setFont(name, size, weight);
    }

    Font::Font(unsigned int size, Weight weight) : Font(font_default_type, size, weight)
    {}

    void Font::setFont(std::string new_name, unsigned int new_size, Weight new_weight)
    {
        bool update = false;
        auto set    = [&](auto &val, auto &new_val) {
            if (val != new_val) {
                val    = new_val;
                update = true;
            }
        };
        set(name, new_name);
        set(size, new_size);
        set(weight, new_weight);
        if (update) {
            std::string raw_font_name = new_name + "_" + c_str(new_weight) + "_" + std::to_string(new_size);
            font                      = FontManager::getInstance().getFont(raw_font_name);
        }
    }

    void Font::setFont(unsigned int size, Weight weight)
    {
        setFont((font_default_type), size, weight);
    }

    void Font::setSize(unsigned int new_size)
    {
        setFont(name, new_size, weight);
    }

    void Font::setWeight(Weight new_weight)
    {
        setFont(name, size, new_weight);
    }

    auto Font::raw() -> RawFont *
    {
        return font;
    }
}; // namespace gui
