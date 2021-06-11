// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Font.hpp"
#include "FontManager.hpp" // for FontManager
#include "RawFont.hpp"
#include <log.hpp>
#include <algorithm>
#include <sstream>

namespace gui
{

    auto toWeight(const std::string &val) -> Font::Weight;

    Font::Font(std::string name, unsigned int size, Weight weight)
    {
        setFont(name, size, weight);
    }

    Font::Font(unsigned int size, Weight weight) : Font(font_default_type, size, weight)
    {}

    Font::Font(RawFont *rawfont)
    {
        if (rawfont == nullptr) {
            font = FontManager::getInstance().getFont(""); // get default
        }

        auto name  = rawfont->getName();
        auto pos   = name.npos;
        auto parse = [&]() {
            pos      = name.rfind('_');
            auto val = name.substr(pos + 1, name.length());
            name.erase(pos);
            return val;
        };

        unsigned int size = std::stoi(parse());
        Weight weight     = toWeight(parse());

        setFont(name, size, weight);
    }

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

    auto toWeight(const std::string &val) -> Font::Weight
    {
        if (val == c_str(Font::Weight::Regular)) {
            return Font::Weight::Regular;
        }
        else if (val == c_str(Font::Weight::Bold)) {
            return Font::Weight::Bold;
        }
        else if (val == c_str(Font::Weight::Light)) {
            return Font::Weight::Light;
        }
        return Font::Weight::Regular;
    }
}; // namespace gui
