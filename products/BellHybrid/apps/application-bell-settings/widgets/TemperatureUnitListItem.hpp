// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Temperature.hpp>
#include <widgets/BellSideListItem.hpp>
#include <widgets/spinners/Spinners.hpp>
#include <utf8/UTF8.hpp>

namespace gui
{
    class TemperatureUnitListItem : public BellSideListItem
    {
      public:
        explicit TemperatureUnitListItem(
            const UTF8 &topDesc = "", Length x = 0, Length y = 0, Length w = 0, Length h = 0);

        auto getUnitAsStr() const noexcept -> UTF8;
        auto setUnit(utils::temperature::Temperature::Unit unit) -> void;

      private:
        StringSpinner *temperatureUnit{};
    };

} // namespace gui
