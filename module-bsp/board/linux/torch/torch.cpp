
#include "bsp/torch/torch.hpp"

#include "bsp/BoardDefinitions.hpp"

using namespace drivers;

static xQueueHandle qHandleIrq = NULL;

namespace bsp
{

    namespace torch
    {
        bool state_simulated = false;

        int32_t init(xQueueHandle qHandle)
        {

            qHandleIrq      = qHandle;
            state_simulated = false;
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

        bool turn(bool state)
        {
            state_simulated = state;
            LOG_INFO("Torch is %s", (state == true) ? "ON \U0001f4a1 \U0001f526" : "OFF");
            return true;
        }

        bool isOn()
        {
            return state_simulated;
        }

        bool setCurrent(const unsigned short mA)
        {
            return true;
        }

    } // namespace torch
} // namespace bsp
