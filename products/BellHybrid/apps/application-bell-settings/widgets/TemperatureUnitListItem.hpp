// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <widgets/BellSideListItem.hpp>
#include <utf8/UTF8.hpp>

namespace gui
{
    class TextSpinner;

    class TemperatureUnitListItem : public BellSideListItem
    {
      public:
        explicit TemperatureUnitListItem(
            const UTF8 &topDesc = "", Length x = 0, Length y = 0, Length w = 0, Length h = 0);

        auto getUnitAsStr() const noexcept -> UTF8;

      private:
        TextSpinner *temperatureUnit{};
    };

} // namespace gui
