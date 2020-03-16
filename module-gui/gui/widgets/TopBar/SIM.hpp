#pragma once

#include "../Image.hpp"
#include <common_data/EventStore.hpp>

namespace gui
{
    class SIM : public Image
    {
        Store::GSM::SIM current = Store::GSM::SIM::SIM_UNKNOWN;

      public:
        /// create first image (sim unknown) and set it
        const unsigned int w   = 32;
        const unsigned int h   = 24;
        const char *sim1       = "sim1";       // sim 1 indicator
        const char *sim2       = "sim2";       // sim 2 indicator
        const char *simunknown = "simunknown"; // sim - unknown sim state indicator (i.e. no initialization was done)
        const char *simfailed  = "simfail";    // sim - notification for sim failure

        /// check if sim set in state -> if not -> show new sim
        void show(Store::GSM::SIM state);
        SIM(Item *parent, uint32_t x, uint32_t y);
    };
} // namespace gui
