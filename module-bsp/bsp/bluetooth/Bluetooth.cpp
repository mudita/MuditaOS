#include "Bluetooth.hpp"
#include <cstdarg>

namespace bsp {
    BTdev::BTdev(unsigned int in_size, unsigned int out_size) : flog(nullptr), in(in_size), out(out_size)
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

    Bluetopia::Bluetopia(unsigned int in_size, unsigned int out_size) : BTdev(in_size,out_size)
    {
    }

    unsigned long get_tick();
};
