// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <sys/SystemManager.hpp>
#include <sys/messages/AlarmActivationStatusChangeRequest.hpp>

#include <system/messages/SystemManagerMessage.hpp>

#include <appmgr/messages/AlarmMessage.hpp>
#include <appmgr/messages/BatteryShutdown.hpp>
#include <service-appmgr/messages/SwitchRequest.hpp>
#include <service-appmgr/Constants.hpp>
#include <service-appmgr/Controller.hpp>
#include <service-evtmgr/Constants.hpp>

namespace sys
{
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
            bus.sendUnicast(std::make_shared<AlarmActivated>(), service::name::appmgr);
            break;
        case AlarmActivationStatus::DEACTIVATED:
            bus.sendUnicast(std::make_shared<AlarmDeactivated>(), service::name::appmgr);
            break;
        }
        return MessageNone{};
    }

    void SystemManager::batteryShutdownLevelAction()
    {
        auto msg = std::make_shared<BatteryShutdown>();
        bus.sendUnicast(msg, service::name::appmgr);
        SystemManagerCommon::batteryShutdownLevelAction();
    }

    void SystemManager::batteryNormalLevelAction()
    {
        SystemManagerCommon::batteryNormalLevelAction();
        auto battNormalMsg = std::make_shared<CriticalBatteryLevelNotification>(false);
        bus.sendUnicast(std::move(battNormalMsg), service::name::appmgr);
    }

    void SystemManager::batteryCriticalLevelAction(bool charging)
    {
        SystemManagerCommon::batteryCriticalLevelAction(charging);
        auto msg = std::make_shared<CriticalBatteryLevelNotification>(true, charging);
        bus.sendUnicast(std::move(msg), service::name::appmgr);
    }

    void SystemManager::Run()
    {
        initialize();

        // in shutdown we need to wait till event manager tells us that it's ok to stfu
        while (state == State::Running) {
            if (auto msg = mailbox.pop(); msg) {
                msg->Execute(this);
            }
        }

        while (state == State::Shutdown) {
            set(State::ShutdownReady);
        }

        DestroySystemService(service::name::evt_manager, this);
        CloseService();

        // it should be called before systemDeinit to make sure this log is dumped to the file
        LogPowerOffReason();

        if (systemDeinit) {
            systemDeinit();
        }

        EndScheduler();

        // Power off system
        PowerOff();
    }
} // namespace sys
