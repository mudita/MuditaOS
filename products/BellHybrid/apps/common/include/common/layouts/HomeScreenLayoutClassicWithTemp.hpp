// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include "HomeScreenLayoutClassic.hpp"

namespace style::homescreen_classic
{
    inline constexpr auto temperature_w{170U};
    inline constexpr auto temperature_h{102U};
} // namespace style::homescreen_classic

namespace gui
{
    class HomeScreenLayoutClassicWithTemp : public HomeScreenLayoutClassic
    {
      public:
        explicit HomeScreenLayoutClassicWithTemp(std::string name);

        auto setTemperature(utils::temperature::Temperature newTemp) -> void override;

      protected:
        auto buildInterface() -> void override;

        TextFixedSize *tempText{nullptr};
    };
} // namespace gui
