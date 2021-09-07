// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <log/log.hpp>
#include "HSPImpl.hpp"
#include "HSP.hpp"

#include <Bluetooth/Error.hpp>
#include <service-evtmgr/Constants.hpp>
#include <BluetoothWorker.hpp>
#include <module-bluetooth/Bluetooth/interface/profiles/SCO/ScoUtils.hpp>
#include <service-audio/AudioMessage.hpp>
#include <service-bluetooth/Constants.hpp>
#include <service-bluetooth/messages/AudioVolume.hpp>
#include <service-cellular/service-cellular/CellularServiceAPI.hpp>
#include <service-evtmgr/Constants.hpp>
#include <service-audio/AudioServiceAPI.hpp>

extern "C"
{
#include "btstack.h"
#include "btstack_run_loop_freertos.h"
#include "btstack_stdin.h"
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

    auto HSP::init() -> Error::Code
    {
        return pimpl->init();
    }

    void HSP::setDeviceAddress(uint8_t *addr)
    {
        pimpl->setDeviceAddress(addr);
    }

    void HSP::setOwnerService(const sys::Service *service)
    {
        ownerService = service;
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

    void HSP::start()
    {
        pimpl->start();
    }

    void HSP::stop()
    {
        pimpl->stop();
    }

    auto HSP::startRinging() const noexcept -> Error::Code
    {
        pimpl->startRinging();
        return Error::Success;
    }

    auto HSP::stopRinging() const noexcept -> Error::Code
    {
        pimpl->stopRinging();
        return Error::Success;
    }

    auto HSP::initializeCall() const noexcept -> Error::Code
    {
        pimpl->initializeCall();
        return Error::Success;
    }

    HSP::~HSP() = default;

    uint16_t HSP::HSPImpl::scoHandle = HCI_CON_HANDLE_INVALID;
    bd_addr_t HSP::HSPImpl::deviceAddr;
    std::array<char, commandBufferLength> HSP::HSPImpl::ATcommandBuffer;
    std::array<uint8_t, serviceBufferLength> HSP::HSPImpl::serviceBuffer;
    std::unique_ptr<SCO> HSP::HSPImpl::sco;
    std::unique_ptr<CellularInterface> HSP::HSPImpl::cellularInterface = nullptr;
    std::unique_ptr<AudioInterface> HSP::HSPImpl::audioInterface       = nullptr;
    const sys::Service *HSP::HSPImpl::ownerService;
    std::string HSP::HSPImpl::agServiceName = "PurePhone HSP";
    bool HSP::HSPImpl::isConnected          = false;
    bool HSP::HSPImpl::callAnswered         = false;
    bool HSP::HSPImpl::isRinging            = false;
    std::shared_ptr<HSPAudioDevice> HSP::HSPImpl::audioDevice;

    void HSP::HSPImpl::sendAudioEvent(audio::EventType event, audio::Event::DeviceState state)
    {
        auto evt       = std::make_shared<audio::Event>(event, state);
        auto msg       = std::make_shared<AudioEventRequest>(std::move(evt));
        auto &busProxy = const_cast<sys::Service *>(ownerService)->bus;
        busProxy.sendUnicast(std::move(msg), service::name::evt_manager);
    }

    void HSP::HSPImpl::packetHandler(uint8_t packetType, uint16_t channel, uint8_t *event, uint16_t eventSize)
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

    void HSP::HSPImpl::processHCIEvent(uint8_t *event)
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

    void HSP::HSPImpl::processHSPEvent(uint8_t *event)
    {
        auto eventDescriptor = event[2];
        switch (eventDescriptor) {
        case HSP_SUBEVENT_RFCOMM_CONNECTION_COMPLETE:
            if (hsp_subevent_rfcomm_connection_complete_get_status(event) != 0u) {
                LOG_DEBUG("RFCOMM connection establishement failed with status %u\n",
                          hsp_subevent_rfcomm_connection_complete_get_status(event));
                sendAudioEvent(audio::EventType::BlutoothHSPDeviceState, audio::Event::DeviceState::Disconnected);
                isConnected = false;
                break;
            }
            LOG_DEBUG("RFCOMM connection established.\n");
            sendAudioEvent(audio::EventType::BlutoothHSPDeviceState, audio::Event::DeviceState::Connected);
            isConnected = true;
            break;
        case HSP_SUBEVENT_RFCOMM_DISCONNECTION_COMPLETE:
            if (hsp_subevent_rfcomm_disconnection_complete_get_status(event) != 0u) {
                LOG_DEBUG("RFCOMM disconnection failed with status %u.\n",
                          hsp_subevent_rfcomm_disconnection_complete_get_status(event));
            }
            else {
                LOG_DEBUG("RFCOMM disconnected.\n");
                sendAudioEvent(audio::EventType::BlutoothHSPDeviceState, audio::Event::DeviceState::Disconnected);
            }
            isConnected = false;
            break;
        case HSP_SUBEVENT_AUDIO_CONNECTION_COMPLETE:
            if (hsp_subevent_audio_connection_complete_get_status(event) != 0u) {
                LOG_DEBUG("Audio connection establishment failed with status %u\n",
                          hsp_subevent_audio_connection_complete_get_status(event));
                sendAudioEvent(audio::EventType::BlutoothHSPDeviceState, audio::Event::DeviceState::Disconnected);
                isConnected = false;
                callAnswered = false;
            }
            else {
                scoHandle = hsp_subevent_audio_connection_complete_get_handle(event);
                LOG_DEBUG("Audio connection established with SCO handle 0x%04x.\n", scoHandle);
                callAnswered = true;
                hci_request_sco_can_send_now_event();
            }
            break;
        case HSP_SUBEVENT_AUDIO_DISCONNECTION_COMPLETE:
            LOG_DEBUG("Audio connection released.\n\n");
            scoHandle   = HCI_CON_HANDLE_INVALID;
            callAnswered = false;
            break;
        case HSP_SUBEVENT_MICROPHONE_GAIN_CHANGED:
            LOG_DEBUG("Received microphone gain change %d\n", hsp_subevent_microphone_gain_changed_get_gain(event));
            break;
        case HSP_SUBEVENT_SPEAKER_GAIN_CHANGED: {
            const auto volume = hsp_subevent_speaker_gain_changed_get_gain(event);
            auto &busProxy    = const_cast<sys::Service *>(ownerService)->bus;
            busProxy.sendUnicast(std::make_shared<message::bluetooth::HSPVolume>(volume), service::name::bluetooth);
            LOG_DEBUG("Received speaker gain change %d\n", hsp_subevent_speaker_gain_changed_get_gain(event));
        } break;
        case HSP_SUBEVENT_HS_CALL_ANSWER:
            LOG_DEBUG("HSP CALL ANSWER");
            cellularInterface->answerIncomingCall(const_cast<sys::Service *>(ownerService));
            break;
        case HSP_SUBEVENT_HS_CALL_HANGUP:
            LOG_DEBUG("HSP CALL HANGUP");
            cellularInterface->hangupCall(const_cast<sys::Service *>(ownerService));
            break;
        case HSP_SUBEVENT_HS_COMMAND: {
            ATcommandBuffer.fill(0);
            auto cmd_length   = hsp_subevent_hs_command_get_value_length(event);
            auto size         = cmd_length <= ATcommandBuffer.size() ? cmd_length : ATcommandBuffer.size();
            auto commandValue = hsp_subevent_hs_command_get_value(event);
            memcpy(ATcommandBuffer.data(), commandValue, size - 1);
            LOG_DEBUG("Received custom command: \"%s\". \nExit code or call hsp_ag_send_result.\n",
                      ATcommandBuffer.data());
            break;
        }
        case HSP_SUBEVENT_BUTTON_PRESSED: {
            if (scoHandle == HCI_CON_HANDLE_INVALID) {
                if (isRinging) {
                    LOG_DEBUG("Button event -> establish audio");
                    establishAudioConnection();
                    audioInterface->startAudioRouting(const_cast<sys::Service *>(ownerService));
                    cellularInterface->answerIncomingCall(const_cast<sys::Service *>(ownerService));
                }
                break;
            }
            LOG_DEBUG("Button event -> release audio");
            isRinging = false;
            if (callAnswered) {
                cellularInterface->hangupCall(const_cast<sys::Service *>(ownerService));
            }
            callAnswered = false;
            hsp_ag_release_audio_connection();
        } break;
        default:
            LOG_DEBUG("event not handled %u\n", event[2]);
            break;
        }
    }

    void HSP::HSPImpl::establishAudioConnection()
    {
        LOG_DEBUG("Establish Audio connection...\n");
        hsp_ag_establish_audio_connection();
    }

    auto HSP::HSPImpl::init() -> Error::Code
    {
        sco = std::make_unique<SCO>();
        sco->setOwnerService(ownerService);
        sco->init();

        cellularInterface = std::make_unique<CellularInterfaceImpl>();
        audioInterface    = std::make_unique<AudioInterfaceImpl>();

        Profile::initL2cap();
        Profile::initSdp();

        serviceBuffer.fill(0);
        constexpr uint32_t hspSdpRecordHandle = 0x10004;
        hsp_ag_create_sdp_record(serviceBuffer.data(), hspSdpRecordHandle, rfcommChannelNr, agServiceName.c_str());

        if (const auto status = sdp_register_service(serviceBuffer.data()); status != ERROR_CODE_SUCCESS) {
            LOG_ERROR("Can't register service. Status %x", status);
        }

        rfcomm_init();

        hsp_ag_init(rfcommChannelNr);
        hsp_ag_register_packet_handler(&packetHandler);

        // register for SCO packets
        hci_register_sco_packet_handler(&packetHandler);

        gap_discoverable_control(1);
        gap_set_class_of_device(CLASS_OF_DEVICE);

        LOG_INFO("HSP init done!");

        return bluetooth::Error::Success;
    }

    void HSP::HSPImpl::connect()
    {
        if (isConnected) {
            disconnect();
        }
        hsp_ag_connect(deviceAddr);
    }

    void HSP::HSPImpl::disconnect()
    {
        hsp_ag_release_audio_connection();
        hsp_ag_disconnect();
    }

    void HSP::HSPImpl::setDeviceAddress(bd_addr_t addr)
    {
        bd_addr_copy(deviceAddr, addr);
        LOG_DEBUG("Address set!");
    }

    void HSP::HSPImpl::setOwnerService(const sys::Service *service)
    {
        ownerService = service;
    }

    auto HSP::HSPImpl::getStreamData() -> std::shared_ptr<BluetoothStreamData>
    {
        return sco->getStreamData();
    }
    void HSP::HSPImpl::start()
    {
        if (!isConnected) {
            connect();
        }
    }
    void HSP::HSPImpl::stop()
    {
        stopRinging();
        hsp_ag_release_audio_connection();
        callAnswered = false;
    }

    void HSP::HSPImpl::startRinging() const noexcept
    {
        LOG_DEBUG("Bluetooth ring started");
        hsp_ag_start_ringing();
        isRinging = true;
    }

    void HSP::HSPImpl::stopRinging() const noexcept
    {
        LOG_DEBUG("Bluetooth ring stopped");
        hsp_ag_stop_ringing();
        isRinging = false;
    }

    void HSP::HSPImpl::initializeCall() const noexcept
    {
        stopRinging();
        establishAudioConnection();
    }

    void HSP::setAudioDevice(std::shared_ptr<bluetooth::BluetoothAudioDevice> audioDevice)
    {
        pimpl->setAudioDevice(audioDevice);
    }

    void HSP::HSPImpl::setAudioDevice(std::shared_ptr<bluetooth::BluetoothAudioDevice> audioDevice)
    {
        HSP::HSPImpl::audioDevice = std::static_pointer_cast<HSPAudioDevice>(audioDevice);
    }
} // namespace bluetooth
