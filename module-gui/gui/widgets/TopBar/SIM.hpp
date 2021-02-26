// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Image.hpp"
#include "TopBar/StatusBarWidgetBase.hpp"
#include <common_data/EventStore.hpp>

namespace gui::top_bar
{
    class SIM : public StatusBarWidgetBase<Image>
    {
        Store::GSM::SIM current = Store::GSM::SIM::SIM_UNKNOWN;

      public:
        enum class SimIndicatorFlag
        {
            SIM_NO_FLAGS = 0, ///> default action, only sim blocked and sim error is displayed
            SIM_SHOW_ALL = 1  ///> includes sim1 and sim2 icon
        };
        SIM(Item *parent, uint32_t x, uint32_t y);

        /// check if sim set in state -> if not -> show new sim
        void update(const Store::GSM::SIM &state, SimIndicatorFlag flag = SimIndicatorFlag::SIM_NO_FLAGS);

      private:
        void setSimWithNumber(const char *sim, SimIndicatorFlag flag);
    };
} // namespace gui::top_bar
