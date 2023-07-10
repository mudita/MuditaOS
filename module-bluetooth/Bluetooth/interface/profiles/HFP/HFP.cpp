// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "HFPImpl.hpp"
#include "HFP.hpp"

#include <Bluetooth/Result.hpp>
#include <log/log.hpp>
#include <service-evtmgr/ServiceEventManagerName.hpp>
#include <service-audio/AudioMessage.hpp>
#include <service-bluetooth/messages/AudioVolume.hpp>
#include <service-bluetooth/messages/Connect.hpp>
#include <service-bluetooth/messages/Disconnect.hpp>
#include <service-bluetooth/messages/RequestStatusIndicatorData.hpp>
#include <service-bluetooth/ServiceBluetoothName.hpp>

#include <BluetoothWorker.hpp>
#include "SCO/ScoUtils.hpp"

#define ARRAY_LENGTH(x) (sizeof(x) / sizeof((x)[0]))

extern "C"
{
#include "btstack.h"
#include "btstack_run_loop_freertos.h"
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

    auto HFP::init() -> Result::Code
    {
        return pimpl->init();
    }

    void HFP::setDevice(const Devicei &device)
    {
        pimpl->setDevice(device);
    }

    void HFP::setOwnerService(sys::Service *service)
    {
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

    auto HFP::incomingCallStarted() const noexcept -> Result::Code
    {
        return pimpl->incomingCallStarted();
    }

    auto HFP::outgoingCallStarted(const std::string &number) const noexcept -> Result::Code
    {
        return pimpl->outgoingCallStarted(number);
    }

    auto HFP::incomingCallAnswered() const noexcept -> Result::Code
    {
        return pimpl->incomingCallAnswered();
    }

    auto HFP::outgoingCallAnswered() const noexcept -> Result::Code
    {
        return pimpl->outgoingCallAnswered();
    }

    auto HFP::setIncomingCallNumber(const std::string &num) const noexcept -> Result::Code
    {
        return pimpl->setIncomingCallNumber(num);
    }

    auto HFP::callTerminated() const noexcept -> Result::Code
    {
        return pimpl->callTerminated();
    }

    auto HFP::callMissed() const noexcept -> Result::Code
    {
        return pimpl->callMissed();
    }

    void HFP::setAudioDevice(std::shared_ptr<bluetooth::BluetoothAudioDevice> audioDevice)
    {
        pimpl->setAudioDevice(audioDevice);
    }

    auto HFP::setSignalStrength(int bars) const noexcept -> Result::Code
    {
        return pimpl->setSignalStrength(bars);
    }

    auto HFP::setOperatorName(const std::string_view &name) const noexcept -> Result::Code
    {
        return pimpl->setOperatorName(name);
    }

    auto HFP::setBatteryLevel(const BatteryLevel &level) const noexcept -> Result::Code
    {
        return pimpl->setBatteryLevel(level);
    }

    auto HFP::setNetworkRegistrationStatus(bool registered) const noexcept -> Result::Code
    {
        return pimpl->setNetworkRegistrationStatus(registered);
    }

    auto HFP::setRoamingStatus(bool enabled) const noexcept -> Result::Code
    {
        return pimpl->setRoamingStatus(enabled);
    }

    HFP::~HFP()
    {
        pimpl->disconnect();
        pimpl->deInit();
    }

    hci_con_handle_t HFP::HFPImpl::scoHandle = HCI_CON_HANDLE_INVALID;
    hci_con_handle_t HFP::HFPImpl::aclHandle = HCI_CON_HANDLE_INVALID;

    std::uint8_t HFP::HFPImpl::serviceBuffer[serviceBufferSize];

    std::unique_ptr<SCO> HFP::HFPImpl::sco;
    SCOCodec HFP::HFPImpl::codec                       = SCOCodec::CVSD;
    std::shared_ptr<CVSDAudioDevice> HFP::HFPImpl::audioDevice;

    std::unique_ptr<CellularInterface> HFP::HFPImpl::cellularInterface;
    std::unique_ptr<AudioInterface> HFP::HFPImpl::audioInterface;

    sys::Service *HFP::HFPImpl::ownerService;
    Devicei HFP::HFPImpl::device;

    const char *HFP::HFPImpl::agServiceName = "Mudita Pure HFP";
    btstack_packet_callback_registration_t HFP::HFPImpl::hciEventCallbackRegistration;

    hfp_ag_indicator_t HFP::HFPImpl::agIndicators[] = {
        // index, name, min range, max range, status, mandatory, enabled, status changed
        {1, "service", 0, 1, 1, 0, 0, 0},
        {2, "call", 0, 1, 0, 1, 1, 0},
        {3, "callsetup", 0, 3, 0, 1, 1, 0},
        {4, "battchg", 0, 5, 3, 0, 1, 0},
        {5, "signal", 0, 4, 5, 0, 1, 0},
        {6, "roam", 0, 1, 0, 0, 1, 0},
        {7, "callheld", 0, 2, 0, 1, 1, 0}};

    const char *HFP::HFPImpl::callHoldServices[] = {"1", "1x", "2", "2x", "3"};

    hfp_generic_status_indicator_t HFP::HFPImpl::hfIndicators[] = {
        {1, 1}, // Enhanced safety indicator
        {2, 1}, // Remaining level of battery indicator
    };

    void HFP::HFPImpl::logSupportedCodecs()
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
                LOG_WARN("mSBC codec disabled because eSCO not supported by local controller");
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
        ownerService->bus.sendUnicast(std::move(msg), service::name::evt_manager);
    }

    void HFP::HFPImpl::packetHandler(std::uint8_t packetType,
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

    void HFP::HFPImpl::processHCIEvent(std::uint8_t *event)
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
        case HCI_EVENT_HFP_META:
            processHFPEvent(event);
            break;
        default:
            break;
        }
    }

    void HFP::HFPImpl::processHFPEvent(std::uint8_t *event)
    {
        switch (hci_event_hfp_meta_get_subevent_code(event)) {
        case HFP_SUBEVENT_SERVICE_LEVEL_CONNECTION_ESTABLISHED: {
            const auto status = hfp_subevent_service_level_connection_established_get_status(event);
            if (status != ERROR_CODE_SUCCESS) {
                LOG_ERROR("Connection failed, status: 0x%02X", status);
                sendAudioEvent(audio::EventType::BluetoothHFPDeviceState, audio::Event::DeviceState::Disconnected);
                break;
            }

            aclHandle = hfp_subevent_service_level_connection_established_get_acl_handle(event);
            hfp_subevent_service_level_connection_established_get_bd_addr(event, device.address);
            LOG_INFO("Service level connection established to %s", bd_addr_to_str(device.address));

            sendAudioEvent(audio::EventType::BluetoothHFPDeviceState, audio::Event::DeviceState::Connected);
            device.deviceState = DeviceState::ConnectedVoice;

            ownerService->bus.sendUnicast(std::make_shared<message::bluetooth::ConnectResult>(
                                              device, message::bluetooth::ConnectResult::Result::Success),
                                          service::name::bluetooth);
            ownerService->bus.sendUnicast(std::make_shared<message::bluetooth::RequestStatusIndicatorData>(),
                                          service::name::bluetooth);

            logSupportedCodecs();
        } break;

        case HFP_SUBEVENT_SERVICE_LEVEL_CONNECTION_RELEASED: {
            aclHandle = HCI_CON_HANDLE_INVALID;

            sendAudioEvent(audio::EventType::BluetoothHFPDeviceState, audio::Event::DeviceState::Disconnected);
            ownerService->bus.sendUnicast(std::make_shared<message::bluetooth::DisconnectResult>(device),
                                          service::name::bluetooth);

            LOG_INFO("Service level connection released");
        } break;

        case HFP_SUBEVENT_AUDIO_CONNECTION_ESTABLISHED: {
            const auto status = hfp_subevent_audio_connection_established_get_status(event);
            if (status != ERROR_CODE_SUCCESS) {
                LOG_ERROR("Audio connection establishment failed, status: 0x%02X", status);
                break;
            }

            scoHandle = hfp_subevent_audio_connection_established_get_sco_handle(event);
            LOG_INFO("Audio connection established with SCO handle 0x%04X", scoHandle);

            codec = static_cast<SCOCodec>(hfp_subevent_audio_connection_established_get_negotiated_codec(event));
            logSupportedCodecs();

            audioInterface->startAudioRouting(ownerService);
            hci_request_sco_can_send_now_event();
            RunLoop::trigger();
        } break;

        case HFP_SUBEVENT_AUDIO_CONNECTION_RELEASED: {
            scoHandle = HCI_CON_HANDLE_INVALID;
            audioDevice.reset();
            LOG_INFO("Audio connection released");
        } break;

        case HFP_SUBEVENT_START_RINGING:
            break;

        case HFP_SUBEVENT_STOP_RINGING:
            break;

        case HFP_SUBEVENT_RING:
            break;

        case HFP_SUBEVENT_PLACE_CALL_WITH_NUMBER: {
            const auto receivedNumber = hfp_subevent_place_call_with_number_get_number(event);
            LOG_INFO("Requested call from HFP to number %s", receivedNumber);
            cellularInterface->dialNumber(ownerService, receivedNumber);
        } break;

        case HFP_SUBEVENT_ATTACH_NUMBER_TO_VOICE_TAG:
            break;

        case HFP_SUBEVENT_TRANSMIT_DTMF_CODES: {
            const auto digitStr = hfp_subevent_transmit_dtmf_codes_get_dtmf(event);
            LOG_DEBUG("Send DTMF Codes: '%s'", digitStr);
            try {
                cellularInterface->sendDTMFCode(ownerService, DTMFCode(digitStr));
            }
            catch (std::out_of_range &e) {
                LOG_ERROR("Can't send DTMF code for digit: %s", digitStr);
            }
            hfp_ag_send_dtmf_code_done(aclHandle);
        } break;

        case HFP_SUBEVENT_CALL_ANSWERED: {
            LOG_INFO("Event: call answered");
            cellularInterface->answerIncomingCall(ownerService);
        } break;

        case HFP_SUBEVENT_SPEAKER_VOLUME: {
            const auto volume = hfp_subevent_speaker_volume_get_gain(event);
            ownerService->bus.sendUnicast(std::make_shared<message::bluetooth::HFPVolume>(volume),
                                          service::name::bluetooth);
            LOG_INFO("Received speaker gain change to %d", volume);
        } break;

        case HFP_SUBEVENT_CALL_TERMINATED: {
            LOG_INFO("Event: call terminated");
            cellularInterface->hangupCall(ownerService);
        } break;

        default:
            LOG_INFO("Event not handled: 0x%02X", hci_event_hfp_meta_get_subevent_code(event));
            break;
        }
    }

    static hfp_phone_number_t subscriber_number = {129, "225577"};
    auto HFP::HFPImpl::init() -> Result::Code
    {
        sco = std::make_unique<SCO>();
        sco->setOwnerService(ownerService);
        sco->init();

        cellularInterface = std::make_unique<CellularInterfaceImpl>();
        audioInterface    = std::make_unique<AudioInterfaceImpl>();

        Profile::initL2cap();
        Profile::initSdp();

        std::memset(serviceBuffer, 0, sizeof(serviceBuffer));

        constexpr auto wideBandSpeechEnabled = 0;
        constexpr auto abilityToRejectCall   = 1;
        constexpr std::uint16_t supportedFeatures =
            (1 << HFP_AGSF_ESCO_S4) | (1 << HFP_AGSF_CODEC_NEGOTIATION) | (1 << HFP_AGSF_EXTENDED_ERROR_RESULT_CODES) |
            (1 << HFP_AGSF_ENHANCED_CALL_CONTROL) | (1 << HFP_AGSF_ENHANCED_CALL_STATUS) |
            (1 << HFP_AGSF_ABILITY_TO_REJECT_A_CALL) | (1 << HFP_AGSF_ATTACH_A_NUMBER_TO_A_VOICE_TAG);

        hfp_ag_create_sdp_record(serviceBuffer,
                                 hfpSdpRecordHandle,
                                 rfcommChannelNr,
                                 agServiceName,
                                 abilityToRejectCall,
                                 supportedFeatures,
                                 wideBandSpeechEnabled);
        if (const auto status = sdp_register_service(serviceBuffer); status != ERROR_CODE_SUCCESS) {
            LOG_ERROR("Can't register service, status 0x%02X", status);
        }

        rfcomm_init();

        hfp_ag_init(rfcommChannelNr);
        hfp_ag_init_supported_features(supportedFeatures);

        initCodecs();

        hfp_ag_init_ag_indicators(ARRAY_LENGTH(agIndicators), agIndicators);
        hfp_ag_init_hf_indicators(ARRAY_LENGTH(hfIndicators), hfIndicators);

        hfp_ag_init_call_hold_services(ARRAY_LENGTH(callHoldServices), callHoldServices);

        /* TODO here own number from cellular should be provided */
        hfp_ag_set_subcriber_number_information(&subscriber_number, 0);

        hciEventCallbackRegistration.callback = &packetHandler;
        hci_add_event_handler(&hciEventCallbackRegistration);
        hci_register_sco_packet_handler(&packetHandler);
        hfp_ag_register_packet_handler(&packetHandler);

        hfp_ag_set_use_in_band_ring_tone(0);

        LOG_INFO("HFP initialized!");
        return bluetooth::Result::Code::Success;
    }

    void HFP::HFPImpl::connect()
    {
        /* Disconnect before attempting to connect */
        disconnect();

        LOG_INFO("Connecting HFP profile");
        hfp_ag_establish_service_level_connection(device.address);
        hfp_ag_set_speaker_gain(aclHandle, 8);
        hfp_ag_set_microphone_gain(aclHandle, 10);
    }

    void HFP::HFPImpl::disconnect()
    {
        LOG_INFO("Disconnecting HFP profile");
        if (hfp_ag_release_service_level_connection(aclHandle) == ERROR_CODE_SUCCESS) {
            ownerService->bus.sendUnicast(std::make_shared<message::bluetooth::DisconnectResult>(device),
                                          service::name::bluetooth);
        }
    }

    void HFP::HFPImpl::setDevice(Devicei dev)
    {
        device = std::move(dev);
        LOG_DEBUG("Device set!");
    }

    void HFP::HFPImpl::setOwnerService(sys::Service *service)
    {
        ownerService = service;
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
        hfp_ag_init_codecs(codecsList.size(), reinterpret_cast<std::uint8_t *>(codecsList.data()));
    }

    void HFP::HFPImpl::setAudioDevice(std::shared_ptr<bluetooth::BluetoothAudioDevice> audioDev)
    {
        HFP::HFPImpl::audioDevice = std::static_pointer_cast<CVSDAudioDevice>(audioDev);
        HFP::HFPImpl::audioDevice->setAclHandle(aclHandle);
        LOG_DEBUG("Audiodevice set!");
    }

    auto HFP::HFPImpl::incomingCallStarted() const noexcept -> Result::Code
    {
        LOG_DEBUG("Incoming call received");
        hfp_ag_incoming_call();
        return Result::Code::Success;
    }

    auto HFP::HFPImpl::outgoingCallStarted(const std::string &number) const noexcept -> Result::Code
    {
        LOG_DEBUG("Outgoing call placed");
        hfp_ag_outgoing_call_initiated();
        hfp_ag_outgoing_call_accepted();
        hfp_ag_outgoing_call_ringing();
        hfp_ag_send_phone_number_for_voice_tag(aclHandle, number.c_str());
        return Result::Code::Success;
    }

    auto HFP::HFPImpl::incomingCallAnswered() const noexcept -> Result::Code
    {
        LOG_DEBUG("Incoming call answered");
        hfp_ag_answer_incoming_call();
        return Result::Code::Success;
    }

    auto HFP::HFPImpl::outgoingCallAnswered() const noexcept -> Result::Code
    {
        LOG_DEBUG("Outgoing call answered");
        hfp_ag_outgoing_call_established();
        return Result::Code::Success;
    }

    auto HFP::HFPImpl::callTerminated() const noexcept -> Result::Code
    {
        LOG_DEBUG("Outgoing call terminated");
        hfp_ag_terminate_call();
        return Result::Code::Success;
    }

    auto HFP::HFPImpl::callMissed() const noexcept -> Result::Code
    {
        LOG_DEBUG("Incoming call missed");
        hfp_ag_terminate_call();
        return Result::Code::Success;
    }

    auto HFP::HFPImpl::setIncomingCallNumber(const std::string &num) const noexcept -> Result::Code
    {
        LOG_SENSITIVE(LOGDEBUG, "Setting number: %s", num.c_str());
        hfp_ag_set_clip(129, num.c_str());
        return Result::Code::Success;
    }

    auto HFP::HFPImpl::setSignalStrength(int bars) const noexcept -> Result::Code
    {
        const auto result = hfp_ag_set_signal_strength(bars);
        LOG_INFO("Set RSSI bars: %d/4, result: 0x%02X", bars, result);
        return Result::Code::Success;
    }

    auto HFP::HFPImpl::setOperatorName(const std::string_view &name) const noexcept -> Result::Code
    {
        const auto result = hfp_ag_set_operator_name(aclHandle, name.data());
        LOG_INFO("Set operator name: %s, result: 0x%02X", name.data(), result);
        return Result::Code::Success;
    }

    auto HFP::HFPImpl::setBatteryLevel(const BatteryLevel &level) const noexcept -> Result::Code
    {
        const auto batteryLevel = level.getBatteryLevelBars();
        const auto result       = hfp_ag_set_battery_level(static_cast<int>(batteryLevel));
        LOG_INFO("Set battery level: %d, result: 0x%02X", batteryLevel, result);
        return Result::Code::Success;
    }

    auto HFP::HFPImpl::setNetworkRegistrationStatus(bool registered) const noexcept -> Result::Code
    {
        const auto result = hfp_ag_set_registration_status(static_cast<int>(registered));
        LOG_INFO("Set registration status: %s, result 0x%02X", registered ? "online" : "offline", result);
        return Result::Code::Success;
    }

    auto HFP::HFPImpl::setRoamingStatus(bool enabled) const noexcept -> Result::Code
    {
        const auto result = hfp_ag_set_roaming_status(static_cast<int>(enabled));
        LOG_INFO("Set roaming status: %s, result: 0x%02X", enabled ? "enabled" : "disabled", result);
        return Result::Code::Success;
    }

    void HFP::HFPImpl::deInit() noexcept
    {
        sdp_unregister_service(hfpSdpRecordHandle);
        cellularInterface.reset();
        audioInterface.reset();
        sco.reset();

        LOG_INFO("HFP deinitialized!");
    }
} // namespace bluetooth
