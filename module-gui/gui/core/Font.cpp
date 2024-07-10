// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Font.hpp"
#include "FontManager.hpp"
#include "RawFont.hpp"
#include <Utils.hpp>

namespace gui
{
    Font::Font(const std::string &name, unsigned size, Weight weight)
    {
        setFont(name, size, weight);
    }

    Font::Font(unsigned size, Weight weight) : Font(FontManager::getInstance().getDefaultFontFamilyName(), size, weight)
    {}

    Font::Font(RawFont *rawFont)
    {
        if (rawFont == nullptr) {
            rawFont = FontManager::getInstance().getFont(""); // Get default
        }

        auto rawFontName = rawFont->getName();
        auto parse       = [&]() {
            const auto pos = rawFontName.rfind('_');
            auto val       = rawFontName.substr(pos + 1, rawFontName.length());
            rawFontName.erase(pos);
            return val;
        };

        const auto rawFontSize   = utils::toNumeric(parse());
        const auto rawFontWeight = toWeight(parse());
        setFont(rawFontName, rawFontSize, rawFontWeight);
    }

    auto Font::setFont(std::string newName, unsigned newSize, Weight newWeight) -> void
    {
        bool update = false;
        auto set    = [&](auto &val, auto &new_val) {
            if (val != new_val) {
                val    = new_val;
                update = true;
            }
        };
        set(name, newName);
        set(size, newSize);
        set(weight, newWeight);
        if (update) {
            const auto &rawFontName = newName + "_" + c_str(newWeight) + "_" + std::to_string(newSize);
            font                    = FontManager::getInstance().getFontByName(rawFontName);
        }
    }

    auto Font::setFont(unsigned newSize, Weight newWeight) -> void
    {
        setFont((FontManager::getInstance().getDefaultFontFamilyName()), newSize, newWeight);
    }

    auto Font::setSize(unsigned newSize) -> void
    {
        setFont(name, newSize, weight);
    }

    auto Font::setWeight(Weight newWeight) -> void
    {
        setFont(name, size, newWeight);
    }

    auto Font::raw() -> RawFont *
    {
        return font;
    }

    auto Font::toWeight(const std::string &val) -> Font::Weight
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
} // namespace gui
