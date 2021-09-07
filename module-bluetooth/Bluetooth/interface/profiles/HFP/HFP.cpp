// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "HFPImpl.hpp"
#include "HFP.hpp"

#include <Bluetooth/Error.hpp>
#include <log/log.hpp>
#include <service-evtmgr/Constants.hpp>
#include <service-audio/AudioMessage.hpp>
#include <BluetoothWorker.hpp>

extern "C"
{
#include "btstack.h"
#include "btstack_run_loop_freertos.h"
#include "btstack_stdin.h"
#include <btstack_defines.h>
}

namespace bluetooth
{
    HFP::HFP() : pimpl(std::make_unique<HFPImpl>(HFPImpl()))
    {}

    HFP::HFP(HFP &&other) noexcept : pimpl(std::move(other.pimpl))
    {}

    auto HFP::operator=(HFP &&other) noexcept -> HFP &
    {
        if (&other == this) {
            return *this;
        }
        pimpl = std::move(other.pimpl);
        return *this;
    }

    auto HFP::init() -> Error::Code
    {
        return pimpl->init();
    }

    void HFP::setDeviceAddress(uint8_t *addr)
    {
        pimpl->setDeviceAddress(addr);
    }

    void HFP::setOwnerService(const sys::Service *service)
    {
        ownerService = service;
        pimpl->setOwnerService(service);
    }

    void HFP::connect()
    {
        pimpl->connect();
    }

    void HFP::disconnect()
    {
        pimpl->disconnect();
    }

    void HFP::start()
    {
        pimpl->start();
    }

    void HFP::stop()
    {
        pimpl->stop();
    }
    auto HFP::startRinging() const noexcept -> Error::Code
    {
        return Error::Success;
    }
    auto HFP::stopRinging() const noexcept -> Error::Code
    {
        return Error::Success;
    }
    auto HFP::initializeCall() const noexcept -> Error::Code
    {
        pimpl->start();
        return Error::Success;
    }

    HFP::~HFP() = default;

    hci_con_handle_t HFP::HFPImpl::scoHandle = HCI_CON_HANDLE_INVALID;
    hci_con_handle_t HFP::HFPImpl::aclHandle = HCI_CON_HANDLE_INVALID;
    bd_addr_t HFP::HFPImpl::deviceAddr;
    std::array<uint8_t, serviceBufferLength> HFP::HFPImpl::serviceBuffer;
    std::unique_ptr<SCO> HFP::HFPImpl::sco;
    std::unique_ptr<CellularInterface> HFP::HFPImpl::cellularInterface = nullptr;
    const sys::Service *HFP::HFPImpl::ownerService;
    std::string HFP::HFPImpl::agServiceName = "PurePhone HFP";
    bool HFP::HFPImpl::isConnected          = false;
    SCOCodec HFP::HFPImpl::codec            = SCOCodec::CVSD;

    int HFP::HFPImpl::memory_1_enabled = 1;
    btstack_packet_callback_registration_t HFP::HFPImpl::hci_event_callback_registration;
    [[maybe_unused]] int HFP::HFPImpl::ag_indicators_nr = 7;
    hfp_ag_indicator_t HFP::HFPImpl::ag_indicators[]    = {
        // index, name, min range, max range, status, mandatory, enabled, status changed
        {1, "service", 0, 1, 1, 0, 0, 0},
        {2, "call", 0, 1, 0, 1, 1, 0},
        {3, "callsetup", 0, 3, 0, 1, 1, 0},
        {4, "battchg", 0, 5, 3, 0, 0, 0},
        {5, "signal", 0, 5, 5, 0, 1, 0},
        {6, "roam", 0, 1, 0, 0, 1, 0},
        {7, "callheld", 0, 2, 0, 1, 1, 0}};
    [[maybe_unused]] int HFP::HFPImpl::call_hold_services_nr                      = 5;
    const char *HFP::HFPImpl::call_hold_services[]                                = {"1", "1x", "2", "2x", "3"};
    [[maybe_unused]] int HFP::HFPImpl::hf_indicators_nr                           = 2;
    [[maybe_unused]] hfp_generic_status_indicator_t HFP::HFPImpl::hf_indicators[] = {
        {1, 1},
        {2, 1},
    };
    void HFP::HFPImpl::dump_supported_codecs(void)
    {
        LOG_DEBUG("Supported codecs: ");

        switch (codec) {
        case SCOCodec::CVSD:
            LOG_DEBUG("CVSD");
            break;
        case SCOCodec::both:
            LOG_DEBUG("CVSD");
            [[fallthrough]];
        case SCOCodec::mSBC:
            if (hci_extended_sco_link_supported()) {
                LOG_DEBUG("mSBC");
            }
            else {
                LOG_WARN("mSBC codec disabled because eSCO not supported by local controller.\n");
            }
            break;

        case SCOCodec::other:
            LOG_WARN("Using other codec");
            break;
        }
    }
    void HFP::HFPImpl::sendAudioEvent(audio::EventType event, audio::Event::DeviceState state)
    {
        auto evt       = std::make_shared<audio::Event>(event, state);
        auto msg       = std::make_shared<AudioEventRequest>(std::move(evt));
        auto &busProxy = const_cast<sys::Service *>(ownerService)->bus;
        busProxy.sendUnicast(std::move(msg), service::name::evt_manager);
    }

    void HFP::HFPImpl::packetHandler(uint8_t packetType, uint16_t channel, uint8_t *event, uint16_t eventSize)
    {
        switch (packetType) {
        case HCI_SCO_DATA_PACKET:
            if (READ_SCO_CONNECTION_HANDLE(event) != scoHandle) {
                break;
            }
            LOG_DEBUG("Processing SCO receive");

            sco->receive(event, eventSize);
            break;

        case HCI_EVENT_PACKET:
            processHCIEvent(event);
            break;
        default:
            break;
        }
    }

    void HFP::HFPImpl::processHCIEvent(uint8_t *event)
    {
        LOG_DEBUG("Inside hfp hci event");
        switch (hci_event_packet_get_type(event)) {
        case HCI_EVENT_SCO_CAN_SEND_NOW:
            LOG_DEBUG("SCO send event");

            sco->send(scoHandle);
            break;
        case HCI_EVENT_HFP_META:
            LOG_DEBUG("Processsing HFP event");
            processHFPEvent(event);
            break;
        default:
            break;
        }
    }

    void HFP::HFPImpl::processHFPEvent(uint8_t *event)
    {
        // NOTE some of the subevents are not mentioned here yet
        /*
         *
         * in hfp_ag.h there are descriptions of cellular and UI interface of HFP:
         * // Cellular Actions
         * and
         * // actions used by local device / user
         * those should be called to trigger requested state in HFP's state machine
         * (which this switch...case depends on)
         */
        switch (hci_event_hfp_meta_get_subevent_code(event)) {
        case HFP_SUBEVENT_SERVICE_LEVEL_CONNECTION_ESTABLISHED:
            std::uint8_t status;
            status = hfp_subevent_service_level_connection_established_get_status(event);
            if (status) {
                LOG_DEBUG("Connection failed, status 0x%02x\n", status);
                break;
            }
            aclHandle = hfp_subevent_service_level_connection_established_get_acl_handle(event);
            hfp_subevent_service_level_connection_established_get_bd_addr(event, deviceAddr);
            LOG_DEBUG("Service level connection established to %s.\n", bd_addr_to_str(deviceAddr));
            isConnected = true;
            sendAudioEvent(audio::EventType::BlutoothHFPDeviceState, audio::Event::DeviceState::Connected);
            dump_supported_codecs();
            break;
        case HFP_SUBEVENT_SERVICE_LEVEL_CONNECTION_RELEASED:
            LOG_DEBUG("Service level connection released.\n");
            aclHandle = HCI_CON_HANDLE_INVALID;

            sendAudioEvent(audio::EventType::BlutoothHFPDeviceState, audio::Event::DeviceState::Disconnected);

            break;
        case HFP_SUBEVENT_AUDIO_CONNECTION_ESTABLISHED:
            if (hfp_subevent_audio_connection_established_get_status(event)) {
                LOG_DEBUG("Audio connection establishment failed with status %u\n",
                          hfp_subevent_audio_connection_established_get_status(event));
            }
            else {
                scoHandle = hfp_subevent_audio_connection_established_get_sco_handle(event);
                LOG_DEBUG("Audio connection established with SCO handle 0x%04x.\n", scoHandle);
                codec = static_cast<SCOCodec>(hfp_subevent_audio_connection_established_get_negotiated_codec(event));
                dump_supported_codecs();

                sco->setCodec(codec);
                hci_request_sco_can_send_now_event();
                RunLoop::trigger();
            }
            break;
        case HFP_SUBEVENT_AUDIO_CONNECTION_RELEASED:
            LOG_DEBUG("Audio connection released\n");
            scoHandle = HCI_CON_HANDLE_INVALID;
            break;
        case HFP_SUBEVENT_START_RINGINIG:
            LOG_DEBUG("Start Ringing\n");

            // todo request here ringtone stream
            break;
        case HFP_SUBEVENT_STOP_RINGINIG:
            LOG_DEBUG("Stop Ringing\n");
            // todo stop ringtone stream here
            break;
        case HFP_SUBEVENT_PLACE_CALL_WITH_NUMBER:
            LOG_DEBUG("Outgoing call '%s'\n", hfp_subevent_place_call_with_number_get_number(event));
            // todo has to be feeded with proper phone number from cellular
            if (strcmp("1234567", hfp_subevent_place_call_with_number_get_number(event)) == 0 ||
                strcmp("7654321", hfp_subevent_place_call_with_number_get_number(event)) == 0 ||
                (memory_1_enabled && strcmp(">1", hfp_subevent_place_call_with_number_get_number(event)) == 0)) {
                LOG_DEBUG("Dialstring valid: accept call\n");
                hfp_ag_outgoing_call_accepted();
            }
            else {
                LOG_DEBUG("Dialstring invalid: reject call\n");
                hfp_ag_outgoing_call_rejected();
            }
            break;

        case HFP_SUBEVENT_ATTACH_NUMBER_TO_VOICE_TAG:
            // todo has to be feeded with proper phone number from cellular
            // LOG_DEBUG("Attach number to voice tag. Sending '1234567\n");
            hfp_ag_send_phone_number_for_voice_tag(aclHandle, "1234567");
            break;
        case HFP_SUBEVENT_TRANSMIT_DTMF_CODES:
            LOG_DEBUG("Send DTMF Codes: '%s'\n", hfp_subevent_transmit_dtmf_codes_get_dtmf(event));
            hfp_ag_send_dtmf_code_done(aclHandle);
            break;
        case HFP_SUBEVENT_CALL_ANSWERED:
            LOG_DEBUG("Call answered by HF\n");
            cellularInterface->answerIncomingCall(const_cast<sys::Service *>(ownerService));
            break;
        case HFP_SUBEVENT_CALL_TERMINATED:
            LOG_DEBUG("Call terminated by HF\n");
            cellularInterface->hangupCall(const_cast<sys::Service *>(ownerService));
            break;
        default:
            LOG_DEBUG("Event not handled %u\n", hci_event_hfp_meta_get_subevent_code(event));
            break;
        }
    }

    void HFP::HFPImpl::establishAudioConnection()
    {
        LOG_DEBUG("Establish Audio connection to %s...\n", bd_addr_to_str(deviceAddr));
        hfp_ag_establish_audio_connection(aclHandle);
    }
    static hfp_phone_number_t subscriber_number = {129, "225577"};
    auto HFP::HFPImpl::init() -> Error::Code
    {
        sco = std::make_unique<SCO>();
        sco->setOwnerService(ownerService);
        sco->init();

        cellularInterface = std::make_unique<CellularInterfaceImpl>();

        Profile::initL2cap();
        Profile::initSdp();

        serviceBuffer.fill(0);
        constexpr std::uint32_t hspSdpRecordHandle = 0x10001;
        uint16_t supported_features                = (1 << HFP_AGSF_ESCO_S4) | (1 << HFP_AGSF_HF_INDICATORS) |
                                      (1 << HFP_AGSF_CODEC_NEGOTIATION) | (1 << HFP_AGSF_EXTENDED_ERROR_RESULT_CODES) |
                                      (1 << HFP_AGSF_ENHANCED_CALL_CONTROL) | (1 << HFP_AGSF_ENHANCED_CALL_STATUS) |
                                      (1 << HFP_AGSF_ABILITY_TO_REJECT_A_CALL) | (1 << HFP_AGSF_IN_BAND_RING_TONE) |
                                      (1 << HFP_AGSF_VOICE_RECOGNITION_FUNCTION) | (1 << HFP_AGSF_THREE_WAY_CALLING);
        int wide_band_speech = 0;
        hfp_ag_create_sdp_record(serviceBuffer.data(),
                                 hspSdpRecordHandle,
                                 rfcommChannelNr,
                                 agServiceName.c_str(),
                                 0,
                                 supported_features,
                                 wide_band_speech);

        if (const auto status = sdp_register_service(serviceBuffer.data()); status != ERROR_CODE_SUCCESS) {
            LOG_ERROR("Can't register service. Status %x", status);
        }
        rfcomm_init();
        hfp_ag_init(rfcommChannelNr);
        hfp_ag_init_supported_features(supported_features);
        initCodecs();
        hfp_ag_init_ag_indicators(ag_indicators_nr, ag_indicators);
        hfp_ag_init_hf_indicators(hf_indicators_nr, hf_indicators);
        hfp_ag_init_call_hold_services(call_hold_services_nr, call_hold_services);
        hfp_ag_set_subcriber_number_information(&subscriber_number, 1);

        hci_event_callback_registration.callback = &packetHandler;
        hci_add_event_handler(&hci_event_callback_registration);
        hci_register_sco_packet_handler(&packetHandler);

        hfp_ag_register_packet_handler(&packetHandler);

        LOG_INFO("HFP init done!");

        return bluetooth::Error::Success;
    }

    void HFP::HFPImpl::connect()
    {
        if (!isConnected) {
            LOG_DEBUG("Connecting the HFP profile");
            hfp_ag_establish_service_level_connection(deviceAddr);
        }
    }

    void HFP::HFPImpl::disconnect()
    {
        hfp_ag_release_service_level_connection(aclHandle);
    }

    void HFP::HFPImpl::setDeviceAddress(bd_addr_t addr)
    {
        bd_addr_copy(deviceAddr, addr);
        LOG_DEBUG("Address set!");
    }

    void HFP::HFPImpl::setOwnerService(const sys::Service *service)
    {
        ownerService = service;
    }

    auto HFP::HFPImpl::getStreamData() -> std::shared_ptr<BluetoothStreamData>
    {
        return sco->getStreamData();
    }
    void HFP::HFPImpl::start()
    {
        if (!isConnected) {
            connect();
        }
        establishAudioConnection();
    }
    void HFP::HFPImpl::stop()
    {
        hfp_ag_release_audio_connection(aclHandle);
    }

    void HFP::HFPImpl::initCodecs()
    {
        std::vector<SCOCodec> codecsList;
        switch (codec) {

        case SCOCodec::other:
        case SCOCodec::CVSD:
            codecsList.push_back(SCOCodec::CVSD);
            break;
        case SCOCodec::mSBC:
            codecsList.push_back(SCOCodec::mSBC);
            break;
        case SCOCodec::both:
            codecsList.push_back(SCOCodec::CVSD);
            codecsList.push_back(SCOCodec::mSBC);
            break;
        }
        hfp_ag_init_codecs(codecsList.size(), reinterpret_cast<uint8_t *>(codecsList.data()));
    }
} // namespace bluetooth
