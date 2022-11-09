// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "endpoints/developerMode/event/ATRequest.hpp"
#include "handler/RawATHandler.hpp"
#include "CellularUrcHandler.hpp"
#include "service-cellular/CellularMessage.hpp"
#include "service-cellular/CellularServiceAPI.hpp"
#include "service-cellular/Pdu.hpp"
#include "service-cellular/ServiceCellular.hpp"
#include "service-cellular/SignalStrength.hpp"
#include "service-cellular/State.hpp"
#include "service-cellular/USSD.hpp"
#include "service-cellular/MessageConstants.hpp"
#include "service-cellular/connection-manager/ConnectionManagerCellularCommands.hpp"
#include "SimCard.hpp"
#include "SMSParser.hpp"
#include "NetworkSettings.hpp"
#include "service-cellular/RequestFactory.hpp"
#include "service-cellular/CellularRequestHandler.hpp"
#include "system/messages/SentinelRegistrationMessage.hpp"

#include <Audio/AudioCommon.hpp>
#include <service-audio/AudioServiceAPI.hpp>
#include <BaseInterface.hpp>
#include <CalllogRecord.hpp>
#include <Commands.hpp>
#include <at/ATFactory.hpp>
#include <Common/Common.hpp>
#include <Common/Query.hpp>
#include <MessageType.hpp>
#include <modem/ATCommon.hpp>
#include <modem/ATParser.hpp>
#include <modem/mux/DLCChannel.h>
#include <modem/mux/CellularMux.h>
#include <NotificationsRecord.hpp>
#include <PhoneNumber.hpp>
#include <Result.hpp>
#include <Service/Message.hpp>
#include <Service/Service.hpp>
#include <Timers/TimerFactory.hpp>
#include <Tables/CalllogTable.hpp>
#include <Tables/Record.hpp>
#include <Utils.hpp>
#include <Utility.hpp>
#include <at/cmd/CLCC.hpp>
#include <at/cmd/CFUN.hpp>
#include <at/UrcCmti.hpp>
#include <at/UrcCreg.hpp>
#include <at/UrcCtze.hpp>
#include <at/UrcCusd.hpp>
#include <at/UrcQind.hpp>
#include <at/UrcCpin.hpp> // for Cpin
#include <at/response.hpp>
#include <bsp/cellular/bsp_cellular.hpp>
#include <EventStore.hpp>
#include <country.hpp>
#include <log/log.hpp>
#include <at/UrcFactory.hpp>
#include <queries/messages/sms/QuerySMSSearchByType.hpp>
#include <queries/notifications/QueryNotificationsIncrement.hpp>
#include <queries/notifications/QueryNotificationsMultipleIncrement.hpp>
#include <projdefs.h>
#include <service-antenna/AntennaMessage.hpp>
#include <service-antenna/AntennaServiceAPI.hpp>
#include <service-antenna/ServiceAntenna.hpp>
#include <service-appmgr/Constants.hpp>
#include <service-appmgr/Controller.hpp>
#include <service-db/agents/settings/SystemSettings.hpp>
#include <service-db/DBServiceAPI.hpp>
#include <service-db/DBNotificationMessage.hpp>
#include <service-db/QueryMessage.hpp>
#include <service-evtmgr/Constants.hpp>
#include <service-evtmgr/EventManagerServiceAPI.hpp>
#include <service-evtmgr/EVMessages.hpp>
#include <service-desktop/DesktopMessages.hpp>
#include <service-desktop/DeveloperModeMessage.hpp>
#include <service-time/service-time/TimeMessage.hpp>
#include <task.h>
#include <ucs2/UCS2.hpp>
#include <utf8/UTF8.hpp>

#include <queries/messages/sms/QuerySMSUpdate.hpp>
#include <queries/messages/sms/QuerySMSAdd.hpp>

#include <algorithm>
#include <bits/exception.h>
#include <cassert>
#include <iostream>
#include <map>
#include <optional>
#include <string>
#include <utility>
#include <vector>
#include "checkSmsCenter.hpp"
#include <service-desktop/Constants.hpp>
#include <gsl/util>
#include <ticks.hpp>

#include "ServiceCellularPriv.hpp"
#include <service-cellular/api/request/sim.hpp>
#include <service-cellular/api/notification/notification.hpp>

#include <ctime>

#include <at/cmd/QCFGUsbnet.hpp>

const char *ServiceCellular::serviceName = cellular::service::name;

using namespace cellular;
using namespace cellular::msg;
using cellular::service::State;

namespace constants
{
    using namespace std::chrono_literals;

    inline constexpr auto cellularStack = 8000;

    inline constexpr std::chrono::milliseconds sleepTimerInterval{500ms};
    inline constexpr std::chrono::milliseconds maxUrcHandleTime{5s};
    inline constexpr std::chrono::milliseconds maxTimeWithoutCommunication{1s};
} // namespace constants

ServiceCellular::ServiceCellular()
    : sys::Service(serviceName, "", constants::cellularStack, sys::ServicePriority::Idle),
      phoneModeObserver{std::make_unique<sys::phone_modes::Observer>()},
      priv{std::make_unique<internal::ServiceCellularPriv>(this)}
{
    LOG_INFO("[ServiceCellular] Initializing");

    bus.channels.push_back(sys::BusChannel::ServiceCellularNotifications);
    bus.channels.push_back(sys::BusChannel::ServiceDBNotifications);
    bus.channels.push_back(sys::BusChannel::ServiceEvtmgrNotifications);
    bus.channels.push_back(sys::BusChannel::PhoneModeChanges);

    callStateTimer = sys::TimerFactory::createPeriodicTimer(
        this, "call_state", std::chrono::milliseconds{1000}, [this](sys::Timer &) { CallStateTimerHandler(); });
    callEndedRecentlyTimer = sys::TimerFactory::createSingleShotTimer(
        this, "callEndedRecentlyTimer", std::chrono::seconds{5}, [this](sys::Timer &timer) {
            priv->outSMSHandler.sendMessageIfDelayed();
        });
    stateTimer = sys::TimerFactory::createPeriodicTimer(
        this, "state", std::chrono::milliseconds{1000}, [&](sys::Timer &) { handleStateTimer(); });
    ussdTimer = sys::TimerFactory::createPeriodicTimer(
        this, "ussd", std::chrono::milliseconds{1000}, [this](sys::Timer &) { handleUSSDTimer(); });
    sleepTimer = sys::TimerFactory::createSingleShotTimer(
        this, "sleep", constants::sleepTimerInterval, [this](sys::Timer &) { SleepTimerHandler(); });
    connectionTimer =
        sys::TimerFactory::createPeriodicTimer(this, "connection", std::chrono::seconds{60}, [this](sys::Timer &) {
            utility::conditionally_invoke(
                [this]() { return phoneModeObserver->isInMode(sys::phone_modes::PhoneMode::Offline); },
                [this]() {
                    if (connectionManager != nullptr)
                        connectionManager->onTimerTick();
                });
        });
    simTimer = sys::TimerFactory::createSingleShotTimer(
        this, "simTimer", std::chrono::milliseconds{6000}, [this](sys::Timer &) { priv->simCard->handleSimTimer(); });

    csqTimer = sys::TimerFactory::createPeriodicTimer(this, "csqTimer", std::chrono::minutes{15}, [this](sys::Timer &) {
        priv->csqHandler->handleTimerTick();
        csqCounter.clearCounter();
    });

    notificationCallback = [this](std::string &data) {
        LOG_DEBUG("Notifications callback called with %u data bytes", static_cast<unsigned int>(data.size()));

        std::string logStr = utils::removeNewLines(data);
        LOG_SENSITIVE(LOGDEBUG, "Data: %s", logStr.c_str());
        atURCStream.write(data);
        auto vUrc = atURCStream.getURCList();

        for (const auto &urc : vUrc) {
            std::string message;
            auto msg = identifyNotification(urc);

            if (msg != std::nullopt) {
                bus.sendMulticast(msg.value(), sys::BusChannel::ServiceCellularNotifications);
            }
        }
    };

    packetData = std::make_unique<packet_data::PacketData>(*this); /// call in apnListChanged handler

    registerMessageHandlers();
}

ServiceCellular::~ServiceCellular()
{
    LOG_INFO("[ServiceCellular] Cleaning resources");
}

void ServiceCellular::SleepTimerHandler()
{
    auto currentTime                = cpp_freertos::Ticks::TicksToMs(cpp_freertos::Ticks::GetTicks());
    auto lastCommunicationTimestamp = cmux->getLastCommunicationTimestamp();
    auto timeOfInactivity           = currentTime >= lastCommunicationTimestamp
                                          ? currentTime - lastCommunicationTimestamp
                                          : std::numeric_limits<TickType_t>::max() - lastCommunicationTimestamp + currentTime;

    if (!ongoingCall->active() && priv->state->get() >= State::ST::URCReady &&
        timeOfInactivity >= constants::maxTimeWithoutCommunication.count()) {
        cmux->enterSleepMode();
        cpuSentinel->ReleaseMinimumFrequency();
    }
    else {
        sleepTimer.restart(constants::sleepTimerInterval);
    }
}

void ServiceCellular::WakeUpHandler()
{
    sleepTimer.stop();
    cpuSentinel->HoldMinimumFrequency(bsp::CpuFrequencyMHz::Level_2);
    cmux->exitSleepMode();
}

void ServiceCellular::CallStateTimerHandler()
{
    LOG_DEBUG("CallStateTimerHandler");
    auto msg = std::make_shared<cellular::ListCallsMessage>();
    bus.sendUnicast(std::move(msg), ServiceCellular::serviceName);
}

sys::ReturnCodes ServiceCellular::InitHandler()
{
    board = cmux->getBoard();

    settings = std::make_unique<settings::Settings>();
    settings->init(::service::ServiceProxy(shared_from_this()));

    connectionManager = std::make_unique<ConnectionManager>(
        utils::getNumericValue<bool>(
            settings->getValue(settings::Cellular::offlineMode, settings::SettingsScope::Global)),
        static_cast<std::chrono::minutes>(utils::getNumericValue<int>(settings->getValue(
            settings->getValue(settings::Offline::connectionFrequency, settings::SettingsScope::Global)))),
        std::make_shared<ConnectionManagerCellularCommands>(*this));

    priv->state->set(State::ST::WaitForStartPermission);
    settings->registerValueChange(
        settings::Cellular::apn_list,
        [this](const std::string &value) { apnListChanged(value); },
        ::settings::SettingsScope::Global);

    priv->setInitialMultiPartSMSUID(static_cast<std::uint8_t>(utils::getNumericValue<int>(
        settings->getValue(settings::Cellular::currentUID, settings::SettingsScope::Global))));

    priv->saveNewMultiPartSMSUIDCallback = [this](std::uint8_t uid) -> void {
        settings->setValue(
            settings::Cellular::currentUID, std::to_string(static_cast<int>(uid)), settings::SettingsScope::Global);
    };

    auto rawVolteSetting = settings->getValue(settings::Cellular::volteEnabled, settings::SettingsScope::Global);
    if (rawVolteSetting.empty()) {
        settings->setValue(settings::Cellular::volteEnabled, "0", settings::SettingsScope::Global);
    }

    cpuSentinel = std::make_shared<sys::CpuSentinel>(serviceName, this);

    ongoingCall =
        std::make_unique<call::Call>(this,
                                     cmux.get(),
                                     sys::TimerFactory::createPeriodicTimer(
                                         this,
                                         "callDurationTimer",
                                         std::chrono::milliseconds{1000},
                                         [this](sys::Timer &) { ongoingCall->handle(call::event::OngoingTimer{}); }),
                                     sys::TimerFactory::createSingleShotTimer(
                                         this,
                                         "callRingTimeout",
                                         std::chrono::milliseconds{1000},
                                         [this](sys::Timer &) { ongoingCall->handle(call::event::RingTimeout{}); }),
                                     cpuSentinel);

    auto sentinelRegistrationMsg = std::make_shared<sys::SentinelRegistrationMessage>(cpuSentinel);
    bus.sendUnicast(sentinelRegistrationMsg, ::service::name::system_manager);

    cmux->registerCellularDevice();

    return sys::ReturnCodes::Success;
}

sys::ReturnCodes ServiceCellular::DeinitHandler()
{
    settings->deinit();
    return sys::ReturnCodes::Success;
}

void ServiceCellular::ProcessCloseReason(sys::CloseReason closeReason)
{}

sys::ReturnCodes ServiceCellular::SwitchPowerModeHandler(const sys::ServicePowerMode mode)
{
    LOG_INFO("[ServiceCellular] PowerModeHandler: %s", c_str(mode));

    switch (mode) {
    case sys::ServicePowerMode::Active:
        cmux->exitSleepMode();
        break;
    case sys::ServicePowerMode::SuspendToRAM:
    case sys::ServicePowerMode::SuspendToNVM:
        cmux->enterSleepMode();
        break;
    }

    return sys::ReturnCodes::Success;
}

void ServiceCellular::registerMessageHandlers()
{
    phoneModeObserver->connect(this);
    phoneModeObserver->subscribe([this](sys::phone_modes::PhoneMode mode) {
        if (priv->simCard->isSimCardSelected()) {
            connectionManager->onPhoneModeChange(mode);
        }
        if (ongoingCall != nullptr) {
            ongoingCall->handle(call::event::ModeChange{mode});
        }
    });
    phoneModeObserver->subscribe([&](sys::phone_modes::Tethering tethering) {
        if (tethering == sys::phone_modes::Tethering::On) {
            Store::GSM::get()->setTethering(Store::Tethering::On);
            priv->tetheringHandler->enable();
        }
        else {
            Store::GSM::get()->setTethering(Store::Tethering::Off);
            priv->tetheringHandler->disable();
        }
        if (ongoingCall != nullptr) {
            ongoingCall->handle(call::event::TetheringChange{tethering});
        }
    });

    priv->connectSimCard();
    priv->connectNetworkTime();
    priv->connectSimContacts();
    priv->connectImeiGetHandler();
    priv->connectCSQHandler();

    connect(typeid(cellular::StartOperatorsScanMessage), [&](sys::Message *request) -> sys::MessagePointer {
        auto msg = static_cast<cellular::StartOperatorsScanMessage *>(request);
        return handleCellularStartOperatorsScan(msg);
    });

    connect(typeid(cellular::GetActiveContextsMessage), [&](sys::Message *request) -> sys::MessagePointer {
        auto msg = static_cast<cellular::GetActiveContextsMessage *>(request);
        return handleCellularGetActiveContextsMessage(msg);
    });

    connect(typeid(cellular::RequestCurrentOperatorNameMessage), [&](sys::Message *request) -> sys::MessagePointer {
        auto msg = static_cast<cellular::RequestCurrentOperatorNameMessage *>(request);
        handleCellularRequestCurrentOperatorName(msg);
        return sys::MessageNone{};
    });

    connect(typeid(cellular::GetAPNMessage), [&](sys::Message *request) -> sys::MessagePointer {
        auto msg = static_cast<cellular::GetAPNMessage *>(request);
        return handleCellularGetAPNMessage(msg);
    });

    connect(typeid(cellular::SetAPNMessage), [&](sys::Message *request) -> sys::MessagePointer {
        auto msg = static_cast<cellular::SetAPNMessage *>(request);
        return handleCellularSetAPNMessage(msg);
    });

    connect(typeid(cellular::NewAPNMessage), [&](sys::Message *request) -> sys::MessagePointer {
        auto msg = static_cast<cellular::NewAPNMessage *>(request);
        return handleCellularNewAPNMessage(msg);
    });

    connect(typeid(cellular::SetDataTransferMessage), [&](sys::Message *request) -> sys::MessagePointer {
        auto msg = static_cast<cellular::SetDataTransferMessage *>(request);
        return handleCellularSetDataTransferMessage(msg);
    });

    connect(typeid(cellular::GetDataTransferMessage), [&](sys::Message *request) -> sys::MessagePointer {
        auto msg = static_cast<cellular::GetDataTransferMessage *>(request);
        return handleCellularGetDataTransferMessage(msg);
    });

    connect(typeid(cellular::ActivateContextMessage), [&](sys::Message *request) -> sys::MessagePointer {
        auto msg = static_cast<cellular::ActivateContextMessage *>(request);
        return handleCellularActivateContextMessage(msg);
    });

    connect(typeid(cellular::DeactivateContextMessage), [&](sys::Message *request) -> sys::MessagePointer {
        auto msg = static_cast<cellular::DeactivateContextMessage *>(request);
        return handleCellularDeactivateContextMessage(msg);
    });

    connect(typeid(cellular::SetFlightModeMessage), [&](sys::Message *request) -> sys::MessagePointer {
        auto msg = static_cast<cellular::SetFlightModeMessage *>(request);
        return handleCellularSetFlightModeMessage(msg);
    });

    connect(typeid(cellular::PowerStateChange), [&](sys::Message *request) -> sys::MessagePointer {
        auto msg             = static_cast<cellular::PowerStateChange *>(request);
        priv->nextPowerState = msg->getNewState();
        handle_power_state_change();
        return sys::MessageNone{};
    });

    connect(typeid(sdesktop::developerMode::DeveloperModeRequest), [&](sys::Message *request) -> sys::MessagePointer {
        auto msg = static_cast<sdesktop::developerMode::DeveloperModeRequest *>(request);
        if (typeid(*msg->event.get()) == typeid(sdesktop::developerMode::CellularHotStartEvent)) {
            priv->simCard->setChannel(nullptr);
            priv->networkTime->setChannel(nullptr);
            priv->simContacts->setChannel(nullptr);
            priv->imeiGetHandler->setChannel(nullptr);

            cmux->closeChannels();
            ///> change state - simulate hot start
            handle_power_up_request();
        }
        if (typeid(*msg->event.get()) == typeid(sdesktop::developerMode::CellularStateInfoRequestEvent)) {
            auto event = std::make_unique<sdesktop::developerMode::CellularStateInfoRequestEvent>(priv->state->c_str());
            auto message = std::make_shared<sdesktop::developerMode::DeveloperModeRequest>(std::move(event));
            bus.sendUnicast(std::move(message), ::service::name::service_desktop);
        }
        if (typeid(*msg->event.get()) == typeid(sdesktop::developerMode::CellularSleepModeInfoRequestEvent)) {
            auto event = std::make_unique<sdesktop::developerMode::CellularSleepModeInfoRequestEvent>(
                cmux->isCellularInSleepMode());
            auto message = std::make_shared<sdesktop::developerMode::DeveloperModeRequest>(std::move(event));
            bus.sendUnicast(std::move(message), ::service::name::service_desktop);
        }
        if (typeid(*msg->event.get()) == typeid(sdesktop::developerMode::ATResponseEvent)) {
            auto channel = cmux->get(CellularMux::Channel::Commands);
            assert(channel);
            auto handler = cellular::RawATHandler(*channel);
            return handler.handle(msg);
        }
        return sys::MessageNone{};
    });

    connect(typeid(cellular::NewIncomingSMSMessage), [&](sys::Message *request) -> sys::MessagePointer {
        auto msg = static_cast<cellular::NewIncomingSMSMessage *>(request);
        auto ret = receiveSMS(msg->getData());
        return std::make_shared<cellular::ResponseMessage>(ret);
    });

    connect(typeid(cellular::AnswerIncomingCallMessage), [&](sys::Message *request) -> sys::MessagePointer {
        auto msg = static_cast<cellular::AnswerIncomingCallMessage *>(request);
        return handleCellularAnswerIncomingCallMessage(msg);
    });

    connect(typeid(cellular::CallRequestMessage), [&](sys::Message *request) -> sys::MessagePointer {
        if (phoneModeObserver->isInMode(sys::phone_modes::PhoneMode::Offline)) {
            this->bus.sendUnicast(std::make_shared<cellular::CallRejectedByOfflineNotification>(),
                                  ::service::name::appmgr);
            return std::make_shared<cellular::ResponseMessage>(true);
        }

        auto msg = static_cast<cellular::CallRequestMessage *>(request);
        return handleCellularCallRequestMessage(msg);
    });

    connect(typeid(cellular::HangupCallMessage), [&](sys::Message *request) -> sys::MessagePointer {
        auto msg = static_cast<cellular::HangupCallMessage *>(request);
        handleCellularHangupCallMessage(msg);
        return sys::MessageNone{};
    });

    connect(typeid(cellular::DismissCallMessage), [&](sys::Message *request) -> sys::MessagePointer {
        handleCellularDismissCallMessage(request);
        return sys::MessageNone{};
    });

    connect(typeid(db::QueryResponse), [&](sys::Message *request) -> sys::MessagePointer {
        auto msg = static_cast<db::QueryResponse *>(request);
        return handleDBQueryResponseMessage(msg);
    });

    connect(typeid(cellular::ListCallsMessage), [&](sys::Message *request) -> sys::MessagePointer {
        auto msg = static_cast<cellular::ListCallsMessage *>(request);
        return handleCellularListCallsMessage(msg);
    });

    connect(typeid(db::NotificationMessage), [&](sys::Message *request) -> sys::MessagePointer {
        auto msg = static_cast<db::NotificationMessage *>(request);
        return handleDBNotificationMessage(msg);
    });

    connect(typeid(cellular::RingingMessage), [&](sys::Message *request) -> sys::MessagePointer {
        auto msg = static_cast<cellular::RingingMessage *>(request);
        return handleCellularRingingMessage(msg);
    });

    connect(typeid(cellular::GetIMSIMessage),
            [&](sys::Message *request) -> sys::MessagePointer { return handleCellularGetIMSIMessage(request); });

    connect(typeid(cellular::GetOwnNumberMessage),
            [&](sys::Message *request) -> sys::MessagePointer { return handleCellularGetOwnNumberMessage(request); });

    connect(typeid(cellular::GetNetworkInfoMessage),
            [&](sys::Message *request) -> sys::MessagePointer { return handleCellularGetNetworkInfoMessage(request); });

    connect(typeid(cellular::AntennaRequestMessage),
            [&](sys::Message *request) -> sys::MessagePointer { return handleCellularSelectAntennaMessage(request); });

    connect(typeid(cellular::SetScanModeMessage),
            [&](sys::Message *request) -> sys::MessagePointer { return handleCellularSetScanModeMessage(request); });

    connect(typeid(cellular::GetScanModeMessage),
            [&](sys::Message *request) -> sys::MessagePointer { return handleCellularGetScanModeMessage(request); });

    connect(typeid(cellular::GetFirmwareVersionMessage), [&](sys::Message *request) -> sys::MessagePointer {
        return handleCellularGetFirmwareVersionMessage(request);
    });

    connect(typeid(sevm::StatusStateMessage),
            [&](sys::Message *request) -> sys::MessagePointer { return handleEVMStatusMessage(request); });

    connect(typeid(cellular::GetCsqMessage),
            [&](sys::Message *request) -> sys::MessagePointer { return handleCellularGetCsqMessage(request); });

    connect(typeid(cellular::GetCregMessage),
            [&](sys::Message *request) -> sys::MessagePointer { return handleCellularGetCregMessage(request); });

    connect(typeid(cellular::GetNwinfoMessage),
            [&](sys::Message *request) -> sys::MessagePointer { return handleCellularGetNwinfoMessage(request); });

    connect(typeid(cellular::GetAntennaMessage),
            [&](sys::Message *request) -> sys::MessagePointer { return handleCellularGetAntennaMessage(request); });

    connect(typeid(cellular::DtmfRequestMessage),
            [&](sys::Message *request) -> sys::MessagePointer { return handleCellularDtmfRequestMessage(request); });

    connect(typeid(cellular::USSDMessage),
            [&](sys::Message *request) -> sys::MessagePointer { return handleCellularUSSDMessage(request); });

    connect(typeid(cellular::StateChange),
            [&](sys::Message *request) -> sys::MessagePointer { return handleStateRequestMessage(request); });

    connect(typeid(cellular::CallActiveNotification),
            [&](sys::Message *request) -> sys::MessagePointer { return handleCallActiveNotification(request); });

    connect(typeid(cellular::PowerUpProcedureCompleteNotification), [&](sys::Message *request) -> sys::MessagePointer {
        return handlePowerUpProcedureCompleteNotification(request);
    });

    connect(typeid(cellular::PowerDownDeregisteringNotification), [&](sys::Message *request) -> sys::MessagePointer {
        return handlePowerDownDeregisteringNotification(request);
    });

    connect(typeid(cellular::PowerDownDeregisteredNotification), [&](sys::Message *request) -> sys::MessagePointer {
        return handlePowerDownDeregisteredNotification(request);
    });

    connect(typeid(cellular::NewIncomingSMSNotification),
            [&](sys::Message *request) -> sys::MessagePointer { return handleNewIncomingSMSNotification(request); });

    connect(typeid(cellular::SmsDoneNotification),
            [&](sys::Message *request) -> sys::MessagePointer { return handleSmsDoneNotification(request); });

    connect(typeid(cellular::SignalStrengthUpdateNotification), [&](sys::Message *request) -> sys::MessagePointer {
        csqCounter.count();
        auto message = std::make_shared<cellular::URCCounterMessage>(csqCounter.getCounter());
        bus.sendUnicast(std::move(message), serviceName);
        return handleSignalStrengthUpdateNotification(request);
    });

    connect(typeid(cellular::NetworkStatusUpdateNotification), [&](sys::Message *request) -> sys::MessagePointer {
        return handleNetworkStatusUpdateNotification(request);
    });

    connect(typeid(cellular::UrcIncomingNotification),
            [&](sys::Message *request) -> sys::MessagePointer { return handleUrcIncomingNotification(request); });

    connect(typeid(cellular::RingNotification),
            [&](sys::Message *request) -> sys::MessagePointer { return handleCellularRingNotification(request); });

    connect(typeid(cellular::CallerIdNotification),
            [&](sys::Message *request) -> sys::MessagePointer { return handleCellularCallerIdNotification(request); });

    connect(typeid(cellular::SetConnectionFrequencyMessage), [&](sys::Message *request) -> sys::MessagePointer {
        return handleCellularSetConnectionFrequencyMessage(request);
    });

    connect(typeid(RetryPhoneModeChangeRequest), [&](sys::Message *request) -> sys::MessagePointer {
        connectionManager->onPhoneModeChange(phoneModeObserver->getCurrentPhoneMode());
        return sys::MessageNone{};
    });

    connect(typeid(cellular::CallAudioEventRequest), [&](sys::Message *request) -> sys::MessagePointer {
        auto message = static_cast<cellular::CallAudioEventRequest *>(request);
        ongoingCall->handle(call::event::AudioRequest{message->eventType});
        return sys::MessageNone{};
    });

    /// aborted notification can come from:
    /// 1. URC
    /// 2. from here from handle cellular hangup
    connect(typeid(cellular::CallAbortedNotification), [&](sys::Message * /*request*/) -> sys::MessagePointer {
        ongoingCall->handle(call::event::Ended{});
        return sys::MessageNone{};
    });

    connect(typeid(cellular::IsCallActive), [&](sys::Message * /*request*/) -> sys::MessagePointer {
        return std::make_shared<cellular::IsCallActiveResponse>(ongoingCall && ongoingCall->active());
    });

    connect(typeid(cellular::GetVolteStateRequest), [&](sys::Message *request) -> sys::MessagePointer {
        return std::make_shared<cellular::GetVolteStateResponse>(priv->volteHandler->getVolteState());
    });

    connect(typeid(cellular::SwitchVolteOnOffRequest), [&](sys::Message *request) -> sys::MessagePointer {
        auto message = static_cast<cellular::SwitchVolteOnOffRequest *>(request);
        auto channel = cmux->get(CellularMux::Channel::Commands);
        settings->setValue(
            settings::Cellular::volteEnabled, message->enable ? "1" : "0", settings::SettingsScope::Global);
        if (not priv->volteHandler->switchVolte(*channel, message->enable)) {
            auto notification = std::make_shared<cellular::VolteStateNotification>(priv->volteHandler->getVolteState());
            bus.sendMulticast(std::move(notification), sys::BusChannel::ServiceCellularNotifications);
            priv->modemResetHandler->performHardReset();
        }
        return sys::MessageNone{};
    });

    handle_CellularGetChannelMessage();
}

void ServiceCellular::change_state(cellular::StateChange *msg)
{
    assert(msg);
    switch (msg->request) {
    case State::ST::Idle:
        handle_idle();
        break;
    case State::ST::WaitForStartPermission:
        handle_wait_for_start_permission();
        break;
    case State::ST::PowerUpRequest:
        handle_power_up_request();
        break;
    case State::ST::StatusCheck:
        handle_status_check();
        break;
    case State::ST::PowerUpInProgress:
        handle_power_up_in_progress_procedure();
        break;
    case State::ST::PowerUpProcedure:
        handle_power_up_procedure();
        break;
    case State::ST::BaudDetect:
        if (nextPowerStateChangeAwaiting) {
            handle_power_state_change();
        }
        else {
            handle_baud_detect();
        }
        break;
    case State::ST::AudioConfigurationProcedure:
        handle_audio_conf_procedure();
        break;
    case State::ST::CellularPrivInit:
        handle_cellular_priv_init();
        break;
    case State::ST::CellularConfProcedure:
        handle_start_conf_procedure();
        break;
    case State::ST::APNConfProcedure:
        handle_apn_conf_procedure();
        break;
    case State::ST::SanityCheck:
        handle_sim_sanity_check();
        break;
    case State::ST::ModemOn:
        handle_modem_on();
        break;
    case State::ST::URCReady:
        handle_URCReady();
        break;
    case State::ST::ModemFatalFailure:
        handle_fatal_failure();
        break;
    case State::ST::Failed:
        handle_failure();
        break;
    case State::ST::Ready:
        handle_ready();
        break;
    case State::ST::PowerDownStarted:
        handle_power_down_started();
        break;
    case State::ST::PowerDownWaiting:
        handle_power_down_waiting();
        break;
    case State::ST::PowerDown:
        handle_power_down();
        if (nextPowerStateChangeAwaiting) {
            handle_power_state_change();
        }
        break;
    };
}

bool ServiceCellular::handle_idle()
{
    LOG_DEBUG("Idle");
    return true;
}

bool ServiceCellular::handle_wait_for_start_permission()
{
    auto msg = std::make_shared<cellular::CheckIfStartAllowedMessage>();
    bus.sendUnicast(msg, ::service::name::system_manager);

    return true;
}

bool ServiceCellular::handle_power_up_request()
{
    cmux->selectAntenna(bsp::cellular::antenna::lowBand);
    switch (board) {
    case bsp::Board::RT1051:
        priv->state->set(State::ST::StatusCheck);
        break;
    case bsp::Board::Linux:
        priv->state->set(State::ST::PowerUpProcedure);
        break;
    case bsp::Board::none:
        return false;
        break;
    }
    return true;
}

bool ServiceCellular::handle_power_up_procedure()
{
    switch (board) {
    case bsp::Board::RT1051: {
        LOG_DEBUG("RT1051 - cold start");
        cmux->turnOnModem();
        // wait for status pin change to change state
        break;
    }
    case bsp::Board::Linux: {
        // check baud once to determine if it's already turned on
        auto ret = cmux->baudDetectOnce();
        if (ret == CellularMux::ConfState::Success) {
            // it's on aka hot start.
            LOG_DEBUG("Linux - hot start");
            priv->state->set(State::ST::CellularConfProcedure);
            break;
        }
        else {
            // it's off aka cold start
            LOG_DEBUG("Linux - cold start");
            LOG_WARN("Press PWR_KEY for 2 sec on modem eval board!");
            vTaskDelay(pdMS_TO_TICKS(2000)); // give some 2 secs more for user input
            // if it's Linux, then wait for status pin to become active, to align its starting position with RT1051
            vTaskDelay(pdMS_TO_TICKS(8000));
            priv->state->set(State::ST::PowerUpInProgress);
            break;
        }
    }
    case bsp::Board::none:
    default:
        LOG_FATAL("Board not known!");
        assert(0);
        break;
    }
    return true;
}

bool ServiceCellular::handle_power_up_in_progress_procedure(void)
{
    if (priv->modemResetHandler->isResetInProgress()) {
        constexpr auto msModemUartInitTime = 12000;
        vTaskDelay(pdMS_TO_TICKS(msModemUartInitTime));
    }

    priv->state->set(State::ST::BaudDetect);
    return true;
}

bool ServiceCellular::handle_baud_detect()
{
    auto ret = cmux->baudDetectProcedure();
    if (ret == CellularMux::ConfState::Success) {
        priv->state->set(State::ST::CellularConfProcedure);
        return true;
    }
    else {
        priv->state->set(State::ST::ModemFatalFailure);
        return false;
    }
}

bool ServiceCellular::handle_power_down_started()
{
    /// we should not send anything to the modem from now on
    return true;
}

bool ServiceCellular::handle_power_down_waiting()
{
    csqTimer.stop();
    switch (board) {
    case bsp::Board::RT1051:
        // wait for pin status become inactive (handled elsewhere)
        break;
    case bsp::Board::Linux:
        // if it's Linux, then wait for status pin to become inactive, to align with RT1051
        vTaskDelay(pdMS_TO_TICKS(17000)); // according to docs this shouldn't be needed, but better be safe than Quectel
        priv->state->set(State::ST::PowerDown);
        break;
    default:
        LOG_ERROR("Powering 'down an unknown device not handled");
        return false;
    }
    return true;
}

bool ServiceCellular::handle_power_down()
{
    LOG_DEBUG("Powered Down");
    cmux->closeChannels();
    cmux.reset();
    cmux = std::make_unique<CellularMux>(PortSpeed_e::PS460800, this);
    if (priv->modemResetHandler->isResetInProgress()) {
        priv->state->set(State::ST::Idle);
    }
    return true;
}

bool ServiceCellular::handle_start_conf_procedure()
{
    // Start configuration procedure, if it's first run modem will be restarted
    auto confRet = cmux->confProcedure();
    if (confRet == CellularMux::ConfState::Success) {
        priv->state->set(State::ST::AudioConfigurationProcedure);
        return true;
    }
    priv->state->set(State::ST::Failed);
    return false;
}

bool ServiceCellular::handle_audio_conf_procedure()
{
    auto audioRet = cmux->audioConfProcedure();
    if (audioRet == CellularMux::ConfState::ModemNeedsReset) {
        priv->modemResetHandler->performReboot();
        return false;
    }

    if (audioRet == CellularMux::ConfState::Success) {
        auto cmd = at::factory(at::AT::IPR) + std::to_string(ATPortSpeeds_text[cmux->getStartParams().PortSpeed]);
        LOG_DEBUG("Setting baudrate %i baud", ATPortSpeeds_text[cmux->getStartParams().PortSpeed]);
        if (!cmux->getParser()->cmd(cmd)) {
            LOG_ERROR("Baudrate setup error");
            priv->state->set(State::ST::Failed);
            return false;
        }
        cmux->getCellular()->setSpeed(ATPortSpeeds_text[cmux->getStartParams().PortSpeed]);
        vTaskDelay(1000);

        if (cmux->startMultiplexer() == CellularMux::ConfState::Success) {

            LOG_DEBUG("[ServiceCellular] Modem is fully operational");
            // open channel - notifications
            DLCChannel *notificationsChannel = cmux->get(CellularMux::Channel::Notifications);
            if (notificationsChannel != nullptr) {
                LOG_DEBUG("Setting up notifications callback");
                notificationsChannel->setCallback(notificationCallback);
            }

            priv->state->set(State::ST::CellularPrivInit);
            return true;
        }
        else {
            priv->state->set(State::ST::Failed);
            return false;
        }
    }
    else if (audioRet == CellularMux::ConfState::Failure) {
        /// restart
        priv->state->set(State::ST::AudioConfigurationProcedure);
        return true;
    }
    // Reset procedure started, do nothing here
    priv->state->set(State::ST::Idle);
    return true;
}

bool ServiceCellular::handle_cellular_priv_init()
{
    auto channel = cmux->get(CellularMux::Channel::Commands);
    priv->simCard->setChannel(channel);
    priv->networkTime->setChannel(channel);
    priv->simContacts->setChannel(channel);
    priv->imeiGetHandler->setChannel(channel);

    bool needReset = false;

    auto enableVolte =
        settings->getValue(settings::Cellular::volteEnabled, settings::SettingsScope::Global) == "1" ? true : false;
    ;
    try {
        needReset = !priv->tetheringHandler->configure() || !priv->volteHandler->switchVolte(*channel, enableVolte);
    }
    catch (std::runtime_error const &exc) {
        LOG_ERROR("%s", exc.what());
    }
    if (needReset) {
        priv->modemResetHandler->performHardReset();
        return true;
    }

    auto flightMode =
        settings->getValue(settings::Cellular::offlineMode, settings::SettingsScope::Global) == "1" ? true : false;
    connectionManager->setFlightMode(flightMode);
    auto interval = 0;
    if (utils::toNumeric(settings->getValue(settings::Offline::connectionFrequency, settings::SettingsScope::Global),
                         interval)) {
        connectionManager->setInterval(std::chrono::minutes{interval});
    }

    priv->initCSQHandler();
    priv->state->set(State::ST::APNConfProcedure);
    return true;
}
auto ServiceCellular::handle(db::query::SMSSearchByTypeResult *response) -> bool
{
    if (response->getResults().empty()) {
        priv->outSMSHandler.handleNoMoreDbRecords();
    }
    else {
        for (auto &rec : response->getResults()) {
            if (rec.type == SMSType::QUEUED) {
                priv->outSMSHandler.handleIncomingDbRecord(rec, callEndedRecentlyTimer.isActive());
            }
        }
    }
    return true;
}

/**
 * NOTICE: URC handling function identifyNotification works on different thread, so sending
 * any AT commands is not allowed here (also in URC handlers and other functions called from here)
 * @return
 */
std::optional<std::shared_ptr<sys::Message>> ServiceCellular::identifyNotification(const std::string &data)
{

    CellularUrcHandler urcHandler(*this);

    std::string str(data.begin(), data.end());

    std::string logStr = utils::removeNewLines(str);
    LOG_SENSITIVE(LOGDEBUG, "Notification:: %s", logStr.c_str());

    auto urc = at::urc::UrcFactory::Create(str);
    urc->Handle(urcHandler);

    if (!urc->isHandled()) {
        LOG_SENSITIVE(LOGWARN, "Unhandled notification: %s", logStr.c_str());
    }

    return urcHandler.getResponse();
}

std::string numberFromAddress(std::string const &address)
{
    return utils::endsWith(address, "/TYPE=PLMN") ? address.substr(0, address.size() - 10) : std::string();
}

auto ServiceCellular::receiveSMS(std::string messageNumber) -> bool
{
    auto retVal = true;

    auto channel = cmux->get(CellularMux::Channel::Commands);
    if (channel == nullptr) {
        retVal = false;
        return retVal;
    }

    auto ucscSetRetries = 0;
    while (ucscSetRetries < at::AtCmdMaxRetries) {
        if (!channel->cmd(at::AT::SMS_UCSC2)) {
            ++ucscSetRetries;
            LOG_ERROR("Could not set UCS2 charset mode for TE. Retry %d", ucscSetRetries);
        }
        else {
            break;
        }
    }
    auto _ = gsl::finally([&channel, &retVal, &messageNumber] {
        if (!channel->cmd(at::AT::SMS_GSM)) {
            LOG_ERROR("Could not set GSM (default) charset mode for TE");
        }

        // delete message from modem memory
        if (retVal && !channel->cmd(at::factory(at::AT::CMGD) + messageNumber)) {
            LOG_ERROR("Could not delete SMS from modem");
        }
    });

    const auto &cmd = at::factory(at::AT::QCMGR);
    at::Result rawMessage;
    auto qcmgrRetries = 0;

    while (qcmgrRetries < at::AtCmdMaxRetries) {
        rawMessage = channel->cmd(cmd + messageNumber, cmd.getTimeout());
        if (!rawMessage) {
            ++qcmgrRetries;
            LOG_ERROR("Could not read text message. Retry %d", qcmgrRetries);
            // There are cases where +QCMGR command is issued to soon after +CMTI URC,
            // and modem is still busy processing messages. This results with an error
            // and some garbage frame. To work around this, we delay a bit and retry +QCMGR
            // to get the message.
            vTaskDelay(at::AtCmdRetryDelayMS);
        }
        else {
            break;
        }
    }

    if (qcmgrRetries == at::AtCmdMaxRetries) {
        LOG_ERROR("Could not read text message");
        return false;
    }

    if (!rawMessage) {
        LOG_ERROR("AT invalid result: %s", magic_enum::enum_name(rawMessage.code).data());
        return false;
    }

    // The modem should send 3 responses:
    //  1. Data with phone number, time etc,
    //  2. Message content
    //  3. Status: OK, ERROR
    auto receivedMessages = rawMessage.response.size();
    if (receivedMessages < 3) {
        LOG_ERROR("AT response too short: %zu", receivedMessages);
        return false;
    }

    for (std::size_t i = 0; i < receivedMessages; i++) {
        bool messageParsed = SMSParser::parse(&rawMessage.response[i]);
        if (messageParsed) {
            UTF8 decodedMessage;
            UTF8 smsNumber         = SMSParser::getNumber();
            std::string smsMessage = SMSParser::getMessage();

            std::string decodedStr;
            try {
                decodedStr = utils::hexToBytes<std::string>(smsMessage);
            }
            catch (const std::exception &e) {
                LOG_ERROR("Decoded SMS: %s", e.what());
                retVal = false;
                break;
            }
            const auto mmsNotificationOpt = pdu::parse(decodedStr);
            if (mmsNotificationOpt) {
                std::string number = numberFromAddress(mmsNotificationOpt->fromAddress);
                // NOTE: number may be empty
                decodedMessage = UTF8("[MMS]");
                smsNumber      = UTF8(number);
            }

            if (decodedMessage.empty()) {
                decodedMessage = UCS2(smsMessage).toUTF8();
            }
            smsMessage.clear();

            auto messageDate  = SMSParser::getTime();
            const auto record = createSMSRecord(decodedMessage, smsNumber, messageDate);

            if (!dbAddSMSRecord(record)) {
                LOG_ERROR("Failed to add text message to db");
                retVal = false;
                break;
            }
        }
    }
    return retVal;
}

bool ServiceCellular::getOwnNumber(std::string &destination)
{
    auto ret = cmux->get(CellularMux::Channel::Commands)->cmd(at::AT::CNUM);

    if (ret) {
        auto begin = ret.response[0].find(',');
        auto end   = ret.response[0].rfind(',');
        if (begin != std::string::npos && end != std::string::npos) {
            std::string number;
            try {
                number = ret.response[0].substr(begin, end - begin);
            }
            catch (std::exception &e) {
                LOG_ERROR("ServiceCellular::getOwnNumber exception: %s", e.what());
                return false;
            }
            number.erase(std::remove(number.begin(), number.end(), '"'), number.end());
            number.erase(std::remove(number.begin(), number.end(), ','), number.end());

            destination = number;
            return true;
        }
    }
    LOG_ERROR("ServiceCellular::getOwnNumber failed.");
    return false;
}

bool ServiceCellular::getIMSI(std::string &destination, bool fullNumber)
{
    auto ret = cmux->get(CellularMux::Channel::Commands)->cmd(at::AT::CIMI);

    if (ret) {
        if (fullNumber) {
            destination = ret.response[0];
        }
        else {
            try {
                destination = ret.response[0].substr(0, 3);
            }
            catch (std::exception &e) {
                LOG_ERROR("ServiceCellular::getIMSI exception: %s", e.what());
                return false;
            }
        }
        return true;
    }
    LOG_ERROR("ServiceCellular::getIMSI failed.");
    return false;
}

std::vector<std::string> ServiceCellular::getNetworkInfo()
{
    std::vector<std::string> data;
    auto channel = cmux->get(CellularMux::Channel::Commands);
    if (channel) {
        auto resp = channel->cmd(at::AT::CSQ);
        if (resp.code == at::Result::Code::OK) {
            data.push_back(resp.response[0]);
        }
        else {
            LOG_ERROR("CSQ Error");
            data.push_back("");
        }

        resp = channel->cmd(at::AT::CREG);
        if (resp.code == at::Result::Code::OK) {
            data.push_back(resp.response[0]);
        }
        else {
            LOG_ERROR("CREG Error");
            data.push_back("");
        }

        resp = channel->cmd(at::AT::QNWINFO);
        if (resp.code == at::Result::Code::OK) {
            std::string ret;
            if (at::response::parseQNWINFO(resp.response[0], ret)) {
                data.push_back(ret);
            }
            else {
                data.push_back("");
            }
        }
        else {
            LOG_ERROR("QNWINFO Error");
            data.push_back("");
        }
    }
    return data;
}

std::vector<std::string> get_last_AT_error(DLCChannel *channel)
{
    auto ret = channel->cmd(at::AT::CEER);
    return std::move(ret.response);
}

void log_last_AT_error(DLCChannel *channel)
{
    std::vector<std::string> atErrors(get_last_AT_error(channel));
    int i = 1;
    for (auto &msg_line : atErrors) {
        LOG_ERROR("%d/%d: %s", i, static_cast<int>(atErrors.size()), msg_line.c_str());
        i++;
    }
}

bool is_SIM_detection_enabled(DLCChannel *channel)
{
    auto ret = channel->cmd(at::AT::SIM_DET);
    if (ret) {
        if (ret.response[0].find("+QSIMDET: 1") != std::string::npos) {
            LOG_DEBUG("SIM detecition enabled!");
            return true;
        }
    }
    else {
        LOG_FATAL("Cant check sim detection status!");
        log_last_AT_error(channel);
    }
    return false;
}

bool enable_SIM_detection(DLCChannel *channel)
{
    auto ret = channel->cmd(at::AT::SIM_DET_ON);
    if (!ret) {
        log_last_AT_error(channel);
        return false;
    }
    return true;
}

bool is_SIM_status_enabled(DLCChannel *channel)
{
    auto ret = channel->cmd(at::AT::QSIMSTAT);
    if (ret) {
        if (ret.response[0].find("+QSIMSTAT: 1") != std::string::npos) {
            LOG_DEBUG("SIM swap enabled!");
            return true;
        }
    }
    else {
        LOG_FATAL("SIM swap status failure! %s", ret.response[0].c_str());
        log_last_AT_error(channel);
    }
    return false;
}

bool enable_SIM_status(DLCChannel *channel)
{
    auto ret = channel->cmd(at::AT::SIMSTAT_ON);
    if (!ret) {
        log_last_AT_error(channel);
        return false;
    }
    return true;
}

void save_SIM_detection_status(DLCChannel *channel)
{
    auto ret = channel->cmd(at::AT::STORE_SETTINGS_ATW);
    if (!ret) {
        log_last_AT_error(channel);
    }
}

bool sim_check_hot_swap(DLCChannel *channel)
{
    assert(channel);
    bool reboot_needed = false;

    if (!is_SIM_detection_enabled(channel)) {
        reboot_needed = true;
    }
    if (!is_SIM_status_enabled(channel)) {
        reboot_needed = true;
    }
    if (reboot_needed) {
        enable_SIM_detection(channel);
        enable_SIM_status(channel);
        save_SIM_detection_status(channel);
        LOG_FATAL("Modem reboot required, Please remove battery!");
    }
    return !reboot_needed;
}

bool ServiceCellular::handle_sim_sanity_check()
{
    auto ret = sim_check_hot_swap(cmux->get(CellularMux::Channel::Commands));
    if (ret) {
        priv->state->set(State::ST::ModemOn);
    }
    else {
        LOG_ERROR("Sanity check failure - modem has to be rebooted");
        priv->modemResetHandler->performHardReset();
    }
    return ret;
}

bool ServiceCellular::handle_modem_on()
{
    auto channel = cmux->get(CellularMux::Channel::Commands);
    channel->cmd("AT+CCLK?");
    // inform host ap ready
    cmux->informModemHostWakeup();
    tetheringTurnOnURC();
    priv->state->set(State::ST::URCReady);
    LOG_DEBUG("AP ready");
    return true;
}

bool ServiceCellular::handle_URCReady()
{
    auto channel = cmux->get(CellularMux::Channel::Commands);
    bool ret     = true;

    ret = ret && channel->cmd(at::AT::ENABLE_NETWORK_REGISTRATION_URC);

    bus.sendMulticast<cellular::msg::notification::ModemStateChanged>(cellular::api::ModemState::Ready);

    channel->onWakeUpModem = [this]() { WakeUpHandler(); };
    channel->onSleepModem  = [this]() { sleepTimer.restart(constants::sleepTimerInterval); };

    LOG_DEBUG("%s", priv->state->c_str());
    return ret;
}

bool ServiceCellular::handleTextMessagesInit()
{
    auto channel = cmux->get(CellularMux::Channel::Commands);
    if (channel == nullptr) {
        LOG_ERROR("Cant configure sim! no Commands channel!");
        return false;
    }

    auto commands = at::getCommadsSet(at::commadsSet::smsInit);
    for (const auto &command : commands) {
        if (!channel->cmd(command)) {
            LOG_ERROR("Text messages init failed!");
            return false;
        }
    }
    if (!receiveAllMessages()) {
        LOG_ERROR("Receiving all messages from modem failed");
        return true; // this is not blocking issue
    }
    return true;
}

SMSRecord ServiceCellular::createSMSRecord(const UTF8 &decodedMessage,
                                           const UTF8 &receivedNumber,
                                           const time_t messageDate,
                                           const SMSType &smsType) const noexcept
{
    SMSRecord record{};
    record.body   = decodedMessage;
    record.number = utils::PhoneNumber::getReceivedNumberView(receivedNumber);
    record.type   = SMSType::INBOX;
    record.date   = messageDate;
    return record;
}

bool ServiceCellular::dbAddSMSRecord(const SMSRecord &record)
{
    return DBServiceAPI::AddSMS(
        this, record, db::QueryCallback::fromFunction([this, number = record.number](auto response) {
            auto result = dynamic_cast<db::query::SMSAddResult *>(response);
            if (result == nullptr || !result->result) {
                return false;
            }
            onSMSReceived(number);
            return true;
        }));
}

void ServiceCellular::onSMSReceived(const utils::PhoneNumber::View &number)
{
    DBServiceAPI::GetQuery(
        this,
        db::Interface::Name::Notifications,
        std::make_unique<db::query::notifications::Increment>(NotificationsRecord::Key::Sms, number));

    bus.sendMulticast(std::make_shared<cellular::IncomingSMSNotificationMessage>(),
                      sys::BusChannel::ServiceCellularNotifications);
}

bool ServiceCellular::receiveAllMessages()
{
    auto channel = cmux->get(CellularMux::Channel::Commands);
    if (channel == nullptr) {
        return false;
    }

    constexpr std::string_view cmd = "CMGL: ";
    if (auto ret = channel->cmd(at::AT::LIST_MESSAGES)) {
        for (std::size_t i = 0; i < ret.response.size(); i++) {
            if (auto pos = ret.response[i].find(cmd); pos != std::string::npos) {
                auto startPos = pos + cmd.size();
                auto endPos   = ret.response[i].find_first_of(',');
                if (receiveSMS(ret.response[i].substr(startPos, endPos - startPos))) {
                    LOG_WARN("Cannot receive text message - %" PRIu32 " / %" PRIu32,
                             static_cast<uint32_t>(i),
                             static_cast<uint32_t>(ret.response.size()));
                }
            }
        }
        return true;
    }
    else {
        return false;
    }
}

bool ServiceCellular::handle_failure()
{
    priv->state->set(State::ST::Idle);
    bus.sendMulticast<cellular::msg::notification::ModemStateChanged>(cellular::api::ModemState::Fail);
    return true;
}

bool ServiceCellular::handle_fatal_failure()
{
    LOG_FATAL("Await for death!");
    bus.sendMulticast<cellular::msg::notification::ModemStateChanged>(cellular::api::ModemState::Fatal);
    while (true) {
        vTaskDelay(500);
    }
    return true;
}

bool ServiceCellular::handle_ready()
{
    LOG_DEBUG("%s", priv->state->c_str());
    return true;
}

bool ServiceCellular::SetScanMode(std::string mode)
{
    auto channel = cmux->get(CellularMux::Channel::Commands);
    if (channel) {
        auto command = at::factory(at::AT::SET_SCANMODE);

        auto resp = channel->cmd(command.getCmd() + mode + ",1", command.getTimeout(), 1);
        if (resp.code == at::Result::Code::OK) {
            return true;
        }
    }
    return false;
}

std::string ServiceCellular::GetScanMode(void)
{
    auto channel = cmux->get(CellularMux::Channel::Commands);
    if (channel) {

        auto resp = channel->cmd(at::AT::GET_SCANMODE);
        if (resp.code == at::Result::Code::OK) {
            auto beg = resp.response[0].find(",");
            if (beg != std::string::npos) {
                auto response = resp.response[0].substr(beg + 1, 1);
                return response;
            }
        }
        else {
            LOG_ERROR("Unable to get network search mode configuration");
        }
    }
    return {};
}

bool ServiceCellular::transmitDtmfTone(DTMFCode code)
{
    auto channel = cmux->get(CellularMux::Channel::Commands);
    at::Result resp;
    if (channel) {
        auto command = at::factory(at::AT::QLDTMF);
        resp         = channel->cmd(command.getCmd() + std::string(code));
        if (resp) {
            command = at::factory(at::AT::VTS);
            resp    = channel->cmd(command.getCmd() + std::string(code));
        }
    }
    return resp.code == at::Result::Code::OK;
}

void ServiceCellular::handle_CellularGetChannelMessage()
{
    connect(cellular::GetChannelMessage(), [&](sys::Message *req) {
        auto getChannelMsg = static_cast<cellular::GetChannelMessage *>(req);
        LOG_DEBUG("Handle request for channel: %s", CellularMux::name(getChannelMsg->dataChannel).c_str());
        std::shared_ptr<cellular::GetChannelResponseMessage> channelResponsMessage =
            std::make_shared<cellular::GetChannelResponseMessage>(cmux->get(getChannelMsg->dataChannel));
        LOG_DEBUG("channel ptr: %p", channelResponsMessage->dataChannelPtr);
        bus.sendUnicast(std::move(channelResponsMessage), req->sender);
        return sys::MessageNone{};
    });
}
bool ServiceCellular::handle_status_check(void)
{
    LOG_INFO("Checking modem status.");
    auto modemActive = cmux->isModemActive();
    if (modemActive) {
        // modem is already turned on, call configutarion procedure
        LOG_INFO("Modem is already turned on.");
        LOG_DEBUG("RT1051 - hot start");
        priv->state->set(State::ST::PowerUpInProgress);
    }
    else {
        priv->state->set(State::ST::PowerUpProcedure);
    }
    return true;
}

void ServiceCellular::startStateTimer(uint32_t timeout)
{
    stateTimeout = timeout;
    stateTimer.start();
}

void ServiceCellular::stopStateTimer()
{
    stateTimeout = 0;
    stateTimer.stop();
}

void ServiceCellular::handleStateTimer(void)
{
    stateTimeout--;
    if (stateTimeout == 0) {
        stopStateTimer();
        LOG_FATAL("State %s timeout occured!", priv->state->c_str());
        priv->state->set(State::ST::ModemFatalFailure);
    }
}

void ServiceCellular::handle_power_state_change()
{
    nextPowerStateChangeAwaiting = false;
    auto modemActive             = cmux->isModemActive();

    if (priv->nextPowerState == State::PowerState::On) {
        if (priv->state->get() == State::ST::PowerDownWaiting) {
            LOG_DEBUG("Powerdown in progress. Powerup request queued.");
            nextPowerStateChangeAwaiting = true;
        }
        else if (priv->state->get() == State::ST::PowerUpProcedure ||
                 priv->state->get() == State::ST::PowerUpInProgress) {
            LOG_DEBUG("Powerup already in progress");
        }
        else if (priv->state->get() == State::ST::PowerDown ||
                 priv->state->get() == State::ST::WaitForStartPermission) {
            LOG_INFO("Modem Power UP.");
            priv->state->set(State::ST::PowerUpRequest);
        }
        else {
            LOG_DEBUG("Modem already powered up.");
        }
    }
    else {
        if (priv->state->get() == State::ST::PowerUpProcedure || priv->state->get() == State::ST::PowerUpInProgress) {
            LOG_DEBUG("Powerup in progress. Powerdown request queued.");
            nextPowerStateChangeAwaiting = true;
        }
        else if (priv->state->get() == State::ST::PowerDownWaiting) {
            LOG_DEBUG("Powerdown already in progress.");
        }
        else if (priv->state->get() == State::ST::PowerDown) {
            LOG_DEBUG("Modem already powered down.");
        }
        else if (priv->state->get() == State::ST::WaitForStartPermission && !modemActive) {
            LOG_DEBUG("Modem already powered down.");
            priv->state->set(State::ST::PowerDown);
        }
        else {
            LOG_INFO("Modem Power DOWN.");
            cmux->turnOffModem();
            priv->state->set(State::ST::PowerDownWaiting);
        }
    }
}

bool ServiceCellular::handleUSSDRequest(cellular::USSDMessage::RequestType requestType, const std::string &request)
{
    constexpr uint32_t commandTimeout = 120000;

    auto channel = cmux->get(CellularMux::Channel::Commands);
    if (channel != nullptr) {
        if (requestType == cellular::USSDMessage::RequestType::pullSessionRequest) {
            channel->cmd(at::AT::SMS_GSM);
            std::string command = at::factory(at::AT::CUSD_SEND) + request + ",15";
            auto result         = channel->cmd(command, std::chrono::milliseconds(commandTimeout));
            if (result.code == at::Result::Code::OK) {
                ussdState = ussd::State::pullRequestSent;
                setUSSDTimer();
            }
        }
        else if (requestType == cellular::USSDMessage::RequestType::abortSession) {

            ussdState   = ussd::State::sessionAborted;
            auto result = channel->cmd(at::AT::CUSD_CLOSE_SESSION);
            if (result.code == at::Result::Code::OK) {
                CellularServiceAPI::USSDRequest(this, cellular::USSDMessage::RequestType::pushSessionRequest);
            }
            else {
                CellularServiceAPI::USSDRequest(this, cellular::USSDMessage::RequestType::abortSession);
            }
        }
        else if (requestType == cellular::USSDMessage::RequestType::pushSessionRequest) {

            ussdState   = ussd::State::pushSession;
            auto result = channel->cmd(at::AT::CUSD_OPEN_SESSION);
            if (result.code == at::Result::Code::OK) {}
        }
        return true;
    }
    return false;
}

void ServiceCellular::handleUSSDTimer(void)
{
    if (ussdTimeout > 0) {
        ussdTimeout -= 1;
    }
    else {
        LOG_WARN("USSD timeout occurred, aborting current session");
        ussdTimer.stop();
        CellularServiceAPI::USSDRequest(this, cellular::USSDMessage::RequestType::abortSession);
    }
}
void ServiceCellular::setUSSDTimer(void)
{
    switch (ussdState) {
    case ussd::State::pullRequestSent:
        ussdTimeout = ussd::pullResponseTimeout;
        break;
    case ussd::State::pullResponseReceived:
        ussdTimeout = ussd::pullSesionTimeout;
        break;
    case ussd::State::pushSession:
    case ussd::State::sessionAborted:
    case ussd::State::none:
        ussdTimeout = ussd::noTimeout;
        break;
    }
    if (ussdTimeout == ussd::noTimeout) {
        ussdTimer.stop();
        return;
    }
    ussdTimer.start();
}

std::shared_ptr<cellular::RawCommandRespAsync> ServiceCellular::handleCellularStartOperatorsScan(
    cellular::StartOperatorsScanMessage *msg)
{
    LOG_INFO("CellularStartOperatorsScan handled");
    auto ret = std::make_shared<cellular::RawCommandRespAsync>(CellularMessage::Type::OperatorsScanResult);
    NetworkSettings networkSettings(*this);
    ret->data = networkSettings.scanOperators(msg->getFullInfo());
    bus.sendUnicast(ret, msg->sender);
    return ret;
}

bool ServiceCellular::handle_apn_conf_procedure()
{
    LOG_DEBUG("APN on modem configuration");
    packetData->setupAPNSettings();
    priv->state->set(State::ST::SanityCheck);
    return true;
}

void ServiceCellular::handleCellularRequestCurrentOperatorName(cellular::RequestCurrentOperatorNameMessage *msg)
{
    LOG_INFO("CellularRequestCurrentOperatorName handled");
    NetworkSettings networkSettings(*this);
    const auto currentNetworkOperatorName = networkSettings.getCurrentOperatorName();
    Store::GSM::get()->setNetworkOperatorName(currentNetworkOperatorName);
    auto notification = std::make_shared<cellular::CurrentOperatorNameNotification>(currentNetworkOperatorName);
    this->bus.sendMulticast(std::move(notification), sys::BusChannel::ServiceCellularNotifications);
}

std::shared_ptr<cellular::GetAPNResponse> ServiceCellular::handleCellularGetAPNMessage(cellular::GetAPNMessage *msg)
{
    std::vector<std::shared_ptr<packet_data::APN::Config>> apns;

    if (auto type = msg->getAPNType(); type) {
        if (auto apn = packetData->getAPNFirst(*type); apn) {
            apns.push_back(*apn);
        }
        return std::make_shared<cellular::GetAPNResponse>(apns);
    }

    if (auto ctxid = msg->getContextId(); ctxid) {
        if (auto apn = packetData->getAPN(*ctxid); apn) {
            apns.push_back(*apn);
        }
        return std::make_shared<cellular::GetAPNResponse>(apns);
    }

    return std::make_shared<cellular::GetAPNResponse>(packetData->getAPNs());
}

std::shared_ptr<cellular::SetAPNResponse> ServiceCellular::handleCellularSetAPNMessage(cellular::SetAPNMessage *msg)
{
    auto apn = msg->getAPNConfig();
    auto ret = packetData->setAPN(apn);
    settings->setValue(settings::Cellular::apn_list, packetData->saveAPNSettings(), settings::SettingsScope::Global);
    return std::make_shared<cellular::SetAPNResponse>(ret);
}
std::shared_ptr<cellular::NewAPNResponse> ServiceCellular::handleCellularNewAPNMessage(cellular::NewAPNMessage *msg)
{
    auto apn           = msg->getAPNConfig();
    std::uint8_t newId = 0;
    auto ret           = packetData->newAPN(apn, newId);
    settings->setValue(settings::Cellular::apn_list, packetData->saveAPNSettings(), settings::SettingsScope::Global);
    return std::make_shared<cellular::NewAPNResponse>(ret, newId);
}

std::shared_ptr<cellular::SetDataTransferResponse> ServiceCellular::handleCellularSetDataTransferMessage(
    cellular::SetDataTransferMessage *msg)
{
    packetData->setDataTransfer(msg->getDataTransfer());
    return std::make_shared<cellular::SetDataTransferResponse>(at::Result::Code::OK);
}

std::shared_ptr<cellular::GetDataTransferResponse> ServiceCellular::handleCellularGetDataTransferMessage(
    cellular::GetDataTransferMessage *msg)
{
    return std::make_shared<cellular::GetDataTransferResponse>(packetData->getDataTransfer());
}

std::shared_ptr<cellular::ActivateContextResponse> ServiceCellular::handleCellularActivateContextMessage(
    cellular::ActivateContextMessage *msg)
{
    return std::make_shared<cellular::ActivateContextResponse>(packetData->activateContext(msg->getContextId()),
                                                               msg->getContextId());
}

std::shared_ptr<cellular::DeactivateContextResponse> ServiceCellular::handleCellularDeactivateContextMessage(
    cellular::DeactivateContextMessage *msg)
{
    return std::make_shared<cellular::DeactivateContextResponse>(packetData->deactivateContext(msg->getContextId()),
                                                                 msg->getContextId());
}

std::shared_ptr<cellular::GetActiveContextsResponse> ServiceCellular::handleCellularGetActiveContextsMessage(
    cellular::GetActiveContextsMessage *msg)
{
    return std::make_shared<cellular::GetActiveContextsResponse>(packetData->getActiveContexts());
}

std::shared_ptr<cellular::SetOperatorAutoSelectResponse> ServiceCellular::handleCellularSetOperatorAutoSelect(
    cellular::SetOperatorAutoSelectMessage *msg)
{
    LOG_INFO("CellularSetOperatorAutoSelect handled");

    NetworkSettings networkSettings(*this);
    return std::make_shared<cellular::SetOperatorAutoSelectResponse>(networkSettings.setOperatorAutoSelect());
}

std::shared_ptr<cellular::SetOperatorResponse> ServiceCellular::handleCellularSetOperator(
    cellular::SetOperatorMessage *msg)
{
    LOG_INFO("CellularSetOperatorAutoSelect handled");

    NetworkSettings networkSettings(*this);
    return std::make_shared<cellular::SetOperatorResponse>(
        networkSettings.setOperator(msg->getMode(), msg->getFormat(), msg->getName()));
}

void ServiceCellular::apnListChanged(const std::string &value)
{
    if (!value.empty()) {
        LOG_INFO("apn_list setting state changed to '%s'.", value.c_str());
        packetData->loadAPNSettings(value);
    }
}

auto ServiceCellular::handleCellularAnswerIncomingCallMessage(CellularMessage *msg)
    -> std::shared_ptr<cellular::ResponseMessage>
{
    LOG_INFO("%s", __PRETTY_FUNCTION__);
    auto ret = ongoingCall->handle(call::event::Answer{});
    return std::make_shared<cellular::ResponseMessage>(ret);
}

namespace
{
    constexpr auto translate(at::Result::Code code) -> cellular::CallRequestGeneralError::ErrorType
    {
        switch (code) {
        case at::Result::Code::ERROR:
        case at::Result::Code::CME_ERROR:
        case at::Result::Code::CMS_ERROR:
            return cellular::CallRequestGeneralError::ErrorType::Error;
        case at::Result::Code::TIMEOUT:
            return cellular::CallRequestGeneralError::ErrorType::ModemTimeout;
        case at::Result::Code::TOKENS:
        case at::Result::Code::PARSING_ERROR:
        case at::Result::Code::FULL_MSG_BUFFER:
        case at::Result::Code::TRANSMISSION_NOT_STARTED:
        case at::Result::Code::RECEIVING_NOT_STARTED:
        case at::Result::Code::DATA_NOT_USED:
        case at::Result::Code::CMUX_FRAME_ERROR:
            return cellular::CallRequestGeneralError::ErrorType::TransmissionError;
        case at::Result::Code::OK:
        case at::Result::Code::NONE:
        case at::Result::Code::UNDEFINED:
            return cellular::CallRequestGeneralError::ErrorType::UndefinedError;
        }
        return cellular::CallRequestGeneralError::ErrorType::UndefinedError;
    }
} // namespace

auto ServiceCellular::handleCellularCallRequestMessage(cellular::CallRequestMessage *msg)
    -> std::shared_ptr<cellular::ResponseMessage>
{
    LOG_INFO("%s", __PRETTY_FUNCTION__);
    auto channel = cmux->get(CellularMux::Channel::Commands);
    if (channel == nullptr) {
        LOG_WARN("commands channel not ready");
        auto message = std::make_shared<cellular::CallRequestGeneralError>(
            cellular::CallRequestGeneralError::ErrorType::ChannelNotReadyError);
        bus.sendUnicast(message, ::service::name::appmgr);
        return std::make_shared<cellular::ResponseMessage>(false);
    }
    cellular::RequestFactory factory(
        msg->number.getEntered(), *channel, msg->callMode, Store::GSM::get()->simCardInserted());

    auto request = factory.create();
    CellularRequestHandler handler(*this);
    auto result = channel->cmd(request->command());
    request->handle(handler, result);
    if (!result) {
        log_last_AT_error(channel);
    }
    LOG_INFO("isHandled %d, %s", static_cast<int>(request->isHandled()), utils::enumToString(result.code).c_str());
    if (!request->isHandled()) {
        cellular::CallRequestGeneralError::ErrorType errorType = translate(result.code);
        auto message = std::make_shared<cellular::CallRequestGeneralError>(errorType);
        bus.sendUnicast(message, ::service::name::appmgr);
    }

    return std::make_shared<cellular::ResponseMessage>(request->isHandled());
}

void ServiceCellular::handleCellularHangupCallMessage(cellular::HangupCallMessage *msg)
{
    LOG_INFO("%s", __PRETTY_FUNCTION__);
    if (!ongoingCall->handle(call::event::Reject{})) {
        LOG_ERROR("Failed to end ongoing call");
    }
    callStateTimer.stop();
    callEndedRecentlyTimer.start();
}

void ServiceCellular::handleCellularDismissCallMessage(sys::Message *msg)
{
    LOG_INFO("%s", __PRETTY_FUNCTION__);
    hangUpCall();
}

auto ServiceCellular::handleDBQueryResponseMessage(db::QueryResponse *msg) -> std::shared_ptr<sys::ResponseMessage>
{
    bool responseHandled = false;

    auto result = msg->getResult();
    if (auto response = dynamic_cast<db::query::SMSSearchByTypeResult *>(result.get())) {
        responseHandled = handle(response);
    }
    else if (result->hasListener()) {
        responseHandled = result->handle();
    }

    if (responseHandled) {
        return std::make_shared<sys::ResponseMessage>();
    }
    else {
        return std::make_shared<sys::ResponseMessage>(sys::ReturnCodes::Unresolved);
    }
}

auto ServiceCellular::handleCellularListCallsMessage(CellularMessage *msg) -> std::shared_ptr<sys::ResponseMessage>
{
    at::cmd::CLCC cmd;
    auto base = cmux->get(CellularMux::Channel::Commands)->cmd(cmd);
    if (auto response = cmd.parseCLCC(base); response) {
        const auto &data = response.getData();
        auto it          = std::find_if(std::begin(data), std::end(data), [&](const auto &entry) {
            return entry.stateOfCall == ModemCall::CallState::Active && entry.mode == ModemCall::CallMode::Voice;
        });

        if (it != std::end(data)) {
            ongoingCall->handle(call::event::Answer{});
            callStateTimer.stop();
            return std::make_shared<cellular::ResponseMessage>(true);
        }
    }
    return std::make_shared<cellular::ResponseMessage>(false);
}

auto ServiceCellular::handleDBNotificationMessage(db::NotificationMessage *msg) -> std::shared_ptr<sys::ResponseMessage>
{
    if (msg->interface == db::Interface::Name::SMS &&
        (msg->type == db::Query::Type::Create || msg->type == db::Query::Type::Update)) {

        priv->outSMSHandler.handleDBNotification();
        return std::make_shared<sys::ResponseMessage>();
    }
    return std::make_shared<sys::ResponseMessage>(sys::ReturnCodes::Failure);
}

auto ServiceCellular::handleCellularRingingMessage(cellular::RingingMessage *msg)
    -> std::shared_ptr<sys::ResponseMessage>
{
    LOG_INFO("%s", __PRETTY_FUNCTION__);
    return std::make_shared<cellular::ResponseMessage>(
        ongoingCall->handle(call::event::StartCall{CallType::CT_OUTGOING, msg->number}));
}

auto ServiceCellular::handleCellularGetIMSIMessage(sys::Message *msg) -> std::shared_ptr<sys::ResponseMessage>
{
    std::string temp;
    if (getIMSI(temp)) {
        return std::make_shared<cellular::ResponseMessage>(true, temp);
    }
    return std::make_shared<cellular::ResponseMessage>(false);
}

auto ServiceCellular::handleCellularGetOwnNumberMessage(sys::Message *msg) -> std::shared_ptr<sys::ResponseMessage>
{
    std::string temp;
    if (getOwnNumber(temp)) {
        return std::make_shared<cellular::GetOwnNumberResponseMessage>(true, temp);
    }
    return std::make_shared<cellular::GetOwnNumberResponseMessage>(false);
}

auto ServiceCellular::handleCellularGetNetworkInfoMessage(sys::Message *msg) -> std::shared_ptr<sys::ResponseMessage>
{
    auto message  = std::make_shared<cellular::RawCommandRespAsync>(CellularMessage::Type::NetworkInfoResult);
    message->data = getNetworkInfo();
    bus.sendUnicast(message, msg->sender);

    return std::make_shared<cellular::ResponseMessage>(true);
}

auto ServiceCellular::handleCellularSelectAntennaMessage(sys::Message *msg) -> std::shared_ptr<sys::ResponseMessage>
{
    auto message = static_cast<cellular::AntennaRequestMessage *>(msg);

    cmux->selectAntenna(message->antenna);
    vTaskDelay(50); // sleep for 50 ms...
    auto actualAntenna = cmux->getAntenna();
    if (actualAntenna == bsp::cellular::antenna::lowBand) {
        LOG_INFO("Low band antenna set");
    }
    else {
        LOG_INFO("High band antenna set");
    }
    bool changedAntenna = (actualAntenna == message->antenna);

    auto notification = std::make_shared<AntennaChangedMessage>();
    bus.sendMulticast(notification, sys::BusChannel::AntennaNotifications);

    return std::make_shared<cellular::ResponseMessage>(changedAntenna);
}
auto ServiceCellular::handleCellularSetScanModeMessage(sys::Message *msg) -> std::shared_ptr<sys::ResponseMessage>
{
    auto message = static_cast<cellular::SetScanModeMessage *>(msg);
    bool ret     = SetScanMode(message->data);

    return std::make_shared<cellular::ResponseMessage>(ret);
}
auto ServiceCellular::handleCellularGetScanModeMessage(sys::Message *msg) -> std::shared_ptr<sys::ResponseMessage>
{
    auto mode = GetScanMode();
    if (mode != "") {
        auto response = std::make_shared<cellular::RawCommandRespAsync>(CellularMessage::Type::GetScanModeResult);
        response->data.push_back(mode);
        bus.sendUnicast(response, msg->sender);
        return std::make_shared<cellular::ResponseMessage>(true);
    }
    return std::make_shared<cellular::ResponseMessage>(false);
}

auto ServiceCellular::handleCellularGetFirmwareVersionMessage(sys::Message *msg)
    -> std::shared_ptr<sys::ResponseMessage>
{
    std::string response;
    auto channel = cmux->get(CellularMux::Channel::Commands);
    if (channel) {
        auto resp = channel->cmd(at::AT::QGMR);
        if (resp.code == at::Result::Code::OK) {
            response = resp.response[0];
            return std::make_shared<cellular::ResponseMessage>(true, response);
        }
    }
    return std::make_shared<cellular::ResponseMessage>(false);
}
auto ServiceCellular::handleEVMStatusMessage(sys::Message *msg) -> std::shared_ptr<sys::ResponseMessage>
{
    using namespace bsp::cellular::status;
    auto message    = static_cast<sevm::StatusStateMessage *>(msg);
    auto status_pin = message->state;
    if (priv->modemResetHandler->handleStatusPinEvent(status_pin == value::ACTIVE)) {
        return std::make_shared<cellular::ResponseMessage>(true);
    }

    if (status_pin == value::ACTIVE) {
        if (priv->state->get() == State::ST::PowerUpProcedure) {
            priv->state->set(State::ST::PowerUpInProgress); // and go to baud detect as usual
        }
    }
    if (status_pin == value::INACTIVE) {
        if (priv->state->get() == State::ST::PowerDownWaiting) {
            priv->state->set(State::ST::PowerDown);
        }
    }
    return std::make_shared<cellular::ResponseMessage>(true);
}

auto ServiceCellular::handleCellularGetCsqMessage(sys::Message *msg) -> std::shared_ptr<sys::ResponseMessage>
{
    auto channel = cmux->get(CellularMux::Channel::Commands);
    if (channel) {
        auto modemResponse = channel->cmd(at::AT::CSQ);
        if (modemResponse.code == at::Result::Code::OK) {
            return std::make_shared<cellular::ResponseMessage>(true, modemResponse.response[0]);
        }
    }
    return std::make_shared<cellular::ResponseMessage>(false);
}

auto ServiceCellular::handleCellularGetCregMessage(sys::Message *msg) -> std::shared_ptr<sys::ResponseMessage>
{
    auto channel = cmux->get(CellularMux::Channel::Commands);
    if (channel) {
        auto resp = channel->cmd(at::AT::CREG);
        if (resp.code == at::Result::Code::OK) {
            return std::make_shared<cellular::ResponseMessage>(true, resp.response[0]);
        }
    }
    return std::make_shared<cellular::ResponseMessage>(false);
}

auto ServiceCellular::handleCellularGetNwinfoMessage(sys::Message *msg) -> std::shared_ptr<sys::ResponseMessage>
{
    auto channel = cmux->get(CellularMux::Channel::Commands);
    if (channel) {
        auto resp = channel->cmd(at::AT::QNWINFO);
        if (resp.code == at::Result::Code::OK) {
            return std::make_shared<cellular::ResponseMessage>(true, resp.response[0]);
        }
    }
    return std::make_shared<cellular::ResponseMessage>(false);
}

auto ServiceCellular::handleCellularGetAntennaMessage(sys::Message *msg) -> std::shared_ptr<sys::ResponseMessage>
{
    auto antenna = cmux->getAntenna();
    return std::make_shared<cellular::AntennaResponseMessage>(true, antenna, CellularMessage::Type::GetAntenna);
}
auto ServiceCellular::handleCellularDtmfRequestMessage(sys::Message *msg) -> std::shared_ptr<sys::ResponseMessage>
{
    auto message = static_cast<cellular::DtmfRequestMessage *>(msg);
    auto resp    = transmitDtmfTone(message->getDTMFCode());
    return std::make_shared<cellular::ResponseMessage>(resp);
}
auto ServiceCellular::handleCellularUSSDMessage(sys::Message *msg) -> std::shared_ptr<sys::ResponseMessage>
{
    auto message = static_cast<cellular::USSDMessage *>(msg);
    return std::make_shared<cellular::ResponseMessage>(handleUSSDRequest(message->type, message->data));
}

auto ServiceCellular::handleStateRequestMessage(sys::Message *msg) -> std::shared_ptr<sys::ResponseMessage>
{
    change_state(dynamic_cast<cellular::StateChange *>(msg));
    return std::make_shared<cellular::ResponseMessage>(true);
}

auto ServiceCellular::handleCallActiveNotification(sys::Message *msg) -> std::shared_ptr<sys::ResponseMessage>
{
    auto ret = std::make_shared<cellular::ResponseMessage>(true);
    NetworkSettings networkSettings(*this);
    auto currentNAT = networkSettings.getCurrentNAT();
    if (currentNAT) {
        auto currentSimpleNAT = NetworkSettings::toSimpleNAT(*currentNAT);
        LOG_INFO("Current NAT %s(%s)",
                 utils::enumToString(*currentNAT).c_str(),
                 utils::enumToString(currentSimpleNAT).c_str());
        if (currentSimpleNAT == NetworkSettings::SimpleNAT::LTE) {
            LOG_INFO("VoLTE call");
        }
        else {
            LOG_INFO("Non VoLTE call");
        }
    }
    else {
        LOG_WARN("Cannot get current NAT");
    }
    return ret;
}

auto ServiceCellular::handlePowerUpProcedureCompleteNotification(sys::Message *msg)
    -> std::shared_ptr<sys::ResponseMessage>
{
    if (board == bsp::Board::Linux) {
        priv->state->set(State::ST::CellularConfProcedure);
    }
    return std::make_shared<cellular::ResponseMessage>(true);
}
auto ServiceCellular::handlePowerDownDeregisteringNotification(sys::Message *msg)
    -> std::shared_ptr<sys::ResponseMessage>
{
    if (priv->state->get() != State::ST::PowerDownWaiting) {
        priv->state->set(State::ST::PowerDownStarted);
        return std::make_shared<cellular::ResponseMessage>(true);
    }
    return std::make_shared<cellular::ResponseMessage>(false);
}
auto ServiceCellular::handlePowerDownDeregisteredNotification(sys::Message *msg)
    -> std::shared_ptr<sys::ResponseMessage>
{
    priv->state->set(State::ST::PowerDownWaiting);
    return std::make_shared<cellular::ResponseMessage>(true);
}
auto ServiceCellular::handleNewIncomingSMSNotification(sys::Message *msg) -> std::shared_ptr<sys::ResponseMessage>
{
    auto message      = static_cast<cellular::NewIncomingSMSNotification *>(msg);
    auto notification = std::make_shared<cellular::NewIncomingSMSMessage>(message->data);
    bus.sendUnicast(std::move(notification), msg->sender);
    return std::make_shared<cellular::ResponseMessage>(true);
}

auto ServiceCellular::handleSmsDoneNotification(sys::Message *msg) -> std::shared_ptr<sys::ResponseMessage>
{
    auto resp = handleTextMessagesInit();
    return std::make_shared<cellular::ResponseMessage>(resp);
}
auto ServiceCellular::handleSignalStrengthUpdateNotification(sys::Message *msg) -> std::shared_ptr<sys::ResponseMessage>
{
    return std::make_shared<cellular::ResponseMessage>(false);
}
auto ServiceCellular::handleNetworkStatusUpdateNotification(sys::Message *msg) -> std::shared_ptr<sys::ResponseMessage>
{
    return std::make_shared<cellular::ResponseMessage>(false);
}

auto ServiceCellular::handleUrcIncomingNotification(sys::Message *msg) -> std::shared_ptr<sys::ResponseMessage>
{
    WakeUpHandler();
    // if there is no response from the host to incoming URC,
    // the modem will automatically sleep after time constants::maxUrcHandleTime
    sleepTimer.restart(constants::maxUrcHandleTime);
    return std::make_shared<cellular::ResponseMessage>(true);
}

auto ServiceCellular::handleCellularSetFlightModeMessage(sys::Message *msg) -> std::shared_ptr<sys::ResponseMessage>
{
    auto setMsg = static_cast<cellular::SetFlightModeMessage *>(msg);
    settings->setValue(
        settings::Cellular::offlineMode, std::to_string(setMsg->flightModeOn), settings::SettingsScope::Global);
    connectionManager->setFlightMode(setMsg->flightModeOn);
    connectionManager->onPhoneModeChange(phoneModeObserver->getCurrentPhoneMode());
    return std::make_shared<cellular::ResponseMessage>(true);
}

auto ServiceCellular::handleCellularRingNotification(sys::Message *msg) -> std::shared_ptr<sys::ResponseMessage>
{
    LOG_INFO("%s", __PRETTY_FUNCTION__);
    ongoingCall->handle(call::event::RING{});
    return std::make_shared<cellular::ResponseMessage>(true);
}

auto ServiceCellular::handleCellularCallerIdNotification(sys::Message *msg) -> std::shared_ptr<sys::ResponseMessage>
{
    auto message = static_cast<cellular::CallerIdNotification *>(msg);
    ongoingCall->handle(call::event::CLIP{message->getNubmer()});
    return std::make_shared<cellular::ResponseMessage>(true);
}

auto ServiceCellular::handleCellularSetConnectionFrequencyMessage(sys::Message *msg)
    -> std::shared_ptr<sys::ResponseMessage>
{
    auto setMsg = static_cast<cellular::SetConnectionFrequencyMessage *>(msg);
    settings->setValue(settings::Offline::connectionFrequency,
                       std::to_string(setMsg->getConnectionFrequency()),
                       settings::SettingsScope::Global);

    connectionManager->setInterval(std::chrono::minutes{setMsg->getConnectionFrequency()});
    connectionManager->onPhoneModeChange(phoneModeObserver->getCurrentPhoneMode());
    return std::make_shared<cellular::ResponseMessage>(true);
}

auto ServiceCellular::hangUpCall() -> bool
{
    auto ret = ongoingCall->handle(call::event::Ended{});
    if (!ret) {
        LOG_ERROR("Failed to hang up call");
    }
    return ret;
}

auto ServiceCellular::hangUpCallBusy() -> bool
{
    auto channel = cmux->get(CellularMux::Channel::Commands);
    if (channel != nullptr) {
        if (channel->cmd(at::factory(at::AT::QHUP_BUSY))) {
            return true;
        }
    }
    LOG_ERROR("Failed to hang up call");
    return false;
}

auto ServiceCellular::tetheringTurnOffURC() -> bool
{
    auto channel = cmux->get(CellularMux::Channel::Commands);
    if (channel != nullptr) {
        if (!channel->cmd(at::factory(at::AT::CSQ_URC_OFF))) {
            LOG_ERROR("Failed to stop CSQ URC");
            return false;
        }
        if (!channel->cmd(at::factory(at::AT::ACT_URC_OFF))) {
            LOG_ERROR("Failed to stop ACT URC");
            return false;
        }
        if (!channel->cmd(at::factory(at::AT::SMS_URC_OFF))) {
            LOG_ERROR("Failed to stop SMS URC");
            return false;
        }
    }
    return true;
}

auto ServiceCellular::tetheringTurnOnURC() -> bool
{
    auto channel = cmux->get(CellularMux::Channel::Commands);
    if (channel != nullptr) {
        if (!channel->cmd(at::factory(at::AT::CSQ_URC_ON))) {
            LOG_ERROR("Failed to stop CSQ URC");
            return false;
        }
        if (!channel->cmd(at::factory(at::AT::ACT_URC_ON))) {
            LOG_ERROR("Failed to stop ACT URC");
            return false;
        }
        if (!channel->cmd(at::factory(at::AT::SMS_URC_ON))) {
            LOG_ERROR("Failed to stop SMS URC");
            return false;
        }
    }
    return true;
}

TaskHandle_t ServiceCellular::getTaskHandle()
{
    return xTaskGetCurrentTaskHandle();
}
