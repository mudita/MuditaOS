// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <service-bluetooth/ServiceBluetooth.hpp>
#include "ProfileManager.hpp"
#include <GAP/GAP.hpp>

namespace bluetooth
{

    ProfileManager::ProfileManager(sys::Service *ownerService) : ownerService(ownerService)
    {}

    auto ProfileManager::init() -> Error::Code
    {
        if (!initialized) {
            profilesList = {{AudioProfile::A2DP, std::make_shared<bluetooth::A2DP>()},
                            {AudioProfile::HSP, nullptr},
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
        ///> connect to remote only if we are sure that remote side supports our profiles
        if (GAP::isServiceSupportedByRemote(address, TYPE_OF_SERVICE::RENDERING)) {
            auto profilePtr = profilesList[AudioProfile::A2DP].get();
            if (profilePtr != nullptr) {
                LOG_DEBUG("Connecting device %s to A2DP", bd_addr_to_str(address));
                profilePtr->setDeviceAddress(remoteAddr);
                profilePtr->connect();
            }
        }
        if (GAP::isServiceSupportedByRemote(address, TYPE_OF_SERVICE::AUDIO)) {
            auto profilePtr = profilesList[AudioProfile::HSP].get();
            if (profilePtr != nullptr) {
                LOG_DEBUG("Connecting device %s to HSP", bd_addr_to_str(address));
                profilePtr->setDeviceAddress(remoteAddr);
                profilePtr->connect();
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

    auto ProfileManager::setAudioDevice(std::shared_ptr<BluetoothAudioDevice> device) -> Error::Code
    {
        if (currentProfilePtr == nullptr) {
            return Error::NotReady;
        }

        currentProfilePtr->setAudioDevice(std::move(device));
        return Error::Success;
    }
} // namespace bluetooth
