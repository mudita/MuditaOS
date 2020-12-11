#include "Bluetooth.hpp"
#include <cstdarg>


namespace bsp {
    BTdev::BTdev() : flog(nullptr)
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

};
