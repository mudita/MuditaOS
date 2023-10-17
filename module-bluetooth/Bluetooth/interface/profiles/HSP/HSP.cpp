// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <log/log.hpp>
#include "HSPImpl.hpp"
#include "HSP.hpp"

#include <Bluetooth/Result.hpp>
#include <service-evtmgr/ServiceEventManagerName.hpp>
#include <BluetoothWorker.hpp>
#include <module-bluetooth/Bluetooth/interface/profiles/SCO/ScoUtils.hpp>
#include <service-audio/AudioMessage.hpp>
#include <service-bluetooth/ServiceBluetoothName.hpp>
#include <service-bluetooth/messages/AudioVolume.hpp>
#include <service-bluetooth/messages/Connect.hpp>
#include <service-bluetooth/messages/Disconnect.hpp>

extern "C"
{
#include "btstack.h"
#include "btstack_run_loop_freertos.h"
#include <btstack_defines.h>
}

namespace bluetooth
{
    HSP::HSP() : pimpl(std::make_unique<HSPImpl>(HSPImpl()))
    {}

    HSP::HSP(HSP &&other) noexcept : pimpl(std::move(other.pimpl))
    {}

    auto HSP::operator=(HSP &&other) noexcept -> HSP &
    {
        if (&other == this) {
            return *this;
        }
        pimpl = std::move(other.pimpl);
        return *this;
    }

    auto HSP::init() -> Result::Code
    {
        return pimpl->init();
    }

    void HSP::setDevice(const Devicei &device)
    {
        pimpl->setDevice(device);
    }

    void HSP::setOwnerService(sys::Service *service)
    {
        pimpl->setOwnerService(service);
    }

    void HSP::connect()
    {
        pimpl->connect();
    }

    void HSP::disconnect()
    {
        pimpl->disconnect();
    }

    void HSP::setAudioDevice(std::shared_ptr<bluetooth::BluetoothAudioDevice> audioDevice)
    {
        pimpl->setAudioDevice(audioDevice);
    }

    auto HSP::setIncomingCallNumber([[maybe_unused]] const std::string &num) const noexcept -> Result::Code
    {
        return Result::Code::Success;
    }

    auto HSP::setSignalStrength([[maybe_unused]] int bars) const noexcept -> Result::Code
    {
        return Result::Code::Success;
    }

    auto HSP::setOperatorName([[maybe_unused]] const std::string_view &name) const noexcept -> Result::Code
    {
        return Result::Code::Success;
    }

    auto HSP::setBatteryLevel([[maybe_unused]] const BatteryLevel &name) const noexcept -> Result::Code
    {
        return Result::Code::Success;
    }

    auto HSP::incomingCallStarted() const noexcept -> Result::Code
    {
        return pimpl->incomingCallStarted();
    }

    auto HSP::outgoingCallStarted(const std::string &number) const noexcept -> Result::Code
    {
        return pimpl->outgoingCallStarted(number);
    }

    auto HSP::incomingCallAnswered() const noexcept -> Result::Code
    {
        return pimpl->incomingCallAnswered();
    }

    auto HSP::outgoingCallAnswered() const noexcept -> Result::Code
    {
        return pimpl->outgoingCallAnswered();
    }

    auto HSP::callTerminated() const noexcept -> Result::Code
    {
        return pimpl->callTerminated();
    }

    auto HSP::callMissed() const noexcept -> Result::Code
    {
        return pimpl->callMissed();
    }

    auto HSP::setNetworkRegistrationStatus([[maybe_unused]] bool registered) const noexcept -> Result::Code
    {
        return Result::Code::Success;
    }

    auto HSP::setRoamingStatus([[maybe_unused]] bool enabled) const noexcept -> Result::Code
    {
        return Result::Code::Success;
    }

    HSP::~HSP() = default;

    std::uint16_t HSP::HSPImpl::scoHandle = HCI_CON_HANDLE_INVALID;
    std::uint8_t HSP::HSPImpl::serviceBuffer[serviceBufferSize];

    const char *HSP::HSPImpl::agServiceName = "Mudita Pure HSP";

    std::unique_ptr<SCO> HSP::HSPImpl::sco;
    std::shared_ptr<CVSDAudioDevice> HSP::HSPImpl::audioDevice;

    std::unique_ptr<CellularInterface> HSP::HSPImpl::cellularInterface = nullptr;
    std::unique_ptr<AudioInterface> HSP::HSPImpl::audioInterface       = nullptr;

    sys::Service *HSP::HSPImpl::ownerService;
    Devicei HSP::HSPImpl::device;

    btstack_packet_callback_registration_t HSP::HSPImpl::hciEventCallbackRegistration;
    char HSP::HSPImpl::ATCommandBuffer[commandBufferSize];
    HSP::HSPImpl::HSPState HSP::HSPImpl::state = HSPState::RfcommDisconnected;

    void HSP::HSPImpl::sendAudioEvent(audio::EventType event, audio::Event::DeviceState deviceState)
    {
        auto evt       = std::make_shared<audio::Event>(event, deviceState);
        auto msg       = std::make_shared<AudioEventRequest>(std::move(evt));
        ownerService->bus.sendUnicast(std::move(msg), service::name::evt_manager);
    }

    void HSP::HSPImpl::packetHandler(std::uint8_t packetType,
                                     [[maybe_unused]] std::uint16_t channel,
                                     std::uint8_t *event,
                                     std::uint16_t eventSize)
    {
        switch (packetType) {
        case HCI_SCO_DATA_PACKET:
            if (READ_SCO_CONNECTION_HANDLE(event) != scoHandle) {
                break;
            }
            if (audioDevice != nullptr) {
                audioDevice->receiveCVSD(audio::AbstractStream::Span{.data = event, .dataSize = eventSize});
            }
            break;

        case HCI_EVENT_PACKET:
            processHCIEvent(event);
            break;

        default:
            break;
        }
    }

    void HSP::HSPImpl::processHCIEvent(std::uint8_t *event)
    {
        switch (hci_event_packet_get_type(event)) {
        case HCI_EVENT_SCO_CAN_SEND_NOW:
            if (audioDevice != nullptr) {
                audioDevice->onDataSend(scoHandle);
            }
            else {
                sco::utils::sendZeros(scoHandle);
            }
            break;

        case HCI_EVENT_HSP_META:
            processHSPEvent(event);
            break;

        default:
            break;
        }
    }

    void HSP::HSPImpl::processHSPEvent(std::uint8_t *event)
    {
        switch (hci_event_hsp_meta_get_subevent_code(event)) {
        case HSP_SUBEVENT_RFCOMM_CONNECTION_COMPLETE: {
            if (hsp_subevent_rfcomm_connection_complete_get_status(event) != ERROR_CODE_SUCCESS) {
                LOG_ERROR("RFCOMM connection failed, status: 0x%02X",
                          hsp_subevent_rfcomm_connection_complete_get_status(event));
                sendAudioEvent(audio::EventType::BluetoothHSPDeviceState, audio::Event::DeviceState::Disconnected);
                state = HSPState::RfcommDisconnected;
                break;
            }

            LOG_INFO("RFCOMM connection established");
            sendAudioEvent(audio::EventType::BluetoothHSPDeviceState, audio::Event::DeviceState::Connected);
            state              = HSPState::RfcommConnected;
            device.deviceState = DeviceState::ConnectedVoice;

            using message::bluetooth::ConnectResult;
            ownerService->bus.sendUnicast(std::make_shared<ConnectResult>(device, ConnectResult::Result::Success),
                                          service::name::bluetooth);
        } break;

        case HSP_SUBEVENT_RFCOMM_DISCONNECTION_COMPLETE: {
            LOG_INFO("RFCOMM disconnected");
            sendAudioEvent(audio::EventType::BluetoothHSPDeviceState, audio::Event::DeviceState::Disconnected);
            state = HSPState::RfcommDisconnected;
            ownerService->bus.sendUnicast(std::make_shared<message::bluetooth::DisconnectResult>(device),
                                          service::name::bluetooth);
        } break;

        case HSP_SUBEVENT_AUDIO_CONNECTION_COMPLETE: {
            const auto status = hsp_subevent_audio_connection_complete_get_status(event);
            if (status != ERROR_CODE_SUCCESS) {
                LOG_ERROR("Audio connection establishment failed, status: 0x%02X", status);
                state = HSPState::RfcommDisconnected;
                audioDevice.reset();
                break;
            }

            scoHandle = hsp_subevent_audio_connection_complete_get_sco_handle(event);
            LOG_INFO("Audio connection established with SCO handle 0x%04X", scoHandle);
            state = HSPState::Answered;

            audioInterface->startAudioRouting(ownerService);
            hci_request_sco_can_send_now_event();
        } break;

        case HSP_SUBEVENT_AUDIO_DISCONNECTION_COMPLETE: {
            scoHandle = HCI_CON_HANDLE_INVALID;
            audioDevice.reset();
            state = HSPState::RfcommConnected;
            LOG_INFO("Audio disconnected");
        } break;

        case HSP_SUBEVENT_MICROPHONE_GAIN_CHANGED: {
            LOG_INFO("Received microphone gain change: %d", hsp_subevent_microphone_gain_changed_get_gain(event));
        } break;

        case HSP_SUBEVENT_SPEAKER_GAIN_CHANGED: {
            const auto volume = hsp_subevent_speaker_gain_changed_get_gain(event);
            LOG_INFO("Received speaker gain change: %d", hsp_subevent_speaker_gain_changed_get_gain(event));
            ownerService->bus.sendUnicast(std::make_shared<message::bluetooth::HSPVolume>(volume),
                                          service::name::bluetooth);
        } break;

        case HSP_SUBEVENT_HS_CALL_ANSWER:
            LOG_INFO("HSP call answer");
            cellularInterface->answerIncomingCall(ownerService);
            break;

        case HSP_SUBEVENT_HS_CALL_HANGUP:
            LOG_INFO("HSP call hangup");
            cellularInterface->hangupCall(ownerService);
            break;

        case HSP_SUBEVENT_HS_COMMAND: {
            const std::size_t cmdLength = hsp_subevent_hs_command_get_value_length(event);
            const auto size             = std::min(cmdLength, sizeof(ATCommandBuffer));
            const auto commandValue     = hsp_subevent_hs_command_get_value(event);

            std::memset(ATCommandBuffer, 0, sizeof(ATCommandBuffer));
            std::memcpy(ATCommandBuffer, commandValue, size - 1);

            LOG_INFO("Received custom command: '%s'", ATCommandBuffer);
            break;
        }

        case HSP_SUBEVENT_BUTTON_PRESSED: {
            if ((scoHandle == HCI_CON_HANDLE_INVALID) && (state == HSPState::Ringing)) {
                LOG_INFO("Received button event in ringing state, answering call");
                cellularInterface->answerIncomingCall(ownerService);
                break;
            }

            if (state == HSPState::Answered) {
                LOG_INFO("Received button event in answered state, hanging up call");
                cellularInterface->hangupCall(ownerService);
            }
        } break;

        default:
            LOG_INFO("Event not handled: 0x%02X", hci_event_hsp_meta_get_subevent_code(event));
            break;
        }
    }

    auto HSP::HSPImpl::init() -> Result::Code
    {
        sco = std::make_unique<SCO>();
        sco->setOwnerService(ownerService);
        sco->init();

        cellularInterface = std::make_unique<CellularInterfaceImpl>();
        audioInterface    = std::make_unique<AudioInterfaceImpl>();

        Profile::initL2cap();
        Profile::initSdp();

        std::memset(serviceBuffer, 0, sizeof(serviceBuffer));

        hsp_ag_create_sdp_record(serviceBuffer, hspSdpRecordHandle, rfcommChannelNr, agServiceName);

        if (const auto status = sdp_register_service(serviceBuffer); status != ERROR_CODE_SUCCESS) {
            LOG_ERROR("Can't register service, status: 0x%02X", status);
        }

        rfcomm_init();

        hsp_ag_init(rfcommChannelNr);

        hciEventCallbackRegistration.callback = &packetHandler;
        hci_add_event_handler(&hciEventCallbackRegistration);
        hsp_ag_register_packet_handler(&packetHandler);
        hci_register_sco_packet_handler(&packetHandler);

        LOG_INFO("HSP initialized!");
        return bluetooth::Result::Code::Success;
    }

    void HSP::HSPImpl::connect()
    {
        if (state != HSPState::RfcommDisconnected) {
            disconnect();
        }
        hsp_ag_connect(device.address);
    }

    void HSP::HSPImpl::disconnect()
    {
        hsp_ag_release_audio_connection();
        hsp_ag_disconnect();
        ownerService->bus.sendUnicast(std::make_shared<message::bluetooth::DisconnectResult>(device),
                                      service::name::bluetooth);
    }

    void HSP::HSPImpl::setDevice(const Devicei &dev)
    {
        device = dev;
        LOG_DEBUG("Device set!");
    }

    void HSP::HSPImpl::setAudioDevice(std::shared_ptr<bluetooth::BluetoothAudioDevice> audioDev)
    {
        HSP::HSPImpl::audioDevice = std::static_pointer_cast<CVSDAudioDevice>(audioDev);
    }

    void HSP::HSPImpl::setOwnerService(sys::Service *service)
    {
        ownerService = service;
    }

    auto HSP::HSPImpl::incomingCallStarted() const noexcept -> Result::Code
    {
        LOG_DEBUG("Incoming call started");
        hsp_ag_start_ringing();
        state = HSPState::Ringing;
        return Result::Code::Success;
    }

    auto HSP::HSPImpl::outgoingCallStarted([[maybe_unused]] const std::string &number) const noexcept -> Result::Code
    {
        LOG_DEBUG("Outgoing call started");
        hsp_ag_establish_audio_connection();
        return Result::Code::Success;
    }

    auto HSP::HSPImpl::incomingCallAnswered() const noexcept -> Result::Code
    {
        LOG_DEBUG("Incoming call answered");
        hsp_ag_stop_ringing();
        hsp_ag_establish_audio_connection();
        return Result::Code::Success;
    }

    auto HSP::HSPImpl::outgoingCallAnswered() const noexcept -> Result::Code
    {
        LOG_DEBUG("Outgoing call answered");
        return Result::Code::Success;
    }

    auto HSP::HSPImpl::callTerminated() const noexcept -> Result::Code
    {
        LOG_DEBUG("Call terminated");
        hsp_ag_stop_ringing();
        hsp_ag_release_audio_connection();
        state = HSPState::RfcommConnected;
        return Result::Code::Success;
    }

    auto HSP::HSPImpl::callMissed() const noexcept -> Result::Code
    {
        return callTerminated();
    }
} // namespace bluetooth
