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

        bool emit(const std::string evt) {
            return hwEmit(evt);
        }

        auto gpio::read(enum Pin pin) -> bool
        {
            return hwGpioRead(pin);
        }
        void gpio::write(enum Pin pin, uint32_t state)
        {
            return hwGpioWrite(pin, state);
        }

        gpio::Pin gpio::to(int which)
        {
            auto what = static_cast<Pin>(which);
            switch (what)
            {
            case SIM_SELECT:
                return SIM_SELECT;
            case SIM_PRESENCE:
                return SIM_PRESENCE;
            default:
                return NONE;
            }
        }
    }; // namespace harness
};     // namespace bsp
