
#include "bsp/torch/torch.hpp"

#include "bsp/BoardDefinitions.hpp"

using namespace drivers;

static xQueueHandle qHandleIrq = NULL;

namespace bsp
{

    namespace torch
    {

        int32_t init(xQueueHandle qHandle)
        {

            qHandleIrq = qHandle;
            return 1;
        }

        bool isPresent(void)
        {
            return false;
        }

    } // namespace torch
} // namespace bsp
