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
                            {AudioProfile::HFP, std::make_shared<bluetooth::HFP>()},
                            {AudioProfile::None, nullptr}};

            for (auto &[profileName, ptr] : profilesList) {
                if (ptr != nullptr) {
                    ptr->setOwnerService(ownerService);
                    ptr->init();
                }
            }
            // TODO profile selection based on capabilities and priority?
            // audio & capa cell & HSP & HFP  & A2DP-> HFP
            // audio & capa cell & HSP  & A2DP-> HSP
            // audio & HSP & HFP & A2DP -> A2DP
            currentProfilePtr = profilesList[AudioProfile::A2DP].get();

            if (auto serviceBt = dynamic_cast<ServiceBluetooth *>(ownerService); serviceBt != nullptr) {
                serviceBt->profileManagerPtr = this;
            }
            initialized = true;
        }
        return Error::Success;
    }

    auto ProfileManager::connect(const Devicei &device) -> Error::Code
    {
        for (auto &[profileName, ptr] : profilesList) {
            if (ptr != nullptr) {
                ptr->setDevice(device);
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

    auto ProfileManager::startRinging() -> Error::Code
    {
        switchProfile(AudioProfile::HFP);
        return currentProfilePtr->startRinging();
    }

    auto ProfileManager::stopRinging() -> Error::Code
    {
        return currentProfilePtr->stopRinging();
    }

    auto ProfileManager::initializeCall() -> Error::Code
    {
        switchProfile(AudioProfile::HFP);
        return currentProfilePtr->initializeCall();
    }

    auto ProfileManager::setAudioDevice(std::shared_ptr<BluetoothAudioDevice> device) -> Error::Code
    {
        auto profileType = device->getProfileType();

        if (currentProfilePtr == nullptr || profilesList[profileType] == nullptr) {
            return Error::NotReady;
        }

        profilesList[profileType]->setAudioDevice(device);
        return switchProfile(profileType);
    }
    auto ProfileManager::callAnswered() -> Error::Code
    {
        return currentProfilePtr->callAnswered();
    }
    auto ProfileManager::setIncomingCallNumber(const std::string &num) -> Error::Code
    {
        if (currentProfilePtr) {
            return currentProfilePtr->setIncomingCallNumber(num);
        }
        LOG_ERROR("No profile, returning!");
        return Error::NotReady;
    }

} // namespace bluetooth
