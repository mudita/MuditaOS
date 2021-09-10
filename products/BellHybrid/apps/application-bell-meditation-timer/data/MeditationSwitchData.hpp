// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <SwitchData.hpp>

#include "../data/MeditationItem.hpp"

namespace meditation
{
    inline constexpr auto MEDITATION_SWITCH_DATA_STR = "MeditationSwitchData";

    class MeditationSwitchData : public gui::SwitchData
    {
      public:
        MeditationSwitchData() = delete;
        explicit MeditationSwitchData(MeditationItem item)
            : gui::SwitchData(MEDITATION_SWITCH_DATA_STR), item{std::move(item)}
        {}
        virtual ~MeditationSwitchData() = default;

        [[nodiscard]] auto getMeditationItem() const noexcept -> const MeditationItem &
        {
            return item;
        }

      private:
        MeditationItem item;
    };
} // namespace meditation
