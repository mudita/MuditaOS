// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SIM.hpp"

namespace gui::top_bar
{
    using namespace Store;

    constexpr auto sim1       = "sim1";       // sim 1 indicator
    constexpr auto sim2       = "sim2";       // sim 2 indicator
    constexpr auto simunknown = "simunknown"; // sim - unknown sim state indicator (i.e. no initialization was done)
    constexpr auto simfailed  = "simfail";    // sim - notification for sim failure

    SIM::SIM(Item *parent, uint32_t x, uint32_t y) : StatusBarWidgetBase(parent, x, y, 0, 0)
    {
        set(simunknown);
    }

    void SIM::update(const Store::GSM::SIM &state)
    {
        if (state == current) {
            return;
        }
        current = state;
        switch (current) {
        case GSM::SIM::SIM1:
            set(sim1);
            break;
        case GSM::SIM::SIM2:
            set(sim2);
            break;
        case GSM::SIM::SIM_FAIL:
            set(simfailed);
            break;
        case GSM::SIM::NONE:
            [[fallthrough]];
        case GSM::SIM::SIM_UNKNOWN:
            set(simunknown);
            break;
        }
    }
}; // namespace gui::top_bar
