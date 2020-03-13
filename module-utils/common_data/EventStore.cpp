#include "EventStore.hpp"
#include <log/log.hpp>
#include <mutex.hpp>

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

    cpp_freertos::MutexStandard mutex;

    GSM *GSM::get()
    {
        cpp_freertos::LockGuard lock(mutex);
        static GSM *ptr = nullptr;
        if (ptr == nullptr)
        {
            ptr = new GSM();
        }
        return ptr;
    }

    void GSM::setSignalStrength(const SignalStrength &signalStrength)
    {
        cpp_freertos::LockGuard lock(mutex);
        LOG_INFO("Setting signal strenth to rssi = %d dBm (%d) : %u bars", signalStrength.rssidBm, signalStrength.rssi, signalStrength.rssiBar);

        get()->signalStrength = signalStrength;
    }

    SignalStrength GSM::getSignalStrength()
    {
        cpp_freertos::LockGuard lock(mutex);

        return get()->signalStrength;
    }
}; // namespace Store
