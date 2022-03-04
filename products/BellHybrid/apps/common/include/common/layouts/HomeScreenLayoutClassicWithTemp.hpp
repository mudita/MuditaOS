// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "HomeScreenLayoutClassic.hpp"

namespace style::homescreen_classic
{
    constexpr inline auto temperature_w = 170U;
    constexpr inline auto temperature_h = 102U;
} // namespace style::homescreen_classic

namespace gui
{

    class HomeScreenLayoutClassicWithTemp : public HomeScreenLayoutClassic
    {
      public:
        HomeScreenLayoutClassicWithTemp(std::string name);

        auto setTemperature(utils::temperature::Temperature newTemp) -> void override;

      protected:
        auto buildInterface() -> void override;

        TextFixedSize *tempText{};
    };
}; // namespace gui
