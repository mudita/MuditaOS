
#include "bsp/torch/torch.hpp"

#include "bsp/BoardDefinitions.hpp"

using namespace drivers;

static xQueueHandle qHandleIrq = NULL;

namespace bsp
{

    namespace torch
    {
        State state_simulated = State::off;

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

        bool turn(State state)
        {
            state_simulated = state;
            LOG_INFO("Torch is %s", (state == State::on) ? "ON \U0001f4a1 \U0001f526" : "OFF");
            return true;
        }

        std::pair<bool, State> getState()
        {
            return std::make_pair(true, state_simulated);
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
