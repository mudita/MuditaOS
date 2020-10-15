#include <module-utils/log/log.hpp>

namespace bsp
{
    namespace vibrator
    {

        void init()
        {}
        void enable()
        {
            LOG_DEBUG("vibration starts\t\U0001f7e2\U0001f4f3");
        }
        void disable()
        {
            LOG_DEBUG("vibration ends  \t\U0001f6d1\U0001f4f3");
        }
        void deinit()
        {}
    } // namespace vibrator
} // namespace bsp
