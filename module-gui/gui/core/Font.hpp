// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <string>

namespace gui
{
    class RawFont;

    /// simple font interface for FontManager && RawFont
    class Font
    {
      public:
        enum class Weight
        {
            Light,
            Regular,
            Bold,
        };

      private:
        RawFont *font = nullptr;
        std::string name;
        unsigned int size = 0;
        Weight weight     = Weight::Regular;

      public:
        Font(std::string name, unsigned int size, Weight weight = Weight::Regular);
        Font(unsigned int size, Weight weight = Weight::Regular);
        Font(RawFont *font);
        void setFont(std::string name, unsigned int size, Weight weight = Weight::Regular);
        void setFont(unsigned int size, Weight weight = Weight::Regular);
        void setSize(unsigned int size);
        void setWeight(Weight weight);
        auto raw() -> RawFont *;
        [[nodiscard]] auto getSize() const
        {
            return size;
        }
        [[nodiscard]] auto getWeight() const
        {
            return weight;
        }
        [[nodiscard]] auto getName() const
        {
            return name;
        }
    };
}; // namespace gui

inline auto c_str(enum gui::Font::Weight w) -> const char *
{
    switch (w) {
    case gui::Font::Weight::Light:
        return "light";
    case gui::Font::Weight::Regular:
        return "regular";
    case gui::Font::Weight::Bold:
        return "bold";
    };
    return "";
};
