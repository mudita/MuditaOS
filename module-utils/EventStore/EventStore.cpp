// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "EventStore.hpp"
#include <log/log.hpp>
#include <memory>
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

    cpp_freertos::MutexStandard GSM::mutex;

    GSM *GSM::get()
    {
        static auto ptr = new GSM();
        return ptr;
    }

    void GSM::setSignalStrength(const SignalStrength &signalStrength)
    {
        cpp_freertos::LockGuard lock(mutex);
        LOG_INFO("Setting signal strength to rssi = %d dBm (%d) : %d bars",
                 signalStrength.rssidBm,
                 signalStrength.rssi,
                 static_cast<int>(signalStrength.rssiBar));

        this->signalStrength = signalStrength;
    }

    SignalStrength GSM::getSignalStrength() const
    {
        cpp_freertos::LockGuard lock(mutex);
        return signalStrength;
    }

    void GSM::setNetwork(const Network &network)
    {
        cpp_freertos::LockGuard lock(mutex);
        this->network = network;
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
}; // namespace Store
