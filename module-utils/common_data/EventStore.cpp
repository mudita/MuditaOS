#include "EventStore.hpp"
#include <log/log.hpp>

namespace Store
{
    // if it grows bigger than these few variables - consider moving it to ram with i.e.
    // delayed construction singletone
    Battery battery;

    const Battery &Battery::get()
    {
        return battery;
    }

    Battery &Battery::modify()
    {
        return battery;
    }

    GSM *GSM::ptr = nullptr;

    GSM *GSM::get()
    {
        if (!ptr)
        {
            ptr = new GSM();
        }
        return ptr;
    }

    void GSM::setSignalStrength(SignalStrength &signalStrength)
    {
        LOG_INFO("Setting signal strenth to rssi = %d dBm (%d) : %u bars", signalStrength.rssidBm, signalStrength.rssi, signalStrength.rssiBar);
        ptr->signalStrength = signalStrength;
    }
}; // namespace Store
