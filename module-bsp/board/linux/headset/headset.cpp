#include "headset.hpp"

namespace bsp
{
    namespace headset
    {

        int32_t Init(xQueueHandle qHandle)
        {
            return 1;
        }

        BaseType_t IRQHandler(void)
        {
            return 1;
        }

        void Handler(void)
        {}

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
