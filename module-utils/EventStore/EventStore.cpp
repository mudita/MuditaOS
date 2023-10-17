// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "EventStore.hpp"
#include <log/log.hpp>
#include <memory>
#include <mutex.hpp>

namespace Store
{
    // if it grows bigger than these few variables - consider moving it to ram with i.e.
    // delayed construction singleton
    Battery battery;
    bool Battery::updated = true;

    const Battery &Battery::get()
    {
        return battery;
    }

    Battery &Battery::modify()
    {
        Battery::setUpdated();
        return battery;
    }

    void Battery::setUpdated()
    {
        battery.updated = true;
    }

    bool Battery::takeUpdated()
    {
        if (battery.updated) {
            battery.updated = false;
            return true;
        }
        return false;
    }

    cpp_freertos::MutexStandard GSM::mutex;

    GSM *GSM::get()
    {
        static auto ptr = new GSM();
        return ptr;
    }

    void GSM::setSignalStrength(const SignalStrength &newSignalStrength)
    {
        cpp_freertos::LockGuard lock(mutex);
        LOG_INFO("Setting signal strength to rssi = %d dBm (%d) : %d bars",
                 newSignalStrength.rssidBm,
                 newSignalStrength.rssi,
                 static_cast<int>(newSignalStrength.rssiBar));

        signalStrength = newSignalStrength;
    }

    SignalStrength GSM::getSignalStrength() const
    {
        cpp_freertos::LockGuard lock(mutex);
        return signalStrength;
    }

    void GSM::setNetwork(const Network &newNetwork)
    {
        cpp_freertos::LockGuard lock(mutex);
        network = newNetwork;
    }

    Network GSM::getNetwork() const
    {
        cpp_freertos::LockGuard lock(mutex);
        return network;
    }

    bool GSM::simCardInserted()
    {
        cpp_freertos::LockGuard lock(mutex);
        return (sim == SIM::SIM1 || sim == SIM::SIM2);
    }

    void GSM::setNetworkOperatorName(const std::string &newNetworkOperatorName)
    {
        cpp_freertos::LockGuard lock(mutex);
        networkOperatorName = newNetworkOperatorName;
    }

    std::string GSM::getNetworkOperatorName() const
    {
        cpp_freertos::LockGuard lock(mutex);
        return networkOperatorName;
    }

    void GSM::setTethering(const Tethering &newTethering)
    {
        cpp_freertos::LockGuard lock(mutex);
        tethering = newTethering;
    }

    Tethering GSM::getTethering() const
    {
        cpp_freertos::LockGuard lock(mutex);
        return tethering;
    }
}; // namespace Store
