// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "HFPImpl.hpp"
#include "HFP.hpp"

#include <Bluetooth/Error.hpp>
#include <log/log.hpp>
#include <service-evtmgr/Constants.hpp>
#include <service-audio/AudioMessage.hpp>
#include <service-bluetooth/messages/AudioVolume.hpp>
#include <service-bluetooth/messages/Connect.hpp>
#include <service-bluetooth/messages/Disconnect.hpp>
#include <service-bluetooth/messages/RequestStatusIndicatorData.hpp>

#include <BluetoothWorker.hpp>
#include "SCO/ScoUtils.hpp"

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

    void HFP::setDevice(const Devicei &device)
    {
        pimpl->setDevice(device);
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
    auto HFP::startRinging() const noexcept -> Error::Code
    {
        pimpl->startRinging();
        return Error::Success;
    }
    auto HFP::stopRinging() const noexcept -> Error::Code
    {
        pimpl->stopRinging();
        return Error::Success;
    }
    auto HFP::initializeCall() const noexcept -> Error::Code
    {
        pimpl->initializeCall();
        return Error::Success;
    }
    auto HFP::terminateCall() const noexcept -> Error::Code
    {
        pimpl->terminateCall();
        return Error::Success;
    }
    void HFP::setAudioDevice(std::shared_ptr<bluetooth::BluetoothAudioDevice> audioDevice)
    {
        pimpl->setAudioDevice(audioDevice);
    }
    auto HFP::callAnswered() const noexcept -> Error::Code
    {
        return pimpl->setupCall();
    }
    auto HFP::setIncomingCallNumber(const std::string &num) const noexcept -> Error::Code
    {
        LOG_SENSITIVE(LOGDEBUG, "Setting number: %s", num.c_str());
        return pimpl->setIncomingCallNumber(num);
    }
    auto HFP::setSignalStrength(int bars) const noexcept -> Error::Code
    {
        LOG_DEBUG("Setting RSSI bars: %d/4", bars);
        return pimpl->setSignalStrength(bars);
    }
    auto HFP::setOperatorName(const std::string_view &name) const noexcept -> Error::Code
    {
        LOG_DEBUG("Setting operator name: %s", name.data());
        return pimpl->setOperatorName(name);
    }
    auto HFP::setBatteryLevel(const BatteryLevel &level) const noexcept -> Error::Code
    {
        LOG_DEBUG("Setting battery level: %d", level.getBatteryLevel());
        return pimpl->setBatteryLevel(level);
    }

    HFP::~HFP() = default;

    hci_con_handle_t HFP::HFPImpl::scoHandle = HCI_CON_HANDLE_INVALID;
    hci_con_handle_t HFP::HFPImpl::aclHandle = HCI_CON_HANDLE_INVALID;
    std::array<uint8_t, serviceBufferLength> HFP::HFPImpl::serviceBuffer;
    std::unique_ptr<SCO> HFP::HFPImpl::sco;
    std::unique_ptr<CellularInterface> HFP::HFPImpl::cellularInterface = nullptr;
    std::unique_ptr<AudioInterface> HFP::HFPImpl::audioInterface       = nullptr;
    const sys::Service *HFP::HFPImpl::ownerService;
    const std::string_view HFP::HFPImpl::agServiceName = "Mudita Pure HFP";
    SCOCodec HFP::HFPImpl::codec            = SCOCodec::CVSD;
    std::shared_ptr<CVSDAudioDevice> HFP::HFPImpl::audioDevice;

    [[maybe_unused]] int HFP::HFPImpl::memory_1_enabled = 1;
    btstack_packet_callback_registration_t HFP::HFPImpl::hci_event_callback_registration;
    [[maybe_unused]] int HFP::HFPImpl::ag_indicators_nr = 7;
    hfp_ag_indicator_t HFP::HFPImpl::ag_indicators[]    = {
        // index, name, min range, max range, status, mandatory, enabled, status changed
        {1, "service", 0, 1, 1, 0, 0, 0},
        {2, "call", 0, 1, 0, 1, 1, 0},
        {3, "callsetup", 0, 3, 0, 1, 1, 0},
        {4, "battchg", 0, 5, 3, 0, 1, 0},
        {5, "signal", 0, 4, 5, 0, 1, 0},
        {6, "roam", 0, 1, 0, 0, 1, 0},
        {7, "callheld", 0, 2, 0, 1, 1, 0}};
    [[maybe_unused]] int HFP::HFPImpl::call_hold_services_nr                      = 5;
    const char *HFP::HFPImpl::call_hold_services[]                                = {"1", "1x", "2", "2x", "3"};
    [[maybe_unused]] int HFP::HFPImpl::hf_indicators_nr                           = 2;
    [[maybe_unused]] hfp_generic_status_indicator_t HFP::HFPImpl::hf_indicators[] = {
        {1, 1},
        {2, 1},
    };
    Devicei HFP::HFPImpl::device;
    bool HFP::HFPImpl::isConnected                  = false;
    bool HFP::HFPImpl::isIncomingCall               = false;
    bool HFP::HFPImpl::isCallInitialized            = false;

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

    void HFP::HFPImpl::processHCIEvent(uint8_t *event)
    {
        switch (hci_event_packet_get_type(event)) {
        case HCI_EVENT_SCO_CAN_SEND_NOW:
            if (audioDevice != nullptr) {
                audioDevice->onDataSend(scoHandle);
            }
            else {
                LOG_DEBUG("Audiodevice nullptr :(");
            }
            break;
        case HCI_EVENT_HFP_META:
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
                sendAudioEvent(audio::EventType::BlutoothHFPDeviceState, audio::Event::DeviceState::Disconnected);
                isConnected = false;
                break;
            }
            aclHandle = hfp_subevent_service_level_connection_established_get_acl_handle(event);
            hfp_subevent_service_level_connection_established_get_bd_addr(event, device.address);
            LOG_DEBUG("Service level connection established to %s.\n", bd_addr_to_str(device.address));
            isConnected = true;
            sendAudioEvent(audio::EventType::BlutoothHFPDeviceState, audio::Event::DeviceState::Connected);
            {
                auto &busProxy     = const_cast<sys::Service *>(ownerService)->bus;
                device.deviceState = DeviceState::ConnectedVoice;
                busProxy.sendUnicast(std::make_shared<message::bluetooth::ConnectResult>(device, true),
                                     service::name::bluetooth);
                // request cellular status indicators for HFP
                busProxy.sendUnicast(std::make_shared<message::bluetooth::RequestStatusIndicatorData>(),
                                     service::name::bluetooth);
            }
            dump_supported_codecs();
            break;
        case HFP_SUBEVENT_SERVICE_LEVEL_CONNECTION_RELEASED:
            LOG_DEBUG("Service level connection released.\n");
            aclHandle   = HCI_CON_HANDLE_INVALID;
            isConnected = false;
            sendAudioEvent(audio::EventType::BlutoothHFPDeviceState, audio::Event::DeviceState::Disconnected);
            {
                auto &busProxy = const_cast<sys::Service *>(ownerService)->bus;
                busProxy.sendUnicast(std::make_shared<message::bluetooth::DisconnectResult>(device),
                                     service::name::bluetooth);
            }
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
                audioInterface->startAudioRouting(const_cast<sys::Service *>(ownerService));
                hci_request_sco_can_send_now_event();
                RunLoop::trigger();
            }
            break;
        case HFP_SUBEVENT_AUDIO_CONNECTION_RELEASED:
            LOG_DEBUG("Audio connection released");
            scoHandle = HCI_CON_HANDLE_INVALID;
            audioInterface->stopAudioRouting(const_cast<sys::Service *>(ownerService));
            audioDevice.reset();
            break;
        case HFP_SUBEVENT_START_RINGING:
            LOG_DEBUG("Start Ringing\n");
            // todo request here ringtone stream
            break;
        case HFP_SUBEVENT_STOP_RINGING:
            LOG_DEBUG("Stop Ringing\n");
            // todo stop ringtone stream here
            break;
        case HFP_SUBEVENT_PLACE_CALL_WITH_NUMBER:
            break;
        case HFP_SUBEVENT_RING:
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
            LOG_DEBUG("Call answered");
            cellularInterface->answerIncomingCall(const_cast<sys::Service *>(ownerService));
            break;

        case HFP_SUBEVENT_SPEAKER_VOLUME: {
            const auto volume = hfp_subevent_speaker_volume_get_gain(event);
            auto &busProxy    = const_cast<sys::Service *>(ownerService)->bus;
            busProxy.sendUnicast(std::make_shared<message::bluetooth::HFPVolume>(volume), service::name::bluetooth);
            LOG_DEBUG("Received speaker gain change %d\n", hsp_subevent_speaker_gain_changed_get_gain(event));
        } break;

        case HFP_SUBEVENT_CALL_TERMINATED:
            LOG_DEBUG("Call terminated");
            cellularInterface->hangupCall(const_cast<sys::Service *>(ownerService));
            break;
        default:
            LOG_DEBUG("Event not handled %u\n", hci_event_hfp_meta_get_subevent_code(event));
            break;
        }
    }

    static hfp_phone_number_t subscriber_number = {129, "225577"};
    auto HFP::HFPImpl::init() -> Error::Code
    {
        sco = std::make_unique<SCO>();
        sco->setOwnerService(ownerService);
        sco->init();

        cellularInterface = std::make_unique<CellularInterfaceImpl>();
        audioInterface    = std::make_unique<AudioInterfaceImpl>();

        Profile::initL2cap();
        Profile::initSdp();

        serviceBuffer.fill(0);
        constexpr std::uint32_t hspSdpRecordHandle = 0x10004;
        uint16_t supported_features                = (1 << HFP_AGSF_ESCO_S4) | /* (1 << HFP_AGSF_HF_INDICATORS) | */
                                      (1 << HFP_AGSF_CODEC_NEGOTIATION) | (1 << HFP_AGSF_EXTENDED_ERROR_RESULT_CODES) |
                                      (1 << HFP_AGSF_ENHANCED_CALL_CONTROL) | (1 << HFP_AGSF_ENHANCED_CALL_STATUS) |
                                      (1 << HFP_AGSF_ABILITY_TO_REJECT_A_CALL) /*| (1 << HFP_AGSF_IN_BAND_RING_TONE) |*/
            /* (1 << HFP_AGSF_VOICE_RECOGNITION_FUNCTION) |(1 << HFP_AGSF_THREE_WAY_CALLING)*/;
        int wide_band_speech = 0;
        hfp_ag_create_sdp_record(serviceBuffer.data(),
                                 hspSdpRecordHandle,
                                 rfcommChannelNr,
                                 agServiceName.data(),
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

        hfp_ag_set_use_in_band_ring_tone(0);

        LOG_INFO("HFP init done!");

        return bluetooth::Error::Success;
    }

    void HFP::HFPImpl::connect()
    {
        if (isConnected) {
            disconnect();
        }
        LOG_DEBUG("Connecting the HFP profile");
        hfp_ag_establish_service_level_connection(device.address);
        hfp_ag_set_speaker_gain(aclHandle, 8);
        hfp_ag_set_microphone_gain(aclHandle, 10);
    }

    void HFP::HFPImpl::disconnect()
    {
        hfp_ag_release_service_level_connection(aclHandle);
        auto &busProxy = const_cast<sys::Service *>(ownerService)->bus;
        busProxy.sendUnicast(std::make_shared<message::bluetooth::DisconnectResult>(device), service::name::bluetooth);
    }

    void HFP::HFPImpl::setDevice(Devicei dev)
    {
        device = dev;
        LOG_DEBUG("Device set!");
    }

    void HFP::HFPImpl::setOwnerService(const sys::Service *service)
    {
        ownerService = service;
    }

    auto HFP::HFPImpl::getStreamData() -> std::shared_ptr<BluetoothStreamData>
    {
        return sco->getStreamData();
    }

    void HFP::HFPImpl::terminateCall() const noexcept
    {
        hfp_ag_terminate_call();
        hfp_ag_release_audio_connection(aclHandle);
        isCallInitialized = false;
        isIncomingCall    = false;
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
    void HFP::HFPImpl::initializeCall() const noexcept
    {
        if (!isCallInitialized && !isIncomingCall) {
            LOG_DEBUG("Initializing outgoing call");
            constexpr auto mockNumber = "1234567"; // Mocking phone number - it is not used in stack
            hfp_ag_outgoing_call_initiated(mockNumber);
            isCallInitialized = true;
        }
    }
    void HFP::HFPImpl::setAudioDevice(std::shared_ptr<bluetooth::BluetoothAudioDevice> audioDevice)
    {
        HFP::HFPImpl::audioDevice = std::static_pointer_cast<CVSDAudioDevice>(audioDevice);
        HFP::HFPImpl::audioDevice->setAclHandle(aclHandle);
        LOG_DEBUG("Audiodevice set!");
    }
    void HFP::HFPImpl::startRinging() const noexcept
    {
        LOG_DEBUG("Starting incoming call");
        isIncomingCall = true;
        hfp_ag_incoming_call();
    }
    void HFP::HFPImpl::stopRinging() const noexcept
    {
        LOG_DEBUG("Stop ringing called!");
    }
    auto HFP::HFPImpl::setupCall() const noexcept -> Error::Code
    {
        if (isIncomingCall) {
            LOG_DEBUG("Answering incoming call");
            hfp_ag_answer_incoming_call();
        }
        else {
            LOG_DEBUG("Establishing outgoing call");
            audioInterface->startAudioRouting(const_cast<sys::Service *>(ownerService));
            hfp_ag_outgoing_call_established();
        }

        return Error::Success;
    }
    auto HFP::HFPImpl::setIncomingCallNumber(const std::string &num) const noexcept -> Error::Code
    {
        if (!isIncomingCall) {
            isIncomingCall = true;
            hfp_ag_incoming_call();
        }
        hfp_ag_set_clip(129, num.c_str());
        return Error::Success;
    }
    auto HFP::HFPImpl::setSignalStrength(int bars) const noexcept -> Error::Code
    {
        hfp_ag_set_signal_strength(bars);
        return Error::Success;
    }

    auto HFP::HFPImpl::setOperatorName(const std::string_view &name) const noexcept -> Error::Code
    {
        auto result = hfp_ag_set_operator_name(HFP::HFPImpl::aclHandle, name.data());
        LOG_DEBUG("Operator set name result: %d", result);
        return Error::Success;
    }
    auto HFP::HFPImpl::setBatteryLevel(const BatteryLevel &level) const noexcept -> Error::Code
    {
        auto result = hfp_ag_set_battery_level(level.getBatteryLevelBars());
        LOG_DEBUG("Battery level (bars): %d, set result: %d", level.getBatteryLevelBars(), result);
        return Error::Success;
    }

} // namespace bluetooth
