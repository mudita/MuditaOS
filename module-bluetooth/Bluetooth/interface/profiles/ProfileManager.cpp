#include <service-bluetooth/ServiceBluetooth.hpp>
#include "ProfileManager.hpp"

namespace bluetooth
{

    ProfileManager::ProfileManager(sys::Service *ownerService) : ownerService(ownerService)
    {}

    auto ProfileManager::init() -> Error::Code
    {
        if (!initialized) {
            profilesList = {{AudioProfile::A2DP, std::make_shared<bluetooth::A2DP>()},
                            {AudioProfile::HSP, std::make_shared<bluetooth::HSP>()},
                            {AudioProfile::HFP, nullptr},
                            {AudioProfile::None, nullptr}};

            for (auto &[profileName, ptr] : profilesList) {
                if (ptr != nullptr) {
                    ptr->setOwnerService(ownerService);
                    ptr->init();
                }
            }
            currentProfilePtr = profilesList[AudioProfile::A2DP].get();

            if (auto serviceBt = dynamic_cast<ServiceBluetooth *>(ownerService); serviceBt != nullptr) {
                serviceBt->profileManagerPtr = this;
            }
            initialized = true;
        }
        return Error::Success;
    }

    auto ProfileManager::connect(bd_addr_t address) -> Error::Code
    {
        bd_addr_copy(remoteAddr, address);
        for (auto &[profileName, ptr] : profilesList) {
            if (ptr != nullptr) {
                ptr->setDeviceAddress(remoteAddr);
                ptr->connect();
            }
        }
        return Error::Success;
    }

    auto ProfileManager::disconnect() -> Error::Code
    {
        for (auto &[profileName, ptr] : profilesList) {
            if (ptr != nullptr) {
                ptr->disconnect();
            }
        }
        return Error::Success;
    }

    auto ProfileManager::switchProfile(AudioProfile profile) -> Error::Code
    {
        if (profilesList[profile] == nullptr) {
            LOG_ERROR("Invalid profile!");
            return Error::SystemError;
        }
        if (currentProfilePtr == profilesList[profile].get()) {
            return Error::Success;
        }
        stop();
        currentProfilePtr = profilesList[profile].get();
        start();
        return Error::Success;
    }
    auto ProfileManager::start() -> Error::Code
    {
        currentProfilePtr->start();
        return Error::Success;
    }
    auto ProfileManager::stop() -> Error::Code
    {
        currentProfilePtr->stop();
        return Error::Success;
    }

} // namespace bluetooth
