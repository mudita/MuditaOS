
#include "bsp/torch/torch.hpp"

#include "bsp/BoardDefinitions.hpp"

using namespace drivers;

static xQueueHandle qHandleIrq = NULL;

namespace bsp
{

    namespace torch
    {
        State state_simulated               = State::off;
        ColourTemperature currentColourTemp = warmest;

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
            if (colourTemp != no_change) {
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

        std::pair<bool, State> getState()
        {
            return std::make_pair(true, state_simulated);
        }

        ColourTemperature getColorTemp()
        {
            return currentColourTemp;
        }

        bool toggle()
        {
            return turn(getState().second == State::off ? State::on : State::off);
        };

        bool setCurrent(const unsigned short mA)
        {
            return true;
        }

    } // namespace torch
} // namespace bsp
