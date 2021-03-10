// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "HSPImpl.hpp"
#include "HSP.hpp"

#include <Bluetooth/Error.hpp>
#include <log/log.hpp>
#include <service-evtmgr/Constants.hpp>
#include <service-audio/AudioMessage.hpp>
#include <BluetoothWorker.hpp>
#include <module-services/service-bluetooth/service-bluetooth/BluetoothMessage.hpp>

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

    auto HSP::getStreamData() -> std::shared_ptr<BluetoothStreamData>
    {
        return pimpl->getStreamData();
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

    HSP::~HSP() = default;

    uint16_t HSP::HSPImpl::scoHandle = HCI_CON_HANDLE_INVALID;
    bd_addr_t HSP::HSPImpl::deviceAddr;
    std::array<char, commandBufferLength> HSP::HSPImpl::ATcommandBuffer;
    std::array<uint8_t, serviceBufferLength> HSP::HSPImpl::serviceBuffer;
    std::unique_ptr<SCO> HSP::HSPImpl::sco;
    const sys::Service *HSP::HSPImpl::ownerService;
    std::string HSP::HSPImpl::agServiceName = "PurePhone HSP";
    bool HSP::HSPImpl::isConnected          = false;

    void HSP::HSPImpl::sendAudioEvent(audio::EventType event, audio::Event::DeviceState state)
    {
        auto evt = std::make_shared<audio::Event>(event, state);
        auto msg = std::make_shared<AudioEventRequest>(std::move(evt));
        auto &busProxy = const_cast<sys::Service *>(ownerService)->bus;
        busProxy.sendUnicast(std::move(msg), service::name::evt_manager);
    }
    void HSP::HSPImpl::sendBluetoothEvent(BluetoothEventMessage::BluetoothEvent event)
    {
        auto evtMsg = std::make_shared<BluetoothEventMessage>(event);

        /*RawKey key{.state = RawKey::State::Released, .key_code = keyCode};
        gui::InputEvent event(key, gui::InputEvent::State::keyReleasedShort, static_cast<gui::KeyCode>(keyCode));*/
        LOG_INFO("Sending %s", "BluetoothEventMessage::BluetoothEvent::HSP_BUTTON_PRESSED");
        // auto message   = std::make_shared<app::AppInputEventMessage>(std::move(event));
        auto &busProxy = const_cast<sys::Service *>(ownerService)->bus;
        busProxy.sendUnicast(std::move(evtMsg), service::name::evt_manager);
    }
    void HSP::HSPImpl::packetHandler(uint8_t packetType, uint16_t channel, uint8_t *event, uint16_t eventSize)
    {
        switch (packetType) {
        case HCI_SCO_DATA_PACKET:
            if (READ_SCO_CONNECTION_HANDLE(event) != scoHandle) {
                break;
            }
            sco->receive(event, eventSize);
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
            sco->send(scoHandle);
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
                break;
            }
            LOG_DEBUG("RFCOMM connection established.\n");
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
            break;
        case HSP_SUBEVENT_AUDIO_CONNECTION_COMPLETE:
            if (hsp_subevent_audio_connection_complete_get_status(event) != 0u) {
                LOG_DEBUG("Audio connection establishment failed with status %u\n",
                          hsp_subevent_audio_connection_complete_get_status(event));
            }
            else {
                scoHandle = hsp_subevent_audio_connection_complete_get_handle(event);
                LOG_DEBUG("Audio connection established with SCO handle 0x%04x.\n", scoHandle);
                hci_request_sco_can_send_now_event();
                RunLoop::trigger();
            }
            break;
        case HSP_SUBEVENT_AUDIO_DISCONNECTION_COMPLETE:
            LOG_DEBUG("Audio connection released.\n\n");
            sendAudioEvent(audio::EventType::BlutoothHSPDeviceState, audio::Event::DeviceState::Disconnected);
            scoHandle = HCI_CON_HANDLE_INVALID;
            isConnected = false;
            break;
        case HSP_SUBEVENT_MICROPHONE_GAIN_CHANGED:
            LOG_DEBUG("Received microphone gain change %d\n", hsp_subevent_microphone_gain_changed_get_gain(event));
            break;
        case HSP_SUBEVENT_SPEAKER_GAIN_CHANGED:
            LOG_DEBUG("Received speaker gain change %d\n", hsp_subevent_speaker_gain_changed_get_gain(event));
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
            sendBluetoothEvent(BluetoothEventMessage::BluetoothEvent::HSP_BUTTON_PRESSED);
            break;
        }
        default:
            LOG_DEBUG("event not handled %u\n", event[2]);
            break;
        }
    }

    auto HSP::HSPImpl::init() -> Error::Code
    {
        sco = std::make_unique<SCO>();
        sco->setOwnerService(ownerService);
        sco->init();

        l2cap_init();
        sdp_init();

        serviceBuffer.fill(0);
        hsp_ag_create_sdp_record(serviceBuffer.data(), 0x10001, rfcommChannelNr, agServiceName.c_str());
        sdp_register_service(serviceBuffer.data());

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
        if (!isConnected) {
            hsp_ag_connect(deviceAddr);
        }
    }

    void HSP::HSPImpl::disconnect()
    {
        hsp_ag_disconnect();
    }

    void HSP::HSPImpl::setDeviceAddress(bd_addr_t addr)
    {
        bd_addr_copy(deviceAddr, addr);
        LOG_INFO("Address set!");
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
        LOG_DEBUG("Establish Audio connection to %s...\n", bd_addr_to_str(deviceAddr));
        hsp_ag_establish_audio_connection();
    }
    void HSP::HSPImpl::stop()
    {
        hsp_ag_release_audio_connection();
    }

} // namespace Bt
