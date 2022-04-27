// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
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

            musicProfilePtr = dynamic_cast<MusicProfile *>(profilesList[AudioProfile::A2DP].get());
            callProfilePtr  = dynamic_cast<CallProfile *>(profilesList[AudioProfile::HFP].get());

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

    auto ProfileManager::start() -> Error::Code
    {
        musicProfilePtr->start();
        return Error::Success;
    }

    auto ProfileManager::stop() -> Error::Code
    {
        musicProfilePtr->stop();
        return Error::Success;
    }

    auto ProfileManager::startRinging() -> Error::Code
    {
        return callProfilePtr->startRinging();
    }

    auto ProfileManager::stopRinging() -> Error::Code
    {
        return callProfilePtr->stopRinging();
    }

    auto ProfileManager::initializeCall() -> Error::Code
    {
        return callProfilePtr->initializeCall();
    }
    auto ProfileManager::terminateCall() -> Error::Code
    {
        return callProfilePtr->terminateCall();
    }

    auto ProfileManager::setAudioDevice(std::shared_ptr<BluetoothAudioDevice> device) -> Error::Code
    {
        auto profileType = device->getProfileType();

        if (profilesList[profileType] == nullptr) {
            return Error::NotReady;
        }

        profilesList[profileType]->setAudioDevice(device);
        LOG_ERROR("AudioDevice for profile: %s set!", magic_enum::enum_name(profileType).data());
        return Error::Success;
    }
    auto ProfileManager::callAnswered() -> Error::Code
    {
        return callProfilePtr->callActive();
    }
    auto ProfileManager::setIncomingCallNumber(const DataVariant &data) -> Error::Code
    {
        auto number = std::get<utils::PhoneNumber::View>(data);
        if (callProfilePtr) {
            return callProfilePtr->setIncomingCallNumber(number.getE164());
        }
        LOG_ERROR("No profile, returning!");
        return Error::NotReady;
    }
    auto ProfileManager::setSignalStrengthData(const DataVariant &data) -> Error::Code
    {
        auto signalData = std::get<Store::SignalStrength>(data);
        if (callProfilePtr) {
            return callProfilePtr->setSignalStrength(static_cast<int>(signalData.rssiBar));
        }
        LOG_ERROR("No profile, returning!");
        return Error::NotReady;
    }
    auto ProfileManager::setOperatorNameData(const DataVariant &data) -> Error::Code
    {
        auto operatorName = std::get<OperatorName>(data);
        if (callProfilePtr) {
            return callProfilePtr->setOperatorName(operatorName.getName());
        }
        LOG_ERROR("No profile, returning!");
        return Error::NotReady;
    }
    auto ProfileManager::setBatteryLevelData(const DataVariant &data) -> Error::Code
    {
        auto batteryLevel = std::get<BatteryLevel>(data);
        if (callProfilePtr) {
            return callProfilePtr->setBatteryLevel(batteryLevel);
        }
        LOG_ERROR("No profile, returning!");
        return Error::NotReady;
    }
    auto ProfileManager::setNetworkStatusData(const DataVariant &data) -> Error::Code
    {
        auto status = std::get<Store::Network::Status>(data);
        if (callProfilePtr) {
            switch (status) {
            case Store::Network::Status::RegisteredRoaming:
                callProfilePtr->setRoamingStatus(true);
                [[fallthrough]];
            case Store::Network::Status::RegisteredHomeNetwork:
                return callProfilePtr->setNetworkRegistrationStatus(true);
            default:
                return callProfilePtr->setNetworkRegistrationStatus(false);
            }
        }
        LOG_ERROR("No profile, returning!");
        return Error::NotReady;
    }
    auto ProfileManager::callStarted(const DataVariant &data) -> Error::Code
    {
        if (callProfilePtr) {
            auto number = std::get<utils::PhoneNumber::View>(data);
            return callProfilePtr->callStarted(number.getE164());
        }
        LOG_ERROR("No profile, returning!");
        return Error::NotReady;
    }

} // namespace bluetooth
