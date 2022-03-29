// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "bsp/torch/torch.hpp"
#include <log/log.hpp>

static xQueueHandle qHandleIrq = NULL;

namespace bsp
{

    namespace torch
    {
        State state_simulated               = State::off;
        ColourTemperature currentColourTemp = ColourTemperature::warmest;

        int32_t init(xQueueHandle qHandle)
        {

            qHandleIrq      = qHandle;
            state_simulated = State::off;
            return 1;
        }

        void deinit()
        {
            qHandleIrq = nullptr;
        }

        bool isPresent(void)
        {
            return true;
        }

        bool turn(State state, ColourTemperature colourTemp)
        {
            state_simulated = state;
            if (colourTemp != ColourTemperature::noChange) {
                currentColourTemp = colourTemp;
            }

            if (state == State::on) {
                LOG_INFO("Torch is ON \U0001f526 (%s)",
                         currentColourTemp == ColourTemperature::warmest ? "warm \U0001f987" : "cold \U0001f535");
            }
            else {
                LOG_INFO("Torch is OFF");
            }
            return true;
        }

        State getState()
        {
            return state_simulated;
        }

        ColourTemperature getColorTemp()
        {
            return currentColourTemp;
        }

        bool toggle()
        {
            return turn(getState() == State::off ? State::on : State::off);
        };

        bool setCurrent(const unsigned short mA)
        {
            return true;
        }

    } // namespace torch
} // namespace bsp
