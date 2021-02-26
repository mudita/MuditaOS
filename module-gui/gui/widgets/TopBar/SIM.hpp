// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "../Image.hpp"
#include <common_data/EventStore.hpp>

namespace gui
{
    class SIM : public Image
    {
        Store::GSM::SIM current = Store::GSM::SIM::SIM_UNKNOWN;

      public:
        enum class SimIndicatorFlag
        {
            SIM_NO_FLAGS = 0, ///> default action, only sim blocked and sim error is displayed
            SIM_SHOW_ALL = 1  ///> includes sim1 and sim2 icon
        };
        /// create first image (sim unknown) and set it
        const unsigned int w   = 24;
        const unsigned int h   = 30;
        const char *sim1       = "sim1";       // sim 1 indicator
        const char *sim2       = "sim2";       // sim 2 indicator
        const char *simunknown = "simunknown"; // sim - unknown sim state indicator (i.e. no initialization was done)
        const char *simfailed  = "simfail";    // sim - notification for sim failure

        /// check if sim set in state -> if not -> show new sim
        void show(Store::GSM::SIM state, SimIndicatorFlag flag = SimIndicatorFlag::SIM_NO_FLAGS);
        SIM(Item *parent, uint32_t x, uint32_t y);

      private:
        void setSimWithNumber(const char *sim, SimIndicatorFlag flag);
    };
} // namespace gui
