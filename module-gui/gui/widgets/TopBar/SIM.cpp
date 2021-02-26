// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SIM.hpp"

namespace gui
{
    using namespace Store;

    SIM::SIM(Item *parent, uint32_t x, uint32_t y) : Image(parent, x, y, 0, 0)
    {
        set(simunknown);
    }

    void SIM::show(Store::GSM::SIM state, SimIndicatorFlag flag)
    {
        if (state == current) {
            return;
        }
        setVisible(true);
        switch (state) {
        case GSM::SIM::SIM1:
            setSimWithNumber(sim1, flag);
            break;
        case GSM::SIM::SIM2:
            setSimWithNumber(sim2, flag);
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
    void SIM::setSimWithNumber(const char *sim, SimIndicatorFlag flag)
    {
        if (flag == SimIndicatorFlag::SIM_SHOW_ALL) {
            set(sim);
        }
        else {
            setVisible(false);
        }
    }
}; // namespace gui
