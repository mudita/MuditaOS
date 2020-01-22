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

        bool emit(const std::string &evt) {
            return hwEmit(evt);
        }
    }; // namespace harness
};     // namespace bsp
