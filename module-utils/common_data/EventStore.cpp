#include "EventStore.hpp"
#include "time/ScopedTime.hpp"
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
    cpp_freertos::MutexStandard mutex;

    GSM *GSM::get()
    {
        auto time = utils::time::Scoped("GSM get");
        cpp_freertos::LockGuard lock(mutex);
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

        ptr->signalStrength = signalStrength;
    }

    SignalStrength GSM::getSignalStrength()
    {
        cpp_freertos::LockGuard lock(mutex);

        return ptr->signalStrength;
    }
}; // namespace Store
