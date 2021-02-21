﻿// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "CellularUrcHandler.hpp"
#include "service-cellular/CellularCall.hpp"
#include "service-cellular/CellularMessage.hpp"
#include "service-cellular/CellularServiceAPI.hpp"
#include "service-cellular/ServiceCellular.hpp"
#include "service-cellular/SignalStrength.hpp"
#include "service-cellular/State.hpp"
#include "service-cellular/USSD.hpp"
#include "service-cellular/MessageConstants.hpp"

#include "SimCard.hpp"
#include "NetworkSettings.hpp"
#include "service-cellular/RequestFactory.hpp"
#include "service-cellular/CellularRequestHandler.hpp"
#include "SystemManager/messages/SentinelRegistrationMessage.hpp"

#include <Audio/AudioCommon.hpp>
#include <BaseInterface.hpp>
#include <CalllogRecord.hpp>
#include <Commands.hpp>
#include <Common/Common.hpp>
#include <Common/Query.hpp>
#include <MessageType.hpp>
#include <Modem/ATCommon.hpp>
#include <Modem/ATParser.hpp>
#include <Modem/TS0710/DLC_channel.h>
#include <Modem/TS0710/TS0710.h>
#include <Modem/TS0710/TS0710_START.h>
#include <NotificationsRecord.hpp>
#include <PhoneNumber.hpp>
#include <Result.hpp>
#include <Service/Message.hpp>
#include <Service/Service.hpp>
#include <Service/Timer.hpp>
#include <Tables/CalllogTable.hpp>
#include <Tables/Record.hpp>
#include <Utils.hpp>
#include <at/cmd/CLCC.hpp>
#include <at/UrcClip.hpp>
#include <at/UrcCmti.hpp>
#include <at/UrcCreg.hpp>
#include <at/UrcCtze.hpp>
#include <at/UrcCusd.hpp>
#include <at/UrcQind.hpp>
#include <at/UrcCpin.hpp> // for Cpin
#include <at/response.hpp>
#include <bsp/cellular/bsp_cellular.hpp>
#include <common_data/EventStore.hpp>
#include <country.hpp>
#include <log/log.hpp>
#include <module-cellular/at/UrcFactory.hpp>
#include <module-db/queries/messages/sms/QuerySMSSearchByType.hpp>
#include <module-db/queries/notifications/QueryNotificationsIncrement.hpp>
#include <projdefs.h>
#include <service-antenna/AntennaMessage.hpp>
#include <service-antenna/AntennaServiceAPI.hpp>
#include <service-antenna/ServiceAntenna.hpp>
#include <service-appmgr/Controller.hpp>
#include <service-audio/AudioServiceAPI.hpp>
#include <service-db/DBServiceAPI.hpp>
#include <service-db/DBNotificationMessage.hpp>
#include <service-db/QueryMessage.hpp>
#include <service-evtmgr/Constants.hpp>
#include <service-evtmgr/EventManagerServiceAPI.hpp>
#include <service-evtmgr/EVMessages.hpp>
#include "service-desktop/DesktopMessages.hpp"
#include <service-appmgr/model/ApplicationManager.hpp>
#include <task.h>
#include <time/time_conversion.hpp>
#include <ucs2/UCS2.hpp>
#include <utf8/UTF8.hpp>

#include <module-db/queries/messages/sms/QuerySMSUpdate.hpp>
#include <module-db/queries/messages/sms/QuerySMSAdd.hpp>

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

const char *ServiceCellular::serviceName = "ServiceCellular";

inline constexpr auto cellularStack = 25000UL;

using namespace cellular;

const char *State::c_str(State::ST state)
{
    switch (state) {
    case ST::Idle:
        return "Idle";
    case ST::WaitForStartPermission:
        return "WaitForStartPermission";
    case ST::PowerUpRequest:
        return "PowerUpRequest";
    case ST::StatusCheck:
        return "StatusCheck";
    case ST::PowerUpInProgress:
        return "PowerUpInProgress";
    case ST::PowerUpProcedure:
        return "PowerUpProcedure";
    case ST::BaudDetect:
        return "BaudDetect";
    case ST::AudioConfigurationProcedure:
        return "AudioConfigurationProcedure";
    case ST::APNConfProcedure:
        return "APNConfProcedure";
    case ST::ModemOn:
        return "ModemOn";
    case ST::URCReady:
        return "URCReady";
    case ST::SimSelect:
        return "SimSelect";
    case ST::Failed:
        return "Failed";
    case ST::SanityCheck:
        return "SanityCheck";
    case ST::SimInit:
        return "SimInit";
    case ST::ModemFatalFailure:
        return "ModemFatalFailure";
    case ST::CellularConfProcedure:
        return "CellularStartConfProcedure";
    case ST::Ready:
        return "Ready";
    case ST::PowerDownStarted:
        return "PowerDownStarted";
    case ST::PowerDownWaiting:
        return "PowerDownWaiting";
    case ST::PowerDown:
        return "PowerDown";
    }
    return "";
}

const char *State::c_str()
{
    return State::c_str(state);
}

void State::set(ServiceCellular *owner, ST state)
{
    assert(owner);
    LOG_DEBUG("GSM state: (%s) -> (%s)", c_str(this->state), c_str(state));
    this->state = state;
    auto msg    = std::make_shared<StateChange>(state);
    owner->bus.sendMulticast(msg, sys::BusChannel::ServiceCellularNotifications);
}

State::ST State::get() const
{
    return this->state;
}

ServiceCellular::ServiceCellular() : sys::Service(serviceName, "", cellularStack, sys::ServicePriority::Idle)
{

    LOG_INFO("[ServiceCellular] Initializing");

    bus.channels.push_back(sys::BusChannel::ServiceCellularNotifications);
    bus.channels.push_back(sys::BusChannel::ServiceDBNotifications);
    bus.channels.push_back(sys::BusChannel::ServiceEvtmgrNotifications);

    callStateTimer = std::make_unique<sys::Timer>("call_state", this, 1000);
    callStateTimer->connect([&](sys::Timer &) { CallStateTimerHandler(); });
    stateTimer = std::make_unique<sys::Timer>("state", this, 1000);
    stateTimer->connect([&](sys::Timer &) { handleStateTimer(); });
    ussdTimer = std::make_unique<sys::Timer>("ussd", this, 1000);
    ussdTimer->connect([&](sys::Timer &) { handleUSSDTimer(); });

    ongoingCall.setStartCallAction([=](const CalllogRecord &rec) {
        auto call = DBServiceAPI::CalllogAdd(this, rec);
        if (call.ID == DB_ID_NONE) {
            LOG_ERROR("CalllogAdd failed");
        }
        return call;
    });

    ongoingCall.setEndCallAction([=](const CalllogRecord &rec) {
        if (DBServiceAPI::CalllogUpdate(this, rec) && rec.type == CallType::CT_MISSED) {
            DBServiceAPI::GetQuery(
                this,
                db::Interface::Name::Notifications,
                std::make_unique<db::query::notifications::Increment>(NotificationsRecord::Key::Calls));
        }
        return true;
    });

    notificationCallback = [this](std::string &data) {
        LOG_DEBUG("Notifications callback called with %u data bytes", static_cast<unsigned int>(data.size()));

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
    settings->unregisterValueChange(settings::Cellular::volte_on, ::settings::SettingsScope::Global);
    settings->unregisterValueChange(settings::Cellular::apn_list, ::settings::SettingsScope::Global);
}

// this static function will be replaced by Settings API
static bool isSettingsAutomaticTimeSyncEnabled()
{
    return true;
}

void ServiceCellular::CallStateTimerHandler()
{
    LOG_DEBUG("CallStateTimerHandler");
    auto msg = std::make_shared<CellularRequestMessage>(MessageType::CellularListCurrentCalls);
    bus.sendUnicast(std::move(msg), ServiceCellular::serviceName);
}

sys::ReturnCodes ServiceCellular::InitHandler()
{
    board = EventManagerServiceAPI::GetBoard(this);

    state.set(this, State::ST::WaitForStartPermission);
    settings->registerValueChange(
        settings::Cellular::volte_on,
        [this](const std::string &value) { volteChanged(value); },
        ::settings::SettingsScope::Global);
    settings->registerValueChange(
        settings::Cellular::apn_list,
        [this](const std::string &value) { apnListChanged(value); },
        ::settings::SettingsScope::Global);

    cpuSentinel = std::make_shared<sys::CpuSentinel>(serviceName, this);

    auto sentinelRegistrationMsg = std::make_shared<sys::SentinelRegistrationMessage>(cpuSentinel);
    bus.sendUnicast(sentinelRegistrationMsg, service::name::system_manager);

    // temporarily limit the minimum CPU frequency
    // due to problems with the UART of the GSM modem
    cpuSentinel->HoldMinimumFrequency(bsp::CpuFrequencyHz::Level_4);

    return sys::ReturnCodes::Success;
}

sys::ReturnCodes ServiceCellular::DeinitHandler()
{

    return sys::ReturnCodes::Success;
}

sys::ReturnCodes ServiceCellular::SwitchPowerModeHandler(const sys::ServicePowerMode mode)
{
    LOG_FATAL("[ServiceCellular] PowerModeHandler: %s", c_str(mode));

    switch (mode) {
    case sys::ServicePowerMode ::Active:
        cmux->ExitSleepMode();
        break;
    case sys::ServicePowerMode ::SuspendToRAM:
    case sys::ServicePowerMode ::SuspendToNVM:
        cmux->EnterSleepMode();
        break;
    }

    return sys::ReturnCodes::Success;
}

void handleCellularSimNewPinDataMessage(CellularSimNewPinDataMessage *msg)
{}

void ServiceCellular::registerMessageHandlers()
{
    connect(typeid(CellularSimNewPinDataMessage), [&](sys::Message *request) -> sys::MessagePointer {
        auto msg = static_cast<CellularSimNewPinDataMessage *>(request);
        return std::make_shared<CellularResponseMessage>(
            changePin(SimCard::pinToString(msg->getOldPin()), SimCard::pinToString(msg->getNewPin())));
    });

    connect(typeid(CellularSimCardLockDataMessage), [&](sys::Message *request) -> sys::MessagePointer {
        auto msg = static_cast<CellularSimCardLockDataMessage *>(request);
        return std::make_shared<CellularResponseMessage>(
            setPinLock(msg->getLock() == CellularSimCardLockDataMessage::SimCardLock::Locked,
                       SimCard::pinToString(msg->getPin())));
    });

    connect(typeid(CellularChangeSimDataMessage), [&](sys::Message *request) -> sys::MessagePointer {
        auto msg                    = static_cast<CellularChangeSimDataMessage *>(request);
        Store::GSM::get()->selected = msg->getSim();
        bsp::cellular::sim::sim_sel();
        bsp::cellular::sim::hotswap_trigger();
        return std::make_shared<CellularResponseMessage>(true);
    });

    connect(typeid(CellularStartOperatorsScanMessage), [&](sys::Message *request) -> sys::MessagePointer {
        auto msg = static_cast<CellularStartOperatorsScanMessage *>(request);
        return handleCellularStartOperatorsScan(msg);
    });

    connect(typeid(CellularGetActiveContextsMessage), [&](sys::Message *request) -> sys::MessagePointer {
        auto msg = static_cast<CellularGetActiveContextsMessage *>(request);
        return handleCellularGetActiveContextsMessage(msg);
    });

    connect(typeid(CellularGetCurrentOperatorMessage), [&](sys::Message *request) -> sys::MessagePointer {
        auto msg = static_cast<CellularGetCurrentOperatorMessage *>(request);
        return handleCellularGetCurrentOperator(msg);
    });

    connect(typeid(CellularGetAPNMessage), [&](sys::Message *request) -> sys::MessagePointer {
        auto msg = static_cast<CellularGetAPNMessage *>(request);
        return handleCellularGetAPNMessage(msg);
    });

    connect(typeid(CellularSetAPNMessage), [&](sys::Message *request) -> sys::MessagePointer {
        auto msg = static_cast<CellularSetAPNMessage *>(request);
        return handleCellularSetAPNMessage(msg);
    });

    connect(typeid(CellularNewAPNMessage), [&](sys::Message *request) -> sys::MessagePointer {
        auto msg = static_cast<CellularNewAPNMessage *>(request);
        return handleCellularNewAPNMessage(msg);
    });

    connect(typeid(CellularSetDataTransferMessage), [&](sys::Message *request) -> sys::MessagePointer {
        auto msg = static_cast<CellularSetDataTransferMessage *>(request);
        return handleCellularSetDataTransferMessage(msg);
    });

    connect(typeid(CellularGetDataTransferMessage), [&](sys::Message *request) -> sys::MessagePointer {
        auto msg = static_cast<CellularGetDataTransferMessage *>(request);
        return handleCellularGetDataTransferMessage(msg);
    });

    connect(typeid(CellularActivateContextMessage), [&](sys::Message *request) -> sys::MessagePointer {
        auto msg = static_cast<CellularActivateContextMessage *>(request);
        return handleCellularActivateContextMessage(msg);
    });

    connect(typeid(CellularDeactivateContextMessage), [&](sys::Message *request) -> sys::MessagePointer {
        auto msg = static_cast<CellularDeactivateContextMessage *>(request);
        return handleCellularDeactivateContextMessage(msg);
    });

    connect(typeid(CellularGetActiveContextsMessage), [&](sys::Message *request) -> sys::MessagePointer {
        auto msg = static_cast<CellularGetActiveContextsMessage *>(request);
        return handleCellularGetActiveContextsMessage(msg);
    });

    connect(typeid(CellularChangeVoLTEDataMessage), [&](sys::Message *request) -> sys::MessagePointer {
        auto msg = static_cast<CellularChangeVoLTEDataMessage *>(request);
        volteOn  = msg->getVoLTEon();
        settings->setValue(settings::Cellular::volte_on, std::to_string(volteOn), settings::SettingsScope::Global);
        NetworkSettings networkSettings(*this);
        auto vstate = networkSettings.getVoLTEState();
        if ((vstate != VoLTEState::On) && volteOn) {
            LOG_DEBUG("VoLTE On");
            networkSettings.setVoLTEState(VoLTEState::On);
        }
        else if (!volteOn) {
            LOG_DEBUG("VoLTE Off");
            networkSettings.setVoLTEState(VoLTEState::Off);
        }
        return std::make_shared<CellularResponseMessage>(true);
    });

    connect(typeid(CellularPowerStateChange), [&](sys::Message *request) -> sys::MessagePointer {
        auto msg       = static_cast<CellularPowerStateChange *>(request);
        nextPowerState = msg->getNewState();
        handle_power_state_change();
        return sys::MessageNone{};
    });

    connect(typeid(CellularPowerStateChange), [&](sys::Message *request) -> sys::MessagePointer {
        auto msg       = static_cast<CellularPowerStateChange *>(request);
        nextPowerState = msg->getNewState();
        handle_power_state_change();
        return sys::MessageNone{};
    });

    connect(typeid(sdesktop::developerMode::DeveloperModeRequest), [&](sys::Message *request) -> sys::MessagePointer {
        auto msg = static_cast<sdesktop::developerMode::DeveloperModeRequest *>(request);
        if (typeid(*msg->event.get()) == typeid(sdesktop::developerMode::CellularHotStartEvent)) {
            cmux->CloseChannels();
            ///> change state - simulate hot start
            handle_power_up_request();
        }
        if (typeid(*msg->event.get()) == typeid(sdesktop::developerMode::CellularStateInfoRequestEvent)) {
            auto event   = std::make_unique<sdesktop::developerMode::CellularStateInfoRequestEvent>(state.c_str());
            auto message = std::make_shared<sdesktop::developerMode::DeveloperModeRequest>(std::move(event));
            bus.sendUnicast(std::move(message), service::name::service_desktop);
        }
        return sys::MessageNone{};
    });
    handle_CellularGetChannelMessage();
}

bool ServiceCellular::resetCellularModule(ResetType type)
{
    LOG_DEBUG("Cellular modem reset. Type %d", static_cast<int>(type));

    auto channel = cmux->get(TS0710::Channel::Commands);
    if (!channel) {
        LOG_ERROR("Bad channel");
        return false;
    }

    switch (type) {
    case ResetType::SoftReset:
        if (auto response = channel->cmd(at::AT::CFUN_RESET); response.code == at::Result::Code::OK) {
            return true;
        }
        LOG_ERROR("Cellular modem reset failed.");
        return false;
    case ResetType::PowerCycle:
        cmux->TurnOffModem();
        cmux->TurnOnModem();
        isAfterForceReboot = true;
        return true;
    case ResetType::HardReset:
        cmux->ResetModem();
        isAfterForceReboot = true;
        return true;
    }
    return false;
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
    case State::ST::CellularConfProcedure:
        handle_start_conf_procedure();
        break;
    case State::ST::APNConfProcedure:
        handle_apn_conf_procedure();
        break;
    case State::ST::SanityCheck:
        handle_sim_sanity_check();
        break;
    case State::ST::SimInit:
        handle_sim_init();
        break;
    case State::ST::SimSelect:
        handle_select_sim();
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
    auto msg = std::make_shared<CellularCheckIfStartAllowedMessage>();
    bus.sendUnicast(msg, service::name::system_manager);

    return true;
}

bool ServiceCellular::handle_power_up_request()
{
    cmux->SelectAntenna(bsp::cellular::antenna::lowBand);
    switch (board) {
    case bsp::Board::T4:
        state.set(this, State::ST::StatusCheck);
        break;
    case bsp::Board::T3:
        state.set(this, State::ST::PowerUpProcedure);
        break;
    case bsp::Board::Linux:
        state.set(this, State::ST::PowerUpProcedure);
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
    case bsp::Board::T4: {
        LOG_DEBUG("T4 - cold start");
        cmux->TurnOnModem();
        // wait for status pin change to change state
        break;
    }
    case bsp::Board::T3: {
        // check baud once to determine if it�s already turned on
        auto ret = cmux->BaudDetectOnce();
        if (ret == TS0710::ConfState::Success) {
            // it�s on aka hot start.
            LOG_DEBUG("T3 - hot start");
            state.set(this, State::ST::CellularConfProcedure);
            break;
        }
        else {
            // it�s off aka cold start
            LOG_DEBUG("T3 - cold start");
            cmux->TurnOnModem();
            // if it�s T3, then wait for status pin to become active, to align its starting position with T4
            vTaskDelay(pdMS_TO_TICKS(8000));
            state.set(this, State::ST::PowerUpInProgress);
            break;
        }
    }
    case bsp::Board::Linux: {
        // it is basically the same as T3
        // check baud once to determine if it�s already turned on
        auto ret = cmux->BaudDetectOnce();
        if (ret == TS0710::ConfState::Success) {
            // it�s on aka hot start.
            LOG_DEBUG("Linux - hot start");
            state.set(this, State::ST::CellularConfProcedure);
            break;
        }
        else {
            // it�s off aka cold start
            LOG_DEBUG("Linux - cold start");
            LOG_WARN("Press PWR_KEY for 2 sec on modem eval board!");
            vTaskDelay(pdMS_TO_TICKS(2000)); // give some 2 secs more for user input
            // if it�s Linux (T3), then wait for status pin to become active, to align its starting position with T4
            vTaskDelay(pdMS_TO_TICKS(8000));
            state.set(this, State::ST::PowerUpInProgress);
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
    if (isAfterForceReboot) {
        constexpr auto msModemUartInitTime = 12000;
        vTaskDelay(pdMS_TO_TICKS(msModemUartInitTime));
        isAfterForceReboot = false;
    }

    state.set(this, cellular::State::ST::BaudDetect);
    return true;
}

bool ServiceCellular::handle_baud_detect()
{
    auto ret = cmux->BaudDetectProcedure();
    if (ret == TS0710::ConfState::Success) {
        state.set(this, cellular::State::ST::CellularConfProcedure);
        return true;
    }
    else {
        state.set(this, cellular::State::ST::ModemFatalFailure);
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
    switch (board) {
    case bsp::Board::T4:
        // wait for pin status become inactive (handled elsewhere)
        break;
    case bsp::Board::Linux:
    case bsp::Board::T3:
        // if it�s T3, then wait for status pin to become inactive, to align with T4
        vTaskDelay(pdMS_TO_TICKS(17000)); // according to docs this shouldn�t be needed, but better be safe than Quectel
        state.set(this, cellular::State::ST::PowerDown);
        break;
    default:
        LOG_ERROR("Powering �down an unknown device not handled");
        return false;
    }
    return true;
}

bool ServiceCellular::handle_power_down()
{
    LOG_DEBUG("Powered Down");
    isAfterForceReboot = true;
    cmux.reset();
    cmux = std::make_unique<TS0710>(PortSpeed_e::PS460800, this);

    return true;
}

bool ServiceCellular::handle_start_conf_procedure()
{
    // Start configuration procedure, if it�s first run modem will be restarted
    auto confRet = cmux->ConfProcedure();
    if (confRet == TS0710::ConfState::Success) {
        state.set(this, State::ST::AudioConfigurationProcedure);
        return true;
    }
    state.set(this, State::ST::Failed);
    return false;
}

bool ServiceCellular::handle_audio_conf_procedure()
{
    auto audioRet = cmux->AudioConfProcedure();
    if (audioRet == TS0710::ConfState::Success) {
        auto cmd = at::factory(at::AT::IPR) + std::to_string(ATPortSpeeds_text[cmux->getStartParams().PortSpeed]);
        LOG_DEBUG("Setting baudrate %i baud", ATPortSpeeds_text[cmux->getStartParams().PortSpeed]);
        if (!cmux->getParser()->cmd(cmd)) {
            LOG_ERROR("Baudrate setup error");
            state.set(this, State::ST::Failed);
            return false;
        }
        cmux->getCellular()->SetSpeed(ATPortSpeeds_text[cmux->getStartParams().PortSpeed]);
        vTaskDelay(1000);

        if (cmux->StartMultiplexer() == TS0710::ConfState::Success) {

            LOG_DEBUG("[ServiceCellular] Modem is fully operational");

            // open channel - notifications
            DLC_channel *notificationsChannel = cmux->get(TS0710::Channel::Notifications);
            if (notificationsChannel != nullptr) {
                LOG_DEBUG("Setting up notifications callback");
                notificationsChannel->setCallback(notificationCallback);
            }
            state.set(this, State::ST::APNConfProcedure);
            return true;
        }
        else {
            state.set(this, State::ST::Failed);
            return false;
        }
    }
    else if (audioRet == TS0710::ConfState::Failure) {
        /// restart
        state.set(this, State::ST::AudioConfigurationProcedure);
        return true;
    }
    // Reset procedure started, do nothing here
    state.set(this, State::ST::Idle);
    return true;
}

auto ServiceCellular::handle(db::query::SMSSearchByTypeResult *response) -> bool
{
    if (response->getResults().size() > 0) {
        LOG_DEBUG("sending %ud last queued message(s)", static_cast<unsigned int>(response->getResults().size()));
        if (Store::GSM::get()->simCardInserted() == false) {
            auto message = std::make_shared<CellularSmsNoSimRequestMessage>();
            bus.sendUnicast(std::move(message), app::manager::ApplicationManager::ServiceName);
            auto records = response->getResults();

            for (auto &record : response->getResults()) {
                if (record.type == SMSType::QUEUED) {
                    record.type = SMSType::FAILED;
                    DBServiceAPI::GetQuery(
                        this, db::Interface::Name::SMS, std::make_unique<db::query::SMSUpdate>(std::move(record)));
                }
            }
            return true;
        }
        for (auto &rec : response->getResults()) {
            if (rec.type == SMSType::QUEUED) {
                sendSMS(rec);
            }
        }
        if (response->getMaxDepth() > response->getResults().size()) {
            LOG_WARN("There still is/are messages QUEUED for sending");
        }
    }
    return true;
}

sys::MessagePointer ServiceCellular::DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp)
{
    std::shared_ptr<sys::ResponseMessage> responseMsg;

    switch (static_cast<MessageType>(msgl->messageType)) {
    case MessageType::CellularStateRequest: {
        change_state(dynamic_cast<cellular::StateChange *>(msgl));
        responseMsg = std::make_shared<CellularResponseMessage>(true);
    } break;
    case MessageType::CellularCall: {
        auto *msg = dynamic_cast<CellularCallMessage *>(msgl);
        assert(msg != nullptr);

        switch (msg->type) {
        case CellularCallMessage::Type::Ringing: {
            auto ret    = ongoingCall.startCall(msg->number, CallType::CT_OUTGOING);
            responseMsg = std::make_shared<CellularResponseMessage>(ret);
            break;
        } break;
        case CellularCallMessage::Type::IncomingCall: {
            auto ret = true;
            if (!ongoingCall.isValid()) {
                ret = ongoingCall.startCall(msg->number, CallType::CT_INCOMING);
            }
            responseMsg = std::make_shared<CellularResponseMessage>(ret);
            break;
        }
        case CellularCallMessage::Type::CallerId: {
            ongoingCall.setNumber(msg->number);
            break;
        }
        }
    } break;
    // Incoming notifications from Notification Virtual Channel
    case MessageType::CellularNotification: {
        CellularNotificationMessage *msg = static_cast<CellularNotificationMessage *>(msgl);
        switch (msg->type) {
        case CellularNotificationMessage::Type::CallActive: {
            auto ret    = ongoingCall.setActive();
            responseMsg = std::make_shared<CellularResponseMessage>(ret);
            break;
        }
        case CellularNotificationMessage::Type::CallAborted: {
            callStateTimer->stop();
            auto ret    = ongoingCall.endCall();
            responseMsg = std::make_shared<CellularResponseMessage>(ret);
            break;
        }
        case CellularNotificationMessage::Type::PowerUpProcedureComplete: {
            if (board == bsp::Board::T3 || board == bsp::Board::Linux) {
                state.set(this, State::ST::CellularConfProcedure);
                responseMsg = std::make_shared<CellularResponseMessage>(true);
            }
            break;
        }
        case CellularNotificationMessage::Type::PowerDownDeregistering: {
            if (state.get() != State::ST::PowerDownWaiting) {
                state.set(this, State::ST::PowerDownStarted);
                responseMsg = std::make_shared<CellularResponseMessage>(true);
            }
            responseMsg = std::make_shared<CellularResponseMessage>(false);
            break;
        }
        case CellularNotificationMessage::Type::PowerDownDeregistered: {
            state.set(this, State::ST::PowerDownWaiting);
            responseMsg = std::make_shared<CellularResponseMessage>(true);
            break;
        }
        case CellularNotificationMessage::Type::NewIncomingSMS: {
            LOG_INFO("New incoming sms received");
            receiveSMS(msg->data);
            responseMsg = std::make_shared<CellularResponseMessage>(true);
            break;
        }
        case CellularNotificationMessage::Type::RawCommand: {
            auto m       = dynamic_cast<cellular::RawCommand *>(msgl);
            auto channel = cmux->get(TS0710::Channel::Commands);
            if (!m || !channel) {
                LOG_ERROR("RawCommand error: %s %s", m == nullptr ? "" : "bad cmd", !channel ? "no channel" : "");
                break;
            }
            auto respMsg      = std::make_shared<cellular::RawCommandResp>(true);
            auto ret          = channel->cmd(m->command, std::chrono::milliseconds(m->timeout));
            respMsg->response = ret.response;
            if (respMsg->response.size()) {
                for (auto const &el : respMsg->response) {
                    LOG_DEBUG("> %s", el.c_str());
                }
                responseMsg = std::make_shared<CellularResponseMessage>(false);
                break;
            }
            responseMsg = respMsg;
            break;
        } break;
        case CellularNotificationMessage::Type::SIM_READY:
            if (Store::GSM::get()->tray == Store::GSM::Tray::IN) {
                state.set(this, cellular::State::ST::SimInit);
                responseMsg = std::make_shared<CellularResponseMessage>(true);
                break;
            }
            break;
        case CellularNotificationMessage::Type::SMSDone: {
            auto resp   = handleAllMessagesFromMessageStorage();
            responseMsg = std::make_shared<CellularResponseMessage>(resp);
        } break;
        case CellularNotificationMessage::Type::SignalStrengthUpdate:
        case CellularNotificationMessage::Type::NetworkStatusUpdate:
        case CellularNotificationMessage::Type::SIM_NOT_READY: {
            // skipped
            responseMsg = std::make_shared<CellularResponseMessage>(false);
        }
        }
    } break;

    case MessageType::CellularSimProcedure: {
        state.set(this, State::ST::SimSelect);
        break;
    }
    case MessageType::CellularListCurrentCalls: {
        at::cmd::CLCC cmd;
        auto base = cmux->get(TS0710::Channel::Commands)->cmd(cmd);
        if (auto response = cmd.parse(base); response) {
            const auto &data = response.getData();
            auto it          = std::find_if(std::begin(data), std::end(data), [&](const auto &entry) {
                return entry.stateOfCall == ModemCall::CallState::Active && entry.mode == ModemCall::CallMode::Voice;
            });
            if (it != std::end(data)) {
                auto msg = std::make_shared<CellularNotificationMessage>(CellularNotificationMessage::Type::CallActive);
                bus.sendMulticast(std::move(msg), sys::BusChannel::ServiceCellularNotifications);
                callStateTimer->stop();
                responseMsg = std::make_shared<CellularResponseMessage>(true);
                break;
            }
        }
        responseMsg = std::make_shared<CellularResponseMessage>(false);
    } break;

    case MessageType::CellularHangupCall: {
        auto channel = cmux->get(TS0710::Channel::Commands);
        LOG_INFO("CellularHangupCall");
        if (channel) {
            if (channel->cmd(at::AT::ATH)) {
                AntennaServiceAPI::LockRequest(this, antenna::lockState::unlocked);
                callStateTimer->stop();
                if (!ongoingCall.endCall(CellularCall::Forced::True)) {
                    LOG_ERROR("Failed to end ongoing call");
                }
                responseMsg = std::make_shared<CellularResponseMessage>(true, msgl->messageType);
            }
            else {
                LOG_ERROR("Call not aborted");
                responseMsg = std::make_shared<CellularResponseMessage>(false, msgl->messageType);
            }
            break;
        }
        responseMsg = std::make_shared<CellularResponseMessage>(false, msgl->messageType);
    } break;

    case MessageType::CellularAnswerIncomingCall: {
        auto channel = cmux->get(TS0710::Channel::Commands);
        auto ret     = false;
        if (channel) {
            // TODO alek: check if your request isn�t for 5 sec when you wait in command for 90000, it�s exclusivelly
            // set to 5000ms in command requesting...
            auto response = channel->cmd(at::AT::ATA);
            if (response) {
                // Propagate "CallActive" notification into system
                bus.sendMulticast(
                    std::make_shared<CellularNotificationMessage>(CellularNotificationMessage::Type::CallActive),
                    sys::BusChannel::ServiceCellularNotifications);
                ret = true;
            }
        }
        responseMsg = std::make_shared<CellularResponseMessage>(ret);
    } break;

    case MessageType::CellularCallRequest: {

        auto *msg = dynamic_cast<CellularCallRequestMessage *>(msgl);
        assert(msg != nullptr);
        auto channel = cmux->get(TS0710::Channel::Commands);
        if (channel == nullptr) {
            responseMsg = std::make_shared<CellularResponseMessage>(false);
            break;
        }

        cellular::RequestFactory factory(msg->number.getEntered(),
                                         *channel,
                                         msg->requestMode,
                                         Store::GSM::get()->simCardInserted()
                                             ? RequestFactory::SimStatus::SimInsterted
                                             : RequestFactory::SimStatus::SimSlotEmpty);

        auto request = factory.create();

        CellularRequestHandler handler(*this);
        auto result = channel->cmd(request->command(), at::default_doc_timeout);
        request->handle(handler, result);
        responseMsg = std::make_shared<CellularResponseMessage>(request->isHandled());

    } break;
    case MessageType::DBServiceNotification: {
        auto msg = dynamic_cast<db::NotificationMessage *>(msgl);
        if (msg == nullptr) {
            responseMsg = std::make_shared<CellularResponseMessage>(false);
            break;
        }
        if (msg->interface == db::Interface::Name::SMS &&
            (msg->type == db::Query::Type::Create || msg->type == db::Query::Type::Update)) {
            // note: this gets triggered on every type update, e.g. on QUEUED ? FAILED so way too often

            // are there new messges queued for sending ?
            auto limitTo = 15; // how many to send in this Query
            DBServiceAPI::GetQuery(this,
                                   db::Interface::Name::SMS,
                                   std::make_unique<db::query::SMSSearchByType>(SMSType::QUEUED, 0, limitTo));

            return std::make_shared<sys::ResponseMessage>();
        }
        return std::make_shared<sys::ResponseMessage>(sys::ReturnCodes::Failure);
        break;
    }
    case MessageType::CellularGetIMSI: {

        std::string temp;
        if (getIMSI(temp)) {
            responseMsg = std::make_shared<CellularResponseMessage>(true, temp);
        }
        else {
            responseMsg = std::make_shared<CellularResponseMessage>(false);
        }
        break;
    }
    case MessageType::CellularGetOwnNumber: {
        std::string temp;
        if (getOwnNumber(temp)) {
            responseMsg = std::make_shared<CellularResponseMessage>(true, temp);
        }
        else {
            responseMsg = std::make_shared<CellularResponseMessage>(false);
        }
    } break;
    case MessageType::CellularGetNetworkInfo: {
        LOG_INFO("CellularGetNetworkInfo handled");
        responseMsg = std::make_shared<CellularResponseMessage>(true);

        auto msg  = std::make_shared<cellular::RawCommandRespAsync>(MessageType::CellularNetworkInfoResult);
        msg->data = getNetworkInfo();
        bus.sendUnicast(msg, msgl->sender);
    } break;
    case MessageType::CellularSelectAntenna: {
        auto msg = dynamic_cast<CellularAntennaRequestMessage *>(msgl);
        if (msg != nullptr) {

            cmux->SelectAntenna(msg->antenna);
            vTaskDelay(50); // sleep for 50 ms...
            auto actualAntenna  = cmux->GetAntenna();
            bool changedAntenna = (actualAntenna == msg->antenna);
            responseMsg         = std::make_shared<CellularResponseMessage>(changedAntenna);

            auto notification = std::make_shared<AntennaChangedMessage>();
            bus.sendMulticast(notification, sys::BusChannel::AntennaNotifications);
        }
        else {
            responseMsg = std::make_shared<CellularResponseMessage>(false);
        }
    } break;
    case MessageType::CellularSetScanMode: {
        auto msg = dynamic_cast<CellularRequestMessage *>(msgl);
        bool ret = false;
        if (msg != nullptr) {
            ret = SetScanMode(msg->data);
        }
        responseMsg = std::make_shared<CellularResponseMessage>(ret);
        break;
    }
    case MessageType::CellularGetScanMode: {

        std::string response;
        response = GetScanMode();

        if (response != "") {
            responseMsg = std::make_shared<CellularResponseMessage>(true);
            auto msg    = std::make_shared<cellular::RawCommandRespAsync>(MessageType::CellularGetScanModeResult);
            msg->data.push_back(response);
            bus.sendUnicast(msg, msgl->sender);
            break;
        }
        responseMsg = std::make_shared<CellularResponseMessage>(false);
        break;
    }
    case MessageType::CellularGetFirmwareVersion: {
        std::string response;
        auto channel = cmux->get(TS0710::Channel::Commands);
        if (channel) {
            auto resp = channel->cmd(at::AT::QGMR);
            if (resp.code == at::Result::Code::OK) {
                response    = resp.response[0];
                responseMsg = std::make_shared<CellularResponseMessage>(true, response);
            }
            else {
                responseMsg = std::make_shared<CellularResponseMessage>(false);
            }
        }
        break;
    }
    case MessageType::EVMModemStatus: {
        using namespace bsp::cellular::status;
        auto msg = dynamic_cast<sevm::StatusStateMessage *>(msgl);
        if (msg != nullptr) {
            if (board == bsp::Board::T4) {
                auto status_pin = msg->state;
                if (status_pin == value::ACTIVE) {
                    if (state.get() == State::ST::PowerUpProcedure) {
                        state.set(this, State::ST::PowerUpInProgress); // and go to baud detect as usual
                    }
                    else {
                        // asynchronous power toggle should fall back to PowerDown regardless the state
                        state.set(this, State::ST::PowerDown);
                    }
                }
                else if (status_pin == value::INACTIVE) {
                    if (isAfterForceReboot == true || state.get() == State::ST::PowerDownWaiting) {
                        state.set(this, State::ST::PowerDown);
                    }
                }
            }
        }
        break;
    }
    case MessageType::CellularGetCSQ: {
        auto channel = cmux->get(TS0710::Channel::Commands);
        if (channel) {
            auto modemResponse = channel->cmd(at::AT::CSQ);
            if (modemResponse.code == at::Result::Code::OK) {
                responseMsg = std::make_shared<CellularResponseMessage>(true, modemResponse.response[0]);
            }
            else {
                responseMsg = std::make_shared<CellularResponseMessage>(false);
            }
        }
    } break;
    case MessageType::CellularGetCREG: {
        auto channel = cmux->get(TS0710::Channel::Commands);
        if (channel) {
            auto resp = channel->cmd(at::AT::CREG);
            if (resp.code == at::Result::Code::OK) {
                responseMsg = std::make_shared<CellularResponseMessage>(true, resp.response[0]);
            }
            else {
                responseMsg = std::make_shared<CellularResponseMessage>(false);
            }
        }
    } break;
    case MessageType::CellularGetNWINFO: {
        auto channel = cmux->get(TS0710::Channel::Commands);
        if (channel) {
            auto resp = channel->cmd(at::AT::QNWINFO);
            if (resp.code == at::Result::Code::OK) {
                responseMsg = std::make_shared<CellularResponseMessage>(true, resp.response[0]);
            }
            else {
                responseMsg = std::make_shared<CellularResponseMessage>(false);
            }
        }
    } break;
    case MessageType::CellularGetAntenna: {
        auto antenna = cmux->GetAntenna();
        responseMsg  = std::make_shared<CellularAntennaResponseMessage>(true, antenna, MessageType::CellularGetAntenna);
    } break;
    case MessageType::CellularTransmitDtmfTones: {
        auto msg = static_cast<CellularDtmfRequestMessage *>(msgl);

        auto resp   = transmitDtmfTone(msg->getDigit());
        responseMsg = std::make_shared<CellularResponseMessage>(resp);
    } break;
    case MessageType::CellularSimState: {
        auto msg    = static_cast<CellularSimStateMessage *>(msgl);
        responseMsg = std::make_shared<CellularResponseMessage>(handleSimState(msg->getState(), msg->getMessage()));
    } break;
    case MessageType::CellularUSSDRequest: {
        auto msg = dynamic_cast<CellularUSSDMessage *>(msgl);
        if (msg != nullptr) {
            responseMsg = std::make_shared<CellularResponseMessage>(handleUSSDRequest(msg->type, msg->data));
        }
        break;
    }
    case MessageType::CellularSimResponse: {
        responseMsg = std::make_shared<CellularResponseMessage>(handleSimResponse(msgl));
    } break;
    default:
        break;

        if (responseMsg == nullptr) {
            LOG_DEBUG("message not handled: %d, %d", static_cast<int>(msgl->type), static_cast<int>(msgl->messageType));
            responseMsg = std::make_shared<CellularResponseMessage>(false);
        }
    }
    if (responseMsg != nullptr) {
        responseMsg->responseTo = msgl->messageType;
        return responseMsg;
    }

    // handle database response
    bool responseHandled = false;
    if (resp != nullptr) {
        if (auto msg = dynamic_cast<db::QueryResponse *>(resp)) {
            auto result = msg->getResult();
            if (auto response = dynamic_cast<db::query::SMSSearchByTypeResult *>(result.get())) {
                responseHandled = handle(response);
            }
            else if (result->hasListener()) {
                responseHandled = result->handle();
            }
        }
        if (responseHandled) {
            return std::make_shared<sys::ResponseMessage>();
        }
        else {
            return std::make_shared<sys::ResponseMessage>(sys::ReturnCodes::Unresolved);
        }
    }
    else {
        return std::make_shared<sys::ResponseMessage>();
    }
}

/**
 * NOTICE: URC handling function identifyNotification works on different thread, so sending
 * any AT commands is not allowed here (also in URC handlers and other functions called from here)
 * @return
 */
std::optional<std::shared_ptr<CellularMessage>> ServiceCellular::identifyNotification(const std::string &data)
{

    CellularUrcHandler urcHandler(*this);

    std::string str(data.begin(), data.end());

    std::string logStr = utils::removeNewLines(str);
    LOG_DEBUG("Notification:: %s", logStr.c_str());

    auto urc = at::urc::UrcFactory::Create(str);
    urc->Handle(urcHandler);

    if (!urc->isHandled()) {
        LOG_WARN("Unhandled notification: %s", logStr.c_str());
    }

    return urcHandler.getResponse();
}

bool ServiceCellular::requestPin(unsigned int attempts, const std::string msg)
{
    auto message = std::make_shared<CellularSimRequestPinMessage>(Store::GSM::get()->selected, attempts, msg);
    bus.sendUnicast(message, app::manager::ApplicationManager::ServiceName);
    LOG_DEBUG("REQUEST PIN");
    return true;
}

bool ServiceCellular::requestPuk(unsigned int attempts, const std::string msg)
{
    auto message = std::make_shared<CellularSimRequestPukMessage>(Store::GSM::get()->selected, attempts, msg);
    bus.sendUnicast(message, app::manager::ApplicationManager::ServiceName);
    LOG_DEBUG("REQUEST PUK");
    return true;
}

bool ServiceCellular::sendSimUnlocked()
{
    auto message = std::make_shared<CellularUnlockSimMessage>(Store::GSM::get()->selected);
    bus.sendUnicast(message, app::manager::ApplicationManager::ServiceName);
    LOG_DEBUG("SIM UNLOCKED");
    return true;
}

bool ServiceCellular::sendSimBlocked()
{
    auto message = std::make_shared<CellularBlockSimMessage>(Store::GSM::get()->selected);
    bus.sendUnicast(message, app::manager::ApplicationManager::ServiceName);
    LOG_ERROR("SIM BLOCKED");
    return true;
}

bool ServiceCellular::sendUnhandledCME(unsigned int cme_error)
{
    auto message = std::make_shared<CellularDisplayCMEMessage>(Store::GSM::get()->selected, cme_error);
    bus.sendUnicast(message, app::manager::ApplicationManager::ServiceName);
    LOG_ERROR("UNHANDLED CME %d", cme_error);
    return true;
}

bool ServiceCellular::sendBadPin()
{
    LOG_DEBUG("SEND BAD PIN");
    SimCard simCard(*this);
    std::string msg;
    if (auto state = simCard.simStateWithMessage(msg); state) {
        return handleSimState(*state, msg);
    }
    return false;
}

bool ServiceCellular::sendBadPuk()
{
    LOG_DEBUG("SEND BAD PUK");
    SimCard simCard(*this);
    std::string msg;
    if (auto state = simCard.simStateWithMessage(msg); state) {
        return handleSimState(*state, msg);
    }
    return false;
}

bool ServiceCellular::sendChangePinResult(SimCardResult res)
{
    LOG_DEBUG("SEND CHANGE PIN RESULT");
    return true;
}

bool ServiceCellular::changePin(const std::string oldPin, const std::string newPin)
{
    SimCard simCard(*this);
    auto result = simCard.changePin(oldPin, newPin);
    sendChangePinResult(result);
    return result == SimCardResult::OK;
}

bool ServiceCellular::setPinLock(bool lock, const std::string pin)
{
    SimCard simCard(*this);
    auto result = simCard.setPinLock(lock, pin);
    return result == SimCardResult::OK;
}

bool ServiceCellular::unlockSimPin(std::string pin)
{
    LOG_ERROR("Unlock pin %s", pin.c_str());
    SimCard simCard(*this);
    SimCardResult sime;
    sime = simCard.supplyPin(pin);

    if (sime == SimCardResult::IncorrectPassword) {
        sendBadPin();
        return false;
    }

    if (sime == SimCardResult::OK) {
        return true;
    }
    else {
        sendUnhandledCME(static_cast<unsigned int>(sime));
        return false;
    }
}

bool ServiceCellular::unlockSimPuk(std::string puk, std::string pin)
{
    SimCard simCard(*this);
    SimCardResult sime;
    LOG_DEBUG("PUK:  %s  %s", puk.c_str(), pin.c_str());
    sime = simCard.supplyPuk(puk, pin);

    if (sime == SimCardResult::IncorrectPassword) {
        sendBadPuk();
        return false;
    }

    if (sime == SimCardResult::OK) {
        return true;
    }
    sendUnhandledCME(static_cast<unsigned int>(sime));
    return false;
}

bool ServiceCellular::handleSimResponse(sys::DataMessage *msgl)
{

    auto msgSimPin = dynamic_cast<CellularSimPinDataMessage *>(msgl);
    if (msgSimPin != nullptr) {
        LOG_DEBUG("Unclocking sim");
        return unlockSimPin(SimCard::pinToString(msgSimPin->getPin()));
    }

    auto msgSimPuk = dynamic_cast<CellularSimPukDataMessage *>(msgl);
    if (msgSimPuk != nullptr) {
        LOG_DEBUG("Unlocking puk");
        return unlockSimPuk(SimCard::pinToString(msgSimPuk->getPuk()), SimCard::pinToString(msgSimPuk->getNewPin()));
    }
    return false;
}

bool ServiceCellular::handleSimState(at::SimState state, const std::string message)
{

    std::shared_ptr<CellularMessage> response;
    switch (state) {
    case at::SimState::Ready:
        Store::GSM::get()->sim = Store::GSM::get()->selected;
        settings->setValue(settings::SystemProperties::activeSim,
                           utils::enumToString(Store::GSM::get()->selected),
                           settings::SettingsScope::Global);
        // SIM causes SIM INIT, only on ready
        response =
            std::move(std::make_unique<CellularNotificationMessage>(CellularNotificationMessage::Type::SIM_READY));
        bus.sendMulticast(response, sys::BusChannel::ServiceCellularNotifications);
        sendSimUnlocked();
        break;
    case at::SimState::NotReady:
        LOG_DEBUG("Not ready");
        Store::GSM::get()->sim = Store::GSM::SIM::SIM_FAIL;
        response =
            std::move(std::make_unique<CellularNotificationMessage>(CellularNotificationMessage::Type::SIM_NOT_READY));
        bus.sendMulticast(response, sys::BusChannel::ServiceCellularNotifications);
        break;
    case at::SimState::SimPin: {
        SimCard simCard(*this);
        if (auto pc = simCard.getAttemptsCounters(); pc) {
            if (pc.value().PukCounter != 0) {
                requestPin(pc.value().PinCounter, message);
                break;
            }
        }
        sendSimBlocked();
        break;
    }
    case at::SimState::SimPuk: {
        SimCard simCard(*this);
        if (auto pc = simCard.getAttemptsCounters(); pc) {
            if (pc.value().PukCounter != 0) {
                requestPuk(pc.value().PukCounter, message);
                break;
            }
        }
        sendSimBlocked();
        break;
    }
    case at::SimState::SimPin2: {
        SimCard simCard(*this);
        if (auto pc = simCard.getAttemptsCounters(SimPinType::SimPin2); pc) {
            if (pc.value().PukCounter != 0) {
                requestPin(pc.value().PinCounter, message);
                break;
            }
        }
        sendSimBlocked();
        break;
    }
    case at::SimState::SimPuk2: {
        SimCard simCard(*this);
        if (auto pc = simCard.getAttemptsCounters(SimPinType::SimPin2); pc) {
            if (pc.value().PukCounter != 0) {
                requestPuk(pc.value().PukCounter, message);
                break;
            }
        }
        sendSimBlocked();
        break;
    }
    case at::SimState::PhNetPin:
        [[fallthrough]];
    case at::SimState::PhNetPuk:
        [[fallthrough]];
    case at::SimState::PhNetSPin:
        [[fallthrough]];
    case at::SimState::PhNetSPuk:
        [[fallthrough]];
    case at::SimState::PhSpPin:
        [[fallthrough]];
    case at::SimState::PhSpPuk:
        [[fallthrough]];
    case at::SimState::PhCorpPin:
        [[fallthrough]];
    case at::SimState::PhCorpPuk:
        Store::GSM::get()->sim = Store::GSM::SIM::SIM_UNKNOWN;
        LOG_ERROR("SimState not supported");
        break;
    case at::SimState::Locked:
        Store::GSM::get()->sim = Store::GSM::SIM::SIM_FAIL;
        sendSimBlocked();
        break;
    case at::SimState::Unknown:
        LOG_ERROR("SimState not supported");
        Store::GSM::get()->sim = Store::GSM::SIM::SIM_UNKNOWN;
        break;
    }
    auto simMessage = std::make_shared<sevm::SIMMessage>();
    bus.sendUnicast(simMessage, service::name::evt_manager);

    return true;
}

bool ServiceCellular::sendSMS(SMSRecord record)
{
    LOG_INFO("Trying to send SMS");

    uint32_t textLen = record.body.length();

    auto commandTimeout                 = at::factory(at::AT::CMGS).getTimeout();
    constexpr uint32_t singleMessageLen = msgConstants::singleMessageMaxLen;
    bool result                         = false;
    auto channel                        = cmux->get(TS0710::Channel::Commands);
    auto receiver                       = record.number.getEntered();
    if (channel) {
        channel->cmd(at::AT::SET_SMS_TEXT_MODE_UCS2);
        channel->cmd(at::AT::SMS_UCSC2);
        // if text fit in single message send
        if (textLen < singleMessageLen) {
            std::string command      = std::string(at::factory(at::AT::CMGS));
            std::string body         = UCS2(UTF8(receiver)).str();
            std::string suffix       = "\"";
            std::string command_data = command + body + suffix;
            if (cmux->CheckATCommandPrompt(
                    channel->SendCommandPrompt(command_data.c_str(), 1, commandTimeout.count()))) {

                if (channel->cmd((UCS2(record.body).str() + "\032").c_str(), commandTimeout)) {
                    result = true;
                }
                else {
                    result = false;
                }
                if (!result)
                    LOG_ERROR("Message to: %s send failure", receiver.c_str());
            }
        }
        // split text, and send concatenated messages
        else {
            const uint32_t maxConcatenatedCount = msgConstants::maxConcatenatedCount;
            uint32_t messagePartsCount          = textLen / singleMessageLen;
            if ((textLen % singleMessageLen) != 0) {
                messagePartsCount++;
            }

            if (messagePartsCount > maxConcatenatedCount) {
                LOG_ERROR("Message to long");
                result = false;
            }
            else {
                auto channel = cmux->get(TS0710::Channel::Commands);

                for (uint32_t i = 0; i < messagePartsCount; i++) {

                    uint32_t partLength = singleMessageLen;
                    if (i * singleMessageLen + singleMessageLen > record.body.length()) {
                        partLength = record.body.length() - i * singleMessageLen;
                    }
                    UTF8 messagePart = record.body.substr(i * singleMessageLen, partLength);

                    std::string command(at::factory(at::AT::QCMGS) + UCS2(UTF8(receiver)).str() + "\",120," +
                                        std::to_string(i + 1) + "," + std::to_string(messagePartsCount));

                    if (cmux->CheckATCommandPrompt(
                            channel->SendCommandPrompt(command.c_str(), 1, commandTimeout.count()))) {
                        // prompt sign received, send data ended by "Ctrl+Z"
                        if (channel->cmd(UCS2(messagePart).str() + "\032", commandTimeout, 2)) {
                            result = true;
                        }
                        else {
                            result = false;
                            LOG_ERROR("Message send failure");
                            break;
                        }
                    }
                    else {
                        result = false;
                        LOG_ERROR("Message send failure");
                        break;
                    }
                }
            }
        }
    }
    if (result) {
        LOG_INFO("SMS sent ok.");
        record.type = SMSType::OUTBOX;
    }
    else {
        LOG_INFO("SMS sending failed.");
        record.type = SMSType::FAILED;
        if (checkSmsCenter(*channel)) {
            LOG_ERROR("SMS center check");
        }
    }
    DBServiceAPI::GetQuery(this, db::Interface::Name::SMS, std::make_unique<db::query::SMSUpdate>(std::move(record)));

    channel->cmd(at::AT::SMS_GSM);
    return result;
}

bool ServiceCellular::receiveSMS(std::string messageNumber)
{
    auto channel = cmux->get(TS0710::Channel::Commands);
    if (channel == nullptr) {
        return false;
    }

    channel->cmd(at::AT::SMS_UCSC2);

    auto cmd           = at::factory(at::AT::QCMGR);
    auto ret           = channel->cmd(cmd + messageNumber, cmd.getTimeout());
    bool messageParsed = false;

    std::string messageRawBody;
    UTF8 receivedNumber;
    if (ret) {
        for (std::size_t i = 0; i < ret.response.size(); i++) {
            if (ret.response[i].find("QCMGR") != std::string::npos) {

                std::istringstream ss(ret.response[i]);
                std::string token;
                std::vector<std::string> tokens;
                while (std::getline(ss, token, ',')) {
                    tokens.push_back(token);
                }
                tokens[1].erase(std::remove(tokens[1].begin(), tokens[1].end(), '\"'), tokens[1].end());
                /*
                 * tokens:
                 * [0] - +QCMGR
                 * [1] - sender number
                 * [2] - none
                 * [3] - date YY/MM/DD
                 * [4] - hour HH/MM/SS/timezone
                 * concatenaded messages
                 * [5] - unique concatenated message id
                 * [6] - current message number
                 * [7] - total messages count
                 *
                 */
                // parse sender number
                receivedNumber = UCS2(tokens[1]).toUTF8();

                // parse date
                tokens[3].erase(std::remove(tokens[3].begin(), tokens[3].end(), '\"'), tokens[3].end());

                utils::time::Timestamp time;
                auto messageDate = time.getTime();

                // if its single message process
                if (tokens.size() == 5) {

                    messageRawBody = ret.response[i + 1];
                    messageParsed  = true;
                }
                // if its concatenated message wait for last message
                else if (tokens.size() == 8) {

                    uint32_t last    = 0;
                    uint32_t current = 0;
                    try {
                        last    = std::stoi(tokens[7]);
                        current = std::stoi(tokens[6]);
                    }
                    catch (const std::exception &e) {
                        LOG_ERROR("ServiceCellular::receiveSMS error %s", e.what());
                        return false;
                    }
                    if (current == last) {
                        messageParts.push_back(ret.response[i + 1]);

                        for (std::size_t j = 0; j < messageParts.size(); j++) {
                            messageRawBody += messageParts[j];
                        }
                        messageParts.clear();
                        messageParsed = true;
                    }
                    else {
                        messageParts.push_back(ret.response[i + 1]);
                    }
                }
                if (messageParsed) {
                    messageParsed = false;

                    const auto decodedMessage = UCS2(messageRawBody).toUTF8();

                    const auto record = createSMSRecord(decodedMessage, receivedNumber, messageDate);

                    if (!dbAddSMSRecord(record)) {
                        LOG_ERROR("Failed to add text message to db");
                        return false;
                    }
                }
            }
        }
    }
    channel->cmd(at::AT::SMS_GSM);
    // delete message from modem memory
    channel->cmd(at::factory(at::AT::CMGD) + messageNumber);
    return true;
}

bool ServiceCellular::getOwnNumber(std::string &destination)
{
    auto ret = cmux->get(TS0710::Channel::Commands)->cmd(at::AT::CNUM);

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
    auto ret = cmux->get(TS0710::Channel::Commands)->cmd(at::AT::CIMI);

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
std::vector<std::string> ServiceCellular::getNetworkInfo(void)
{
    std::vector<std::string> data;
    auto channel = cmux->get(TS0710::Channel::Commands);
    if (channel) {
        auto resp = channel->cmd(at::AT::CSQ);
        if (resp.code == at::Result::Code::OK) {
            data.push_back(resp.response[0]);
        }
        else {
            data.push_back("");
        }

        resp = channel->cmd(at::AT::CREG);
        if (resp.code == at::Result::Code::OK) {

            data.push_back(resp.response[0]);
        }
        else {
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
            data.push_back("");
        }
    }
    return data;
}

std::vector<std::string> get_last_AT_error(DLC_channel *channel)
{
    auto ret = channel->cmd(at::AT::CEER);
    return std::move(ret.response);
}

void log_last_AT_error(DLC_channel *channel)
{
    std::vector<std::string> atErrors(get_last_AT_error(channel));
    int i = 1;
    for (auto &msg_line : atErrors) {
        LOG_ERROR("%d/%d: %s", i, static_cast<int>(atErrors.size()), msg_line.c_str());
        i++;
    }
}

bool is_SIM_detection_enabled(DLC_channel *channel)
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

bool enable_SIM_detection(DLC_channel *channel)
{
    auto ret = channel->cmd(at::AT::SIM_DET_ON);
    if (!ret) {
        log_last_AT_error(channel);
        return false;
    }
    return true;
}

bool is_SIM_status_enabled(DLC_channel *channel)
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

bool enable_SIM_status(DLC_channel *channel)
{
    auto ret = channel->cmd(at::AT::SIMSTAT_ON);
    if (!ret) {
        log_last_AT_error(channel);
        return false;
    }
    return true;
}

void save_SIM_detection_status(DLC_channel *channel)
{
    auto ret = channel->cmd(at::AT::STORE_SETTINGS_ATW);
    if (!ret) {
        log_last_AT_error(channel);
    }
}

bool sim_check_hot_swap(DLC_channel *channel)
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
    auto ret = sim_check_hot_swap(cmux->get(TS0710::Channel::Commands));
    if (ret) {
        state.set(this, State::ST::ModemOn);
        bsp::cellular::sim::sim_sel();
    }
    else {
        LOG_ERROR("Sanity check failure - user will be promped about full shutdown");
        state.set(this, State::ST::ModemFatalFailure);
    }
    return ret;
}

bool ServiceCellular::handle_select_sim()
{

    bsp::cellular::sim::sim_sel();
    bsp::cellular::sim::hotswap_trigger();
#if defined(TARGET_Linux)
    DLC_channel *channel = cmux->get(TS0710::Channel::Commands);
    auto ret             = channel->cmd(at::AT::QSIMSTAT);
    if (!ret) {
        LOG_FATAL("Cant check sim stat status");
    }
    else {
        if (ret.response[0].find("+QSIMSTAT: 1,1") != std::string::npos) {
            // SIM IN - only sim1 mocup
            Store::GSM::get()->sim = Store::GSM::SIM::SIM1;
        }
        else {
            // NO SIM IN
            Store::GSM::get()->sim = Store::GSM::SIM::SIM_FAIL;
        }
        bus.sendUnicast(std::make_shared<sevm::SIMMessage>(), service::name::evt_manager);
        bool ready = false;
        while (!ready) {
            auto response = channel->cmd("AT+CPIN?");
            for (auto &line : response.response) {
                if (line.find("+CPIN: READY") == std::string::npos) {
                    ready = true;
                }
            }
        }
        state.set(this, cellular::State::ST::SimInit);
    }
#endif
    return true;
}

bool ServiceCellular::handle_modem_on()
{
    auto channel = cmux->get(TS0710::Channel::Commands);
    channel->cmd("AT+CCLK?");
    // inform host ap ready
    cmux->InformModemHostWakeup();
    state.set(this, State::ST::URCReady);
    LOG_DEBUG("AP ready");
    return true;
}

bool ServiceCellular::handle_URCReady()
{
    auto channel = cmux->get(TS0710::Channel::Commands);
    if (isSettingsAutomaticTimeSyncEnabled()) {
        channel->cmd(at::AT::ENABLE_TIME_ZONE_UPDATE);
        channel->cmd(at::AT::SET_TIME_ZONE_REPORTING);
    }
    channel->cmd(at::AT::ENABLE_NETWORK_REGISTRATION_URC);

    LOG_DEBUG("%s", state.c_str());
    return true;
}

bool ServiceCellular::handle_sim_init()
{
    auto channel = cmux->get(TS0710::Channel::Commands);
    if (channel == nullptr) {
        LOG_ERROR("Cant configure sim! no Commands channel!");
        state.set(this, State::ST::Failed);
        return false;
    }
    bool success  = true;
    auto commands = at::getCommadsSet(at::commadsSet::simInit);

    for (auto command : commands) {
        if (!channel->cmd(command)) {
            LOG_ERROR("SIM initialization failure!");
            return false;
        }
    }

    state.set(this, State::ST::Ready);
    return success;
}

bool ServiceCellular::handleAllMessagesFromMessageStorage()
{
    auto channel = cmux->get(TS0710::Channel::Commands);
    if (channel == nullptr) {
        LOG_ERROR("Cant configure sim! no Commands channel!");
        return false;
    }

    auto commands = at::getCommadsSet(at::commadsSet::smsInit);

    const auto errorState = []() {
        LOG_ERROR("HANDLE ALL MESSAGE FROM MESSAGE STORAGE FAILED!");
        return false;
    };

    for (const auto &command : commands) {
        if (command == at::AT::LIST_MESSAGES && !handleListMessages(command, channel)) {
            return errorState();
        }
        else if (!channel->cmd(command)) {
            return errorState();
        }
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
    return DBServiceAPI::AddSMS(this, record, db::QueryCallback::fromFunction([this](auto response) {
                                    auto result = dynamic_cast<db::query::SMSAddResult *>(response);
                                    if (result == nullptr || !result->result) {
                                        return false;
                                    }
                                    onSMSReceived();
                                    return true;
                                }));
}

void ServiceCellular::onSMSReceived()
{
    DBServiceAPI::GetQuery(this,
                           db::Interface::Name::Notifications,
                           std::make_unique<db::query::notifications::Increment>(NotificationsRecord::Key::Sms));
    const std::string ringtone_path = "assets/audio/SMS-drum2.mp3";
    AudioServiceAPI::PlaybackStart(this, audio::PlaybackType::TextMessageRingtone, ringtone_path);
}

bool ServiceCellular::handleListMessages(const at::AT &command, DLC_channel *channel)
{
    if (channel == nullptr) {
        return false;
    }
    constexpr std::string_view cmd = "CMGL: ";
    if (auto ret = channel->cmd(command)) {
        for (std::size_t i = 0; i < ret.response.size(); i++) {
            if (auto pos = ret.response[i].find(cmd); pos != std::string::npos) {
                auto startPos = pos + cmd.size();
                auto endPos   = ret.response[i].find_first_of(",");
                receiveSMS(ret.response[i].substr(startPos, endPos - startPos));
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
    state.set(this, State::ST::Idle);
    return true;
}

bool ServiceCellular::handle_fatal_failure()
{
    LOG_FATAL("Await for death!");
    while (true) {
        vTaskDelay(500);
    }
    return true;
}

bool ServiceCellular::handle_ready()
{
    LOG_DEBUG("%s", state.c_str());
    return true;
}

bool ServiceCellular::SetScanMode(std::string mode)
{
    auto channel = cmux->get(TS0710::Channel::Commands);
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
    auto channel = cmux->get(TS0710::Channel::Commands);
    if (channel) {

        auto resp = channel->cmd(at::AT::GET_SCANMODE);
        if (resp.code == at::Result::Code::OK) {
            auto beg = resp.response[0].find(",");
            if (beg != std::string::npos) {
                auto response = resp.response[0].substr(beg + 1, 1);
                return response;
            }
        }
    }
    return ("");
}

bool ServiceCellular::transmitDtmfTone(uint32_t digit)
{
    auto channel = cmux->get(TS0710::Channel::Commands);
    at::Result resp;
    if (channel) {
        auto command           = at::factory(at::AT::QLDTMF);
        std::string dtmfString = "\"" + std::string(1, digit) + "\"";
        resp                   = channel->cmd(command.getCmd() + dtmfString);
        if (resp) {
            command = at::factory(at::AT::VTS);
            resp    = channel->cmd(command.getCmd() + dtmfString);
        }
    }
    return resp.code == at::Result::Code::OK;
}

void ServiceCellular::handle_CellularGetChannelMessage()
{
    connect(CellularGetChannelMessage(), [&](sys::Message *req) {
        auto getChannelMsg = static_cast<CellularGetChannelMessage *>(req);
        LOG_DEBUG("Handle request for channel: %s", TS0710::name(getChannelMsg->dataChannel).c_str());
        std::shared_ptr<CellularGetChannelResponseMessage> channelResponsMessage =
            std::make_shared<CellularGetChannelResponseMessage>(cmux->get(getChannelMsg->dataChannel));
        LOG_DEBUG("channel ptr: %p", channelResponsMessage->dataChannelPtr);
        bus.sendUnicast(std::move(channelResponsMessage), req->sender);
        return sys::MessageNone{};
    });
}
bool ServiceCellular::handle_status_check(void)
{
    LOG_INFO("Checking modem status.");
    auto modemActive = cmux->IsModemActive();
    if (modemActive) {
        // modem is already turned on, call configutarion procedure
        LOG_INFO("Modem is already turned on.");
        LOG_DEBUG("T4 - hot start");
        state.set(this, cellular::State::ST::PowerUpInProgress);
    }
    else {
        state.set(this, cellular::State::ST::PowerUpProcedure);
    }
    return true;
}

void ServiceCellular::startStateTimer(uint32_t timeout)
{
    stateTimeout = timeout;
    stateTimer->reload();
}

void ServiceCellular::stopStateTimer()
{
    stateTimeout = 0;
    stateTimer->stop();
}

void ServiceCellular::handleStateTimer(void)
{
    stateTimeout--;
    if (stateTimeout == 0) {
        stopStateTimer();
        LOG_FATAL("State %s timeout occured!", state.c_str(state.get()));
        state.set(this, cellular::State::ST::ModemFatalFailure);
    }
}

void ServiceCellular::handle_power_state_change()
{
    nextPowerStateChangeAwaiting = false;
    auto modemActive             = cmux->IsModemActive();

    if (nextPowerState == State::PowerState::On) {
        if (state.get() == State::ST::PowerDownWaiting) {
            LOG_DEBUG("Powerdown in progress. Powerup request queued.");
            nextPowerStateChangeAwaiting = true;
        }
        else if (state.get() == State::ST::PowerUpProcedure || state.get() == State::ST::PowerUpInProgress) {
            LOG_DEBUG("Powerup already in progress");
        }
        else if (state.get() == State::ST::PowerDown || state.get() == State::ST::WaitForStartPermission) {
            LOG_INFO("Modem Power UP.");
            state.set(this, State::ST::PowerUpRequest);
        }
        else {
            LOG_DEBUG("Modem already powered up.");
        }
    }
    else {
        if (state.get() == State::ST::PowerUpProcedure || state.get() == State::ST::PowerUpInProgress) {
            LOG_DEBUG("Powerup in progress. Powerdown request queued.");
            nextPowerStateChangeAwaiting = true;
        }
        else if (state.get() == State::ST::PowerDownWaiting) {
            LOG_DEBUG("Powerdown already in progress.");
        }
        else if (state.get() == State::ST::PowerDown) {
            LOG_DEBUG("Modem already powered down.");
        }
        else if (state.get() == State::ST::WaitForStartPermission && !modemActive) {
            LOG_DEBUG("Modem already powered down.");
            state.set(this, State::ST::PowerDown);
        }
        else {
            LOG_INFO("Modem Power DOWN.");
            cmux->TurnOffModem();
            state.set(this, State::ST::PowerDownWaiting);
        }
    }
}

bool ServiceCellular::handleUSSDRequest(CellularUSSDMessage::RequestType requestType, const std::string &request)
{
    constexpr uint32_t commandTimeout        = 120000;
    constexpr uint32_t commandExpectedTokens = 2;

    auto channel = cmux->get(TS0710::Channel::Commands);
    if (channel != nullptr) {
        if (requestType == CellularUSSDMessage::RequestType::pullSesionRequest) {
            channel->cmd(at::AT::SMS_GSM);
            std::string command = at::factory(at::AT::CUSD_SEND) + request + ",15";
            auto result = channel->cmd(command, std::chrono::milliseconds(commandTimeout), commandExpectedTokens);
            if (result.code == at::Result::Code::OK) {
                ussdState = ussd::State::pullRequestSent;
                setUSSDTimer();
            }
        }
        else if (requestType == CellularUSSDMessage::RequestType::abortSesion) {

            ussdState   = ussd::State::sesionAborted;
            auto result = channel->cmd(at::AT::CUSD_CLOSE_SESSION);
            if (result.code == at::Result::Code::OK) {
                CellularServiceAPI::USSDRequest(this, CellularUSSDMessage::RequestType::pushSesionRequest);
            }
            else {
                CellularServiceAPI::USSDRequest(this, CellularUSSDMessage::RequestType::abortSesion);
            }
        }
        else if (requestType == CellularUSSDMessage::RequestType::pushSesionRequest) {

            ussdState   = ussd::State::pushSesion;
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
        LOG_WARN("USSD timeout occured, abotrig current session");
        ussdTimer->stop();
        CellularServiceAPI::USSDRequest(this, CellularUSSDMessage::RequestType::abortSesion);
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
    case ussd::State::pushSesion:
    case ussd::State::sesionAborted:
    case ussd::State::none:
        ussdTimeout = ussd::noTimeout;
        break;
    }
    if (ussdTimeout == ussd::noTimeout) {
        ussdTimer->stop();
        return;
    }
    ussdTimer->reload();
}

std::shared_ptr<cellular::RawCommandRespAsync> ServiceCellular::handleCellularStartOperatorsScan(
    CellularStartOperatorsScanMessage *msg)
{
    LOG_INFO("CellularStartOperatorsScan handled");
    auto ret = std::make_shared<cellular::RawCommandRespAsync>(MessageType::CellularOperatorsScanResult);
    NetworkSettings networkSettings(*this);
    ret->data = networkSettings.scanOperators(msg->getFullInfo());
    bus.sendUnicast(ret, msg->sender);
    return ret;
}

bool ServiceCellular::handle_apn_conf_procedure()
{
    LOG_DEBUG("APN on modem configuration");
    packetData->setupAPNSettings();
    state.set(this, State::ST::SanityCheck);
    return true;
}

std::shared_ptr<CellularGetCurrentOperatorResponse> ServiceCellular::handleCellularGetCurrentOperator(
    CellularGetCurrentOperatorMessage *msg)
{
    LOG_INFO("CellularGetCurrentOperator handled");
    NetworkSettings networkSettings(*this);
    return std::make_shared<CellularGetCurrentOperatorResponse>(networkSettings.getCurrentOperator());
}

std::shared_ptr<CellularGetAPNResponse> ServiceCellular::handleCellularGetAPNMessage(CellularGetAPNMessage *msg)
{
    std::vector<std::shared_ptr<packet_data::APN::Config>> apns;

    if (auto type = msg->getAPNType(); type) {
        if (auto apn = packetData->getAPNFirst(*type); apn) {
            apns.push_back(*apn);
        }
        return std::make_shared<CellularGetAPNResponse>(apns);
    }

    if (auto ctxid = msg->getContextId(); ctxid) {
        if (auto apn = packetData->getAPN(*ctxid); apn) {
            apns.push_back(*apn);
        }
        return std::make_shared<CellularGetAPNResponse>(apns);
    }

    return std::make_shared<CellularGetAPNResponse>(packetData->getAPNs());
}

std::shared_ptr<CellularSetAPNResponse> ServiceCellular::handleCellularSetAPNMessage(CellularSetAPNMessage *msg)
{
    auto apn = msg->getAPNConfig();
    auto ret = packetData->setAPN(apn);
    settings->setValue(settings::Cellular::apn_list, packetData->saveAPNSettings(), settings::SettingsScope::Global);
    return std::make_shared<CellularSetAPNResponse>(ret);
}
std::shared_ptr<CellularNewAPNResponse> ServiceCellular::handleCellularNewAPNMessage(CellularNewAPNMessage *msg)
{
    auto apn           = msg->getAPNConfig();
    std::uint8_t newId = 0;
    auto ret           = packetData->newAPN(apn, newId);
    settings->setValue(settings::Cellular::apn_list, packetData->saveAPNSettings(), settings::SettingsScope::Global);
    return std::make_shared<CellularNewAPNResponse>(ret, newId);
}

std::shared_ptr<CellularSetDataTransferResponse> ServiceCellular::handleCellularSetDataTransferMessage(
    CellularSetDataTransferMessage *msg)
{
    packetData->setDataTransfer(msg->getDataTransfer());
    return std::make_shared<CellularSetDataTransferResponse>(at::Result::Code::OK);
}

std::shared_ptr<CellularGetDataTransferResponse> ServiceCellular::handleCellularGetDataTransferMessage(
    CellularGetDataTransferMessage *msg)
{
    return std::make_shared<CellularGetDataTransferResponse>(packetData->getDataTransfer());
}

std::shared_ptr<CellularActivateContextResponse> ServiceCellular::handleCellularActivateContextMessage(
    CellularActivateContextMessage *msg)
{
    return std::make_shared<CellularActivateContextResponse>(packetData->activateContext(msg->getContextId()),
                                                             msg->getContextId());
}

std::shared_ptr<CellularDeactivateContextResponse> ServiceCellular::handleCellularDeactivateContextMessage(
    CellularDeactivateContextMessage *msg)
{
    return std::make_shared<CellularDeactivateContextResponse>(packetData->deactivateContext(msg->getContextId()),
                                                               msg->getContextId());
}

std::shared_ptr<CellularGetActiveContextsResponse> ServiceCellular::handleCellularGetActiveContextsMessage(
    CellularGetActiveContextsMessage *msg)
{
    return std::make_shared<CellularGetActiveContextsResponse>(packetData->getActiveContexts());
}

std::shared_ptr<CellularSetOperatorAutoSelectResponse> ServiceCellular::handleCellularSetOperatorAutoSelect(
    CellularSetOperatorAutoSelectMessage *msg)
{
    LOG_INFO("CellularSetOperatorAutoSelect handled");

    NetworkSettings networkSettings(*this);
    return std::make_shared<CellularSetOperatorAutoSelectResponse>(networkSettings.setOperatorAutoSelect());
}

std::shared_ptr<CellularSetOperatorResponse> ServiceCellular::handleCellularSetOperator(CellularSetOperatorMessage *msg)
{
    LOG_INFO("CellularSetOperatorAutoSelect handled");

    NetworkSettings networkSettings(*this);
    return std::make_shared<CellularSetOperatorResponse>(
        networkSettings.setOperator(msg->getMode(), msg->getFormat(), msg->getName()));
}

void ServiceCellular::volteChanged(const std::string &value)
{
    if (!value.empty()) {
        volteOn = utils::getNumericValue<bool>(value);
    }
}

void ServiceCellular::apnListChanged(const std::string &value)
{
    LOG_ERROR("apnListChanged");
    if (!value.empty()) {
        packetData->loadAPNSettings(value);
    }
}
