#include "Bluetooth.hpp"
#include <cstdarg>


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

    Bluetopia::Bluetopia(unsigned int in_size, unsigned int out_size, int threshold) : BluetoothCommon(in_size,out_size, threshold),rx_thread(0), thandle(NULL)
    {
    }

    Bluetopia::~Bluetopia()
    {
    }

    Bluetopia *Bluetopia::getInstance()
    {
        return Bluetopia::getInstance();
    }
};
