#include "headset.hpp"

namespace bsp
{
    namespace headset
    {

        int32_t Init(xQueueHandle qHandle)
        {
            return 1;
        }

        bool Handler(uint8_t notification)
        {
            return false;
        }

        bool IsInserted(void)
        {
            return false;
        }

        int32_t Deinit(void)
        {
            return 1;
        }
    } // namespace headset
} // namespace bsp
