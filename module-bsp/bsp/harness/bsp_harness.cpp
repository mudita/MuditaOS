#include "bsp_harness.hpp"

namespace bsp
{
    namespace harness
    {

        int Init(xQueueHandle qHandle)
        {
            return hwInit(qHandle);
        }

        std::string read()
        {
            return hwRead();
        }

        bool flush() {
            return hwFlush();
        }
    }; // namespace harness
};     // namespace bsp
