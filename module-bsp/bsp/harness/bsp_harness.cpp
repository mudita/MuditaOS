#include "bsp_harness.hpp"

#if defined(TARGET_RT1051)
#include "board/rt1051/bsp/harness/rt1051_harness.hpp"
#elif defined(TARGET_Linux)
#include "cellular/linux_cellular.hpp"
#else
#error "Unsupported target"
#endif

namespace bsp
{
    namespace harness
    {

        int Init(xQueueHandle qHandle)
        {
            return hwInit(qHandle);
        }
    }; // namespace harness
};     // namespace bsp
