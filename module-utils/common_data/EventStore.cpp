#include "EventStore.hpp"
#include <log/log.hpp>
#include <memory>
#include <mutex.hpp>
#include <source/version.hpp>

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

    OperatingSystem operatingSystem;

    const OperatingSystem &OperatingSystem::get()
    {
        return operatingSystem;
    }

    OperatingSystem &OperatingSystem::modify()
    {
        return operatingSystem;
    }

    void OperatingSystem::Version::initFromMemory()
    {
        sscanf(VERSION, "%d.%d.%d", &major, &minor, &patch);
        gitTag              = GIT_TAG;
        gitBranch           = GIT_BRANCH;
        gitRev              = GIT_REV;
        kernelVersionString = tskKERNEL_VERSION_NUMBER;
    }

    const json11::Json OperatingSystem::Version::toJson() const
    {
        json11::Json v = json11::Json::object{
            {"major", std::to_string(major)}, {"minor", std::to_string(minor)}, {"patch", std::to_string(patch)}};

        json11::Json git = json11::Json::object{{"branch", gitBranch}, {"tag", gitTag}, {"revision", gitRev}};

        json11::Json misc = json11::Json::object{{"codename", codename}, {"kernel", tskKERNEL_VERSION_NUMBER}};

        return json11::Json::object{{"version", v}, {"git", git}, {"misc", misc}};
    }

    OperatingSystem::Version OperatingSystem::Version::fromJson(const json11::Json &json)
    {
        Version v;

        v.major = json["version"]["major"].int_value();
        v.minor = json["version"]["minor"].int_value();
        v.patch = json["version"]["patch"].int_value();

        v.gitBranch = json["git"]["branch"].int_value();
        v.gitRev    = json["git"]["rev"].int_value();
        v.gitTag    = json["git"]["tag"].int_value();

        v.codename            = json["mic"]["codename"].int_value();
        v.kernelVersionString = json["mic"]["kernel"].int_value();
        return v;
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

    SignalStrength GSM::getSignalStrength()
    {
        cpp_freertos::LockGuard lock(mutex);

        return signalStrength;
    }

    bool GSM::simCardInserted()
    {
        return (sim == SIM::SIM1 || sim == SIM::SIM2);
    }
}; // namespace Store
