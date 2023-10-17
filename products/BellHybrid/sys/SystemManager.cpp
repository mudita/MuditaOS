// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <sys/SystemManager.hpp>
#include <sys/messages/AlarmActivationStatusChangeRequest.hpp>

#include <system/messages/SystemManagerMessage.hpp>

#include <appmgr/messages/AlarmMessage.hpp>
#include <service-appmgr/ServiceApplicationManagerName.hpp>

namespace sys
{
    namespace
    {
        namespace RegularClose
        {
            const std::vector<std::string> whitelist{};
        } // namespace RegularClose
    }     // namespace

    SystemManager::SystemManager(std::vector<std::unique_ptr<BaseServiceCreator>> &&creators)
        : SystemManagerCommon(std::move(creators))
    {}

    auto SystemManager::InitHandler() -> ReturnCodes
    {
        SystemManagerCommon::InitHandler();

        connect(typeid(AlarmActivationStatusChangeRequest), [this](sys::Message *message) -> sys::MessagePointer {
            auto request = static_cast<AlarmActivationStatusChangeRequest *>(message);
            return handleAlarmActivationStatusChangeRequest(request);
        });

        return ReturnCodes::Success;
    }

    auto SystemManager::handleAlarmActivationStatusChangeRequest(AlarmActivationStatusChangeRequest *request)
        -> MessagePointer
    {
        switch (request->getStatus()) {
        case AlarmActivationStatus::ACTIVATED:
            bus.sendMulticast(std::make_shared<AlarmActivated>(), sys::BusChannel::AlarmNotifications);
            break;
        case AlarmActivationStatus::DEACTIVATED:
            bus.sendMulticast(std::make_shared<AlarmDeactivated>(), sys::BusChannel::AlarmNotifications);
            break;
        }
        return MessageNone{};
    }

    void SystemManager::handleShutdown()
    {
        set(State::ShutdownReady);
    }

    const std::vector<std::string> &SystemManager::getWhiteListFor(WhiteListType /*type*/)
    {
        return RegularClose::whitelist;
    }
} // namespace sys
