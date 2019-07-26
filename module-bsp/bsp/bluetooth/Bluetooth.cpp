#include "Bluetooth.hpp"
#include <cstdarg>

#if defined(TARGET_RT1051)
#include "bluetooth/bsp_bt.hpp"
#elif defined(TARGET_Linux)
#include "bluetooth/bsp_bt.hpp"
#else
#error "Unsupported target"
#endif

namespace bsp {
    BTdev::BTdev(unsigned int in_size, unsigned int out_size, int threshold) : flog(nullptr), in(in_size, threshold), out(out_size, threshold)
    {
        is_open = false;
    }

    BTdev::~BTdev()
    {
    }

    void BTdev::log(LogLvl lvl,const char* val, ...)
    {
        if(loglvl>=lvl && flog) {
            va_list args;
            va_start(args, val);
            flog(val,args);
            va_end(args);
        }
    }

    Bluetopia::Bluetopia(unsigned int in_size, unsigned int out_size, int threshold) : BTdev(in_size,out_size, threshold),rx_thread(0), thandle(NULL)
    {
    }

    Bluetopia::~Bluetopia()
    {
    }

    Bluetopia *Bluetopia::getInstance()
    {
        return BluetopiaHW::getInstance();
    }
};
