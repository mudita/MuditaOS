
#include "bsp/magnetometer/magnetometer.hpp"

#include "bsp/BoardDefinitions.hpp"

using namespace drivers;

static xQueueHandle qHandleIrq = NULL;

namespace bsp
{

    namespace magnetometer
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

        bsp::Board GetBoard(void)
        {
            return bsp::Board::Linux;
        }
    } // namespace magnetometer
} // namespace bsp
