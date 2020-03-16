#include "SIM.hpp"

namespace gui
{
    using namespace Store;

    SIM::SIM(Item *parent, uint32_t x, uint32_t y) : Image(parent, x, y, 0, 0)
    {
        set(simunknown);
    }

    void SIM::show(Store::GSM::SIM state)
    {
        if (state == current) {
            return;
        }
        switch (state) {
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
}; // namespace gui
