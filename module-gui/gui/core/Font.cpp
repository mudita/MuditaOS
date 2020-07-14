#include "Font.hpp"
#include "FontManager.hpp"  // for FontManager

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
        std::string raw_font_name = new_name + "_" + c_str(new_weight) + "_" + std::to_string(new_size);
        if (new_size != size || weight != new_weight || new_name != name) {
            font = FontManager::getInstance().getFont(raw_font_name);
        }
    }

    void Font::setFont(unsigned int size, Weight weight)
    {
        setFont((font_default_type), size, weight);
    }

    void Font::setSize(unsigned int new_size)
    {
        if (size != new_size) {
            size = new_size;
            setFont(name, size, weight);
        }
    }

    void Font::setWeight(Weight new_weight)
    {
        if (weight != new_weight) {
            weight = new_weight;
            setFont(name, size, weight);
        }
    }

    auto Font::raw() -> RawFont *
    {
        return font;
    }
}; // namespace gui
