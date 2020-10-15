#include <ctime>

extern "C"
{
    time_t FreeRTOS_time(time_t *pxTime)
    {
        return std::time(pxTime);
    }
}
