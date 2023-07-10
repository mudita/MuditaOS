// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <service-bluetooth/ServiceBluetooth.hpp>
#include "ProfileManager.hpp"
#include <GAP/GAP.hpp>

namespace bluetooth
{
    ProfileManager::ProfileManager(sys::Service *ownerService) : ownerService(ownerService)
    {}

    auto ProfileManager::init() -> Result::Code
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
        return Result::Code::Success;
    }

    auto ProfileManager::connect(const Devicei &device) -> Result::Code
    {
        for (auto &[profileName, ptr] : profilesList) {
            if (ptr != nullptr) {
                ptr->setDevice(device);
                ptr->connect();
            }
        }
        return Result::Code::Success;
    }

    auto ProfileManager::disconnect() -> Result::Code
    {
        for (auto &[profileName, ptr] : profilesList) {
            if (ptr != nullptr) {
                ptr->disconnect();
            }
        }
        return Result::Code::Success;
    }

    auto ProfileManager::start() -> Result::Code
    {
        musicProfilePtr->start();
        return Result::Code::Success;
    }

    auto ProfileManager::stop() -> Result::Code
    {
        musicProfilePtr->stop();
        return Result::Code::Success;
    }

    auto ProfileManager::incomingCallStarted() -> Result::Code
    {
        return callProfilePtr->incomingCallStarted();
    }

    auto ProfileManager::outgoingCallStarted(const utils::PhoneNumber &nr) -> Result::Code
    {
        if (callProfilePtr) {
            return callProfilePtr->outgoingCallStarted(nr.getView().getNonEmpty());
        }
        LOG_ERROR("No profile, returning!");
        return Result::Code::NotReady;
    }

    auto ProfileManager::incomingCallAnswered() -> Result::Code
    {
        return callProfilePtr->incomingCallAnswered();
    }

    auto ProfileManager::outgoingCallAnswered() -> Result::Code
    {
        return callProfilePtr->outgoingCallAnswered();
    }

    auto ProfileManager::callTerminated() -> Result::Code
    {
        return callProfilePtr->callTerminated();
    }

    auto ProfileManager::callMissed() -> Result::Code
    {
        return callProfilePtr->callMissed();
    }

    auto ProfileManager::setAudioDevice(std::shared_ptr<BluetoothAudioDevice> device) -> Result::Code
    {
        auto profileType = device->getProfileType();

        if (profilesList[profileType] == nullptr) {
            return Result::Code::NotReady;
        }

        profilesList[profileType]->setAudioDevice(device);
        LOG_ERROR("AudioDevice for profile: %s set!", magic_enum::enum_name(profileType).data());
        return Result::Code::Success;
    }

    auto ProfileManager::setIncomingCallNumber(const utils::PhoneNumber &nr) -> Result::Code
    {
        if (callProfilePtr) {
            return callProfilePtr->setIncomingCallNumber(nr.getView().getE164());
        }
        LOG_ERROR("No profile, returning!");
        return Result::Code::NotReady;
    }

    auto ProfileManager::setSignalStrengthData(const DataVariant &data) -> Result::Code
    {
        auto signalData = std::get<Store::SignalStrength>(data);
        if (callProfilePtr) {
            return callProfilePtr->setSignalStrength(static_cast<int>(signalData.rssiBar));
        }
        LOG_ERROR("No profile, returning!");
        return Result::Code::NotReady;
    }

    auto ProfileManager::setOperatorNameData(const DataVariant &data) -> Result::Code
    {
        auto operatorName = std::get<OperatorName>(data);
        if (callProfilePtr) {
            return callProfilePtr->setOperatorName(operatorName.getName());
        }
        LOG_ERROR("No profile, returning!");
        return Result::Code::NotReady;
    }

    auto ProfileManager::setBatteryLevelData(unsigned int level) -> Result::Code
    {
        auto batteryLevel = BatteryLevel(level);
        if (callProfilePtr) {
            return callProfilePtr->setBatteryLevel(batteryLevel);
        }
        LOG_ERROR("No profile, returning!");
        return Result::Code::NotReady;
    }

    auto ProfileManager::setNetworkStatusData(const DataVariant &data) -> Result::Code
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
        return Result::Code::NotReady;
    }

    void ProfileManager::deInit()
    {
        for (auto &[profileName, ptr] : profilesList) {
            if (ptr != nullptr) {
                ptr.reset();
            }
        }
        callProfilePtr  = nullptr;
        musicProfilePtr = nullptr;
        initialized     = false;

        LOG_DEBUG("ProfileManager deinit done");
    }
} // namespace bluetooth
