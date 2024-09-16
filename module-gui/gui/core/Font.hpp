// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include <string>

namespace gui
{
    class RawFont;

    /// Simple font interface for FontManager && RawFont
    class Font
    {
      public:
        enum class Weight
        {
            Light,
            Regular,
            Bold
        };

      public:
        Font(const std::string &name, unsigned size, Weight weight = Weight::Regular);
        explicit Font(unsigned size, Weight weight = Weight::Regular);
        explicit Font(RawFont *font);

        auto setFont(std::string name, unsigned size, Weight weight = Weight::Regular) -> void;
        auto setFont(unsigned size, Weight weight = Weight::Regular) -> void;

        auto setSize(unsigned size) -> void;
        auto setWeight(Weight weight) -> void;

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

      private:
        RawFont *font = nullptr;
        std::string name;
        unsigned size = 0;
        Weight weight = Weight::Regular;

        [[nodiscard]] auto toWeight(const std::string &val) -> Font::Weight;
    };
} // namespace gui

inline auto c_str(enum gui::Font::Weight w) -> const char *
{
    switch (w) {
    case gui::Font::Weight::Light:
        return "light";
    case gui::Font::Weight::Regular:
        return "regular";
    case gui::Font::Weight::Bold:
        return "bold";
    }
    return "";
}
