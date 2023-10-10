// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "A2DP.hpp"
#include "A2DPImpl.hpp"
#include "AVDTP.hpp"
#include "AVRCP.hpp"
#include <Bluetooth/Device.hpp>
#include <Bluetooth/Result.hpp>
#include <log/log.hpp>
#include <Audio/AudioCommon.hpp>
#include <service-audio/AudioMessage.hpp>
#include <service-evtmgr/ServiceEventManagerName.hpp>
#include "service-bluetooth/messages/Connect.hpp"
#include "service-bluetooth/messages/Disconnect.hpp"
#include "service-bluetooth/ServiceBluetoothName.hpp"
#include <audio/BluetoothAudioDevice.hpp>

extern "C"
{
#include "module-bluetooth/lib/btstack/src/btstack.h"
#include <btstack_defines.h>
}

#define ARRAY_LENGTH(x) (sizeof(x) / sizeof((x)[0]))

namespace bluetooth
{
    A2DP::A2DP() : pimpl(std::make_unique<A2DPImpl>(A2DPImpl()))
    {}

    A2DP::~A2DP()
    {
        pimpl->disconnect();
        pimpl->deInit();
    }

    A2DP::A2DP(A2DP &other) : pimpl(new A2DPImpl(*other.pimpl))
    {}

    auto A2DP::operator=(A2DP rhs) -> A2DP &
    {
        swap(pimpl, rhs.pimpl);
        return *this;
    }

    A2DP::A2DP(A2DP &&other) noexcept : pimpl(std::move(other.pimpl))
    {
        other.pimpl = nullptr;
    }

    A2DP &A2DP::operator=(A2DP &&other) noexcept
    {
        if (&other == this) {
            return *this;
        }

        pimpl       = std::move(other.pimpl);
        other.pimpl = nullptr;

        return *this;
    }

    auto A2DP::init() -> Result::Code
    {
        return pimpl->init();
    }

    void A2DP::setDevice(const Devicei &device)
    {
        pimpl->setDevice(device);
    }

    void A2DP::setOwnerService(sys::Service *service)
    {
        pimpl->setOwnerService(service);
    }

    void A2DP::connect()
    {
        pimpl->connect();
    }

    void A2DP::disconnect()
    {
        pimpl->disconnect();
    }

    void A2DP::start()
    {
        pimpl->start();
    }

    void A2DP::stop()
    {
        pimpl->stop();
    }

    void A2DP::setAudioDevice(std::shared_ptr<bluetooth::BluetoothAudioDevice> audioDevice)
    {
        pimpl->setAudioDevice(std::move(audioDevice));
    }

    std::uint8_t A2DP::A2DPImpl::sdpSourceServiceBuffer[sourceServiceBufferSize];
    std::uint8_t A2DP::A2DPImpl::mediaSbcCodecCapabilities[] = {
        (AVDTP_SBC_44100 << 4) | AVDTP_SBC_STEREO,
        0xFF, //(AVDTP_SBC_BLOCK_LENGTH_16 << 4) | (AVDTP_SBC_SUBBANDS_8 << 2) | AVDTP_SBC_ALLOCATION_METHOD_LOUDNESS,
        2,
        53};

    std::shared_ptr<BluetoothAudioDevice> A2DP::A2DPImpl::audioDevice;
    sys::Service *A2DP::A2DPImpl::ownerService = nullptr;
    Devicei A2DP::A2DPImpl::device;

    btstack_packet_callback_registration_t A2DP::A2DPImpl::hciEventCallbackRegistration;

    QueueHandle_t A2DP::A2DPImpl::sourceQueue = nullptr;

    bool A2DP::A2DPImpl::isConnected = false;

    auto A2DP::A2DPImpl::init() -> Result::Code
    {
        // request role change on reconnecting headset to always use them in slave mode
        hci_set_master_slave_policy(0);

        Profile::initL2cap();

        a2dp_source_init();
        a2dp_source_register_packet_handler(&sourcePacketHandler);

        // Create stream endpoint.
        const auto local_stream_endpoint = a2dp_source_create_stream_endpoint(AVDTP_AUDIO,
                                                                              AVDTP_CODEC_SBC,
                                                                              mediaSbcCodecCapabilities,
                                                                              ARRAY_LENGTH(mediaSbcCodecCapabilities),
                                                                              AVDTP::sbcCodecConfiguration.data(),
                                                                              AVDTP::sbcCodecConfiguration.size());
        if (local_stream_endpoint == nullptr) {
            LOG_INFO("A2DP Source: not enough memory to create local stream endpoint");
            return Result::Code::SystemError;
        }
        AVRCP::mediaTracker.local_seid = avdtp_local_seid(local_stream_endpoint);
        avdtp_source_register_delay_reporting_category(AVRCP::mediaTracker.local_seid);

        AVRCP::init(ownerService);

        Profile::initSdp();

        std::memset(sdpSourceServiceBuffer, 0, sizeof(sdpSourceServiceBuffer));

        a2dp_source_create_sdp_record(
            sdpSourceServiceBuffer, a2dpSdpRecordHandle, AVDTP_SOURCE_FEATURE_MASK_PLAYER, nullptr, nullptr);
        if (const auto status = sdp_register_service(sdpSourceServiceBuffer); status != ERROR_CODE_SUCCESS) {
            LOG_ERROR("Can't register service, status 0x%02X", status);
        }

        std::memset(AVRCP::sdpTargetServiceBuffer, 0, sizeof(AVRCP::sdpTargetServiceBuffer));

        std::uint16_t supportedFeatures = AVRCP_FEATURE_MASK_CATEGORY_PLAYER_OR_RECORDER;
#ifdef AVRCP_BROWSING_ENABLED
        supported_features |= AVRCP_FEATURE_MASK_BROWSING;
#endif
        avrcp_target_create_sdp_record(
            AVRCP::sdpTargetServiceBuffer, avrcpServiceSdpRecordHandle, supportedFeatures, nullptr, nullptr);
        if (const auto status = sdp_register_service(AVRCP::sdpTargetServiceBuffer); status != ERROR_CODE_SUCCESS) {
            LOG_ERROR("Can't register service, status 0x%02X", status);
        }

        std::memset(AVRCP::sdpControllerServiceBuffer, 0, sizeof(AVRCP::sdpControllerServiceBuffer));

        const std::uint16_t controllerSupportedFeatures = AVRCP_FEATURE_MASK_CATEGORY_MONITOR_OR_AMPLIFIER;

        avrcp_controller_create_sdp_record(AVRCP::sdpControllerServiceBuffer,
                                           avrcpControllerSdpRecordHandle,
                                           controllerSupportedFeatures,
                                           nullptr,
                                           nullptr);
        if (const auto status = sdp_register_service(AVRCP::sdpControllerServiceBuffer); status != ERROR_CODE_SUCCESS) {
            LOG_ERROR("Can't register service. Status %x", status);
        }

        hciEventCallbackRegistration.callback = &hciPacketHandler;
        hci_add_event_handler(&hciEventCallbackRegistration);

        LOG_INFO("A2DP initialized!");
        return Result::Code::Success;
    }

    void A2DP::A2DPImpl::sendMediaPacket()
    {
        const auto numBytesInFrame   = btstack_sbc_encoder_sbc_buffer_length();
        const auto bytesInStorage    = AVRCP::mediaTracker.sbc_storage_count;
        const std::uint8_t numFrames = bytesInStorage / numBytesInFrame;

        a2dp_source_stream_send_media_payload(AVRCP::mediaTracker.a2dp_cid,
                                              AVRCP::mediaTracker.local_seid,
                                              AVRCP::mediaTracker.sbc_storage,
                                              bytesInStorage,
                                              numFrames,
                                              0);

        AVRCP::mediaTracker.sbc_storage_count = 0;
        AVRCP::mediaTracker.sbc_ready_to_send = 0;
    }

    void A2DP::A2DPImpl::audioTimeoutHandler(btstack_timer_source_t *timer)
    {
        const auto context = static_cast<MediaContext *>(btstack_run_loop_get_timer_context(timer));
        btstack_run_loop_set_timer(&context->audio_timer, audioTimeoutMs);
        btstack_run_loop_add_timer(&context->audio_timer);
        const auto now = btstack_run_loop_get_time_ms();

        std::uint32_t updatePeriodMs = audioTimeoutMs;
        if (context->time_audio_data_sent_in_ms > 0) {
            updatePeriodMs = now - context->time_audio_data_sent_in_ms;
        }

        std::uint32_t numSamples = (updatePeriodMs * AVDTP::sampleRate) / 1000;
        context->acc_num_missed_samples += (updatePeriodMs * AVDTP::sampleRate) % 1000;

        while (context->acc_num_missed_samples >= 1000) {
            numSamples++;
            context->acc_num_missed_samples -= 1000;
        }
        context->time_audio_data_sent_in_ms = now;
        context->samples_ready += numSamples;

        if (context->sbc_ready_to_send != 0) {
            return;
        }

        if (audioDevice != nullptr) {
            audioDevice->onDataSend();
        }

        if ((context->sbc_storage_count + btstack_sbc_encoder_sbc_buffer_length()) > context->max_media_payload_size) {
            context->sbc_ready_to_send = 1; // schedule sending
            a2dp_source_stream_endpoint_request_can_send_now(context->a2dp_cid, context->local_seid);
        }
    }

    void A2DP::A2DPImpl::startTimer(MediaContext *context)
    {
        LOG_DEBUG("A2DP timer start");

        context->max_media_payload_size =
            btstack_min(a2dp_max_media_payload_size(context->a2dp_cid, context->local_seid), SBC_STORAGE_SIZE);
        context->sbc_storage_count = 0;
        context->sbc_ready_to_send = 0;
        context->streaming         = 1;

        btstack_run_loop_remove_timer(&context->audio_timer);
        btstack_run_loop_set_timer_handler(&context->audio_timer, audioTimeoutHandler);
        btstack_run_loop_set_timer_context(&context->audio_timer, context);
        btstack_run_loop_set_timer(&context->audio_timer, audioTimeoutMs);
        btstack_run_loop_add_timer(&context->audio_timer);
    }

    void A2DP::A2DPImpl::stopTimer(MediaContext *context)
    {
        LOG_DEBUG("A2DP timer stop");

        context->time_audio_data_sent_in_ms = 0;
        context->acc_num_missed_samples     = 0;
        context->samples_ready              = 0;
        context->streaming                  = 1;
        context->sbc_storage_count          = 0;
        context->sbc_ready_to_send          = 0;

        btstack_run_loop_remove_timer(&context->audio_timer);
    }

    void A2DP::A2DPImpl::hciPacketHandler([[maybe_unused]] uint8_t packetType,
                                          [[maybe_unused]] uint16_t channel,
                                          [[maybe_unused]] uint8_t *packet,
                                          [[maybe_unused]] uint16_t size)
    {}

    void A2DP::A2DPImpl::sourcePacketHandler(uint8_t packetType,
                                             [[maybe_unused]] uint16_t channel,
                                             uint8_t *packet,
                                             [[maybe_unused]] uint16_t size)
    {
        std::uint8_t status;
        std::uint8_t local_seid;
        bd_addr_t address;
        std::uint16_t cid;

        if (packetType != HCI_EVENT_PACKET) {
            return;
        }

        if (hci_event_packet_get_type(packet) != HCI_EVENT_A2DP_META) {
            return;
        }

        switch (hci_event_a2dp_meta_get_subevent_code(packet)) {
        case A2DP_SUBEVENT_SIGNALING_CONNECTION_ESTABLISHED: {
            a2dp_subevent_signaling_connection_established_get_bd_addr(packet, address);
            cid    = a2dp_subevent_signaling_connection_established_get_a2dp_cid(packet);
            status = a2dp_subevent_signaling_connection_established_get_status(packet);

            if (status != ERROR_CODE_SUCCESS) {
                LOG_INFO("Connection failed, status 0x%02X, cid 0x%02X, a2dp_cid 0x%02X",
                         status,
                         cid,
                         AVRCP::mediaTracker.a2dp_cid);
                AVRCP::mediaTracker.a2dp_cid = 0;
                ownerService->bus.sendUnicast(std::make_shared<message::bluetooth::ConnectResult>(
                                                  device, message::bluetooth::ConnectResult::Result::Failure),
                                              service::name::bluetooth);
                break;
            }

            AVRCP::mediaTracker.a2dp_cid = cid;
            AVRCP::mediaTracker.volume   = 64;

            LOG_INFO("Connected, a2dp_cid 0x%02X, local seid 0x%02X",
                     AVRCP::mediaTracker.a2dp_cid,
                     AVRCP::mediaTracker.local_seid);
            device.deviceState = DeviceState::ConnectedAudio;
            isConnected        = true;

            /* Handle proper device matching when connection was initiated by remote device */
            a2dp_subevent_signaling_connection_established_get_bd_addr(packet, device.address);

            ownerService->bus.sendUnicast(std::make_shared<message::bluetooth::ConnectResult>(
                                              device, message::bluetooth::ConnectResult::Result::Success),
                                          service::name::bluetooth);
            break;
        }

        case A2DP_SUBEVENT_SIGNALING_MEDIA_CODEC_SBC_CONFIGURATION: {
            cid = avdtp_subevent_signaling_media_codec_sbc_configuration_get_avdtp_cid(packet);
            if (cid != AVRCP::mediaTracker.a2dp_cid) {
                return;
            }

            AVRCP::mediaTracker.remote_seid =
                a2dp_subevent_signaling_media_codec_sbc_configuration_get_remote_seid(packet);

            AVDTP::sbcConfig.reconfigure =
                a2dp_subevent_signaling_media_codec_sbc_configuration_get_reconfigure(packet);
            AVDTP::sbcConfig.numChannels =
                a2dp_subevent_signaling_media_codec_sbc_configuration_get_num_channels(packet);
            AVDTP::sbcConfig.samplingFrequency =
                a2dp_subevent_signaling_media_codec_sbc_configuration_get_sampling_frequency(packet);
            AVDTP::sbcConfig.blockLength =
                a2dp_subevent_signaling_media_codec_sbc_configuration_get_block_length(packet);
            AVDTP::sbcConfig.subbands = a2dp_subevent_signaling_media_codec_sbc_configuration_get_subbands(packet);
            AVDTP::sbcConfig.minBitpoolValue =
                a2dp_subevent_signaling_media_codec_sbc_configuration_get_min_bitpool_value(packet);
            AVDTP::sbcConfig.maxBitpoolValue =
                a2dp_subevent_signaling_media_codec_sbc_configuration_get_max_bitpool_value(packet);

            LOG_INFO("Received SBC codec configuration, sampling frequency %uHz, a2dp_cid 0x%02X, local seid 0x%02X, "
                     "remote seid 0x%02X",
                     AVDTP::sbcConfig.samplingFrequency,
                     cid,
                     AVRCP::mediaTracker.local_seid,
                     AVRCP::mediaTracker.remote_seid);

            // Adapt Bluetooth spec definition to SBC Encoder expected input
            const auto allocation_method =
                a2dp_subevent_signaling_media_codec_sbc_configuration_get_allocation_method(packet);
            AVDTP::sbcConfig.allocationMethod = static_cast<btstack_sbc_allocation_method_t>(allocation_method - 1);
            AVDTP::sbcConfig.numChannels = 2;

            const auto channel_mode = static_cast<avdtp_channel_mode_t>(
                a2dp_subevent_signaling_media_codec_sbc_configuration_get_channel_mode(packet));
            switch (channel_mode) {
            case AVDTP_CHANNEL_MODE_JOINT_STEREO:
                AVDTP::sbcConfig.channelMode = SBC_CHANNEL_MODE_JOINT_STEREO;
                break;

            case AVDTP_CHANNEL_MODE_STEREO:
                AVDTP::sbcConfig.channelMode = SBC_CHANNEL_MODE_STEREO;
                break;

            case AVDTP_CHANNEL_MODE_DUAL_CHANNEL:
                AVDTP::sbcConfig.channelMode = SBC_CHANNEL_MODE_DUAL_CHANNEL;
                break;

            case AVDTP_CHANNEL_MODE_MONO:
                AVDTP::sbcConfig.channelMode = SBC_CHANNEL_MODE_MONO;
                break;
            }
            AVDTP::dumpSbcConfiguration();

            btstack_sbc_encoder_init(&AVDTP::sbcEncoderState,
                                     SBC_MODE_STANDARD,
                                     AVDTP::sbcConfig.blockLength,
                                     AVDTP::sbcConfig.subbands,
                                     AVDTP::sbcConfig.allocationMethod,
                                     AVDTP::sbcConfig.samplingFrequency,
                                     AVDTP::sbcConfig.maxBitpoolValue,
                                     AVDTP::sbcConfig.channelMode);
        } break;

        case A2DP_SUBEVENT_SIGNALING_DELAY_REPORTING_CAPABILITY: {
            LOG_INFO("Remote supports delay report, remote seid 0x%02X",
                     avdtp_subevent_signaling_delay_reporting_capability_get_remote_seid(packet));
        } break;

        case A2DP_SUBEVENT_SIGNALING_CAPABILITIES_DONE: {
            LOG_INFO("All capabilities reported, remote seid 0x%02X",
                     avdtp_subevent_signaling_capabilities_done_get_remote_seid(packet));
        } break;

        case A2DP_SUBEVENT_SIGNALING_DELAY_REPORT: {
            LOG_INFO("Received delay report of %d.%0d ms, local seid 0x%02X",
                     avdtp_subevent_signaling_delay_report_get_delay_100us(packet) / 10,
                     avdtp_subevent_signaling_delay_report_get_delay_100us(packet) % 10,
                     avdtp_subevent_signaling_delay_report_get_local_seid(packet));
        } break;

        case A2DP_SUBEVENT_STREAM_ESTABLISHED: {
            a2dp_subevent_stream_established_get_bd_addr(packet, address);
            status = a2dp_subevent_stream_established_get_status(packet);
            if (status != ERROR_CODE_SUCCESS) {
                LOG_INFO("Stream failed, status 0x%02X", status);
                break;
            }

            local_seid = a2dp_subevent_stream_established_get_local_seid(packet);
            cid        = a2dp_subevent_stream_established_get_a2dp_cid(packet);

            LOG_INFO("A2DP subevent stream established: a2dp_cid (expected 0x%02X, received 0x%02X), local_seid "
                     "(expected 0x%02X, received 0x%02X), remote_seid (expected 0x%02X, received 0x%02X)",
                     AVRCP::mediaTracker.a2dp_cid,
                     cid,
                     AVRCP::mediaTracker.local_seid,
                     local_seid,
                     a2dp_subevent_stream_established_get_remote_seid(packet),
                     AVRCP::mediaTracker.remote_seid);

            if (local_seid != AVRCP::mediaTracker.local_seid) {
                LOG_INFO("Stream failed, wrong local seid 0x%02X, expected 0x%02X",
                         local_seid,
                         AVRCP::mediaTracker.local_seid);
                break;
            }

            LOG_INFO("Stream established, a2dp_cid 0x%02X, local seid 0x%02X, remote seid 0x%02X",
                     AVRCP::mediaTracker.a2dp_cid,
                     AVRCP::mediaTracker.local_seid,
                     a2dp_subevent_stream_established_get_remote_seid(packet));

            sourceQueue = xQueueCreate(sourceQueueLength, sizeof(AudioData_t));
            if (sourceQueue == nullptr) {
                LOG_ERROR("Failed to create sourceQueue!");
                break;
            }

            sendAudioEvent(audio::EventType::BluetoothA2DPDeviceState, audio::Event::DeviceState::Connected);
        } break;

        case A2DP_SUBEVENT_STREAM_RECONFIGURED: {
            local_seid = a2dp_subevent_stream_reconfigured_get_local_seid(packet);
            cid        = a2dp_subevent_stream_reconfigured_get_a2dp_cid(packet);
            status     = a2dp_subevent_stream_reconfigured_get_status(packet);

            LOG_INFO("Reconfigured, a2dp_cid [expected 0x%02X, received 0x%02X], local_seid [expected 0x%02X, received "
                     "0x%02X], status 0x%02X",
                     AVRCP::mediaTracker.a2dp_cid,
                     cid,
                     AVRCP::mediaTracker.local_seid,
                     local_seid,
                     status);
        } break;

        case A2DP_SUBEVENT_STREAM_STARTED: {
            local_seid = a2dp_subevent_stream_started_get_local_seid(packet);
            cid        = a2dp_subevent_stream_started_get_a2dp_cid(packet);

            AVRCP::playInfo.status = AVRCP_PLAYBACK_STATUS_PLAYING;
            if (AVRCP::mediaTracker.avrcp_cid != 0) {
                avrcp_target_set_playback_status(AVRCP::mediaTracker.avrcp_cid, AVRCP::playInfo.status);
            }
            startTimer(&AVRCP::mediaTracker);

            LOG_INFO("Stream started: a2dp_cid [expected 0x%02X, received 0x%02X], local_seid [expected 0x%02X, "
                     "received 0x%02X]",
                     AVRCP::mediaTracker.a2dp_cid,
                     cid,
                     AVRCP::mediaTracker.local_seid,
                     local_seid);
        } break;

        case A2DP_SUBEVENT_STREAMING_CAN_SEND_MEDIA_PACKET_NOW: {
            sendMediaPacket();
        } break;

        case A2DP_SUBEVENT_STREAM_SUSPENDED: {
            local_seid = a2dp_subevent_stream_suspended_get_local_seid(packet);
            cid        = a2dp_subevent_stream_suspended_get_a2dp_cid(packet);

            AVRCP::playInfo.status = AVRCP_PLAYBACK_STATUS_PAUSED;
            if (AVRCP::mediaTracker.avrcp_cid != 0) {
                avrcp_target_set_playback_status(AVRCP::mediaTracker.avrcp_cid, AVRCP::playInfo.status);
            }

            LOG_INFO("Stream paused: a2dp_cid [expected 0x%02X, received 0x%02X], local_seid [expected 0x%02X, "
                     "received 0x%02X]",
                     AVRCP::mediaTracker.a2dp_cid,
                     cid,
                     AVRCP::mediaTracker.local_seid,
                     local_seid);

            stopTimer(&AVRCP::mediaTracker);
        } break;

        case A2DP_SUBEVENT_STREAM_RELEASED: {
            cid                    = a2dp_subevent_stream_released_get_a2dp_cid(packet);
            local_seid             = a2dp_subevent_stream_released_get_local_seid(packet);

            AVRCP::playInfo.status = AVRCP_PLAYBACK_STATUS_STOPPED;

            LOG_INFO("Stream released: a2dp_cid [expected 0x%02X, received 0x%02X], local_seid [expected 0x%02X, "
                     "received 0x%02X]",
                     AVRCP::mediaTracker.a2dp_cid,
                     cid,
                     AVRCP::mediaTracker.local_seid,
                     local_seid);

            sendAudioEvent(audio::EventType::BluetoothA2DPDeviceState, audio::Event::DeviceState::Disconnected);

            if (cid == AVRCP::mediaTracker.a2dp_cid) {
                AVRCP::mediaTracker.stream_opened = 0;
                LOG_INFO("Stream released");
            }

            if (AVRCP::mediaTracker.avrcp_cid != 0) {
                avrcp_target_set_playback_status(AVRCP::mediaTracker.avrcp_cid, AVRCP_PLAYBACK_STATUS_STOPPED);
            }

            ownerService->bus.sendUnicast(std::make_shared<message::bluetooth::DisconnectResult>(device),
                                          service::name::bluetooth);

            stopTimer(&AVRCP::mediaTracker);
            break;
        }
        case A2DP_SUBEVENT_SIGNALING_CONNECTION_RELEASED: {
            cid = a2dp_subevent_signaling_connection_released_get_a2dp_cid(packet);

            if (cid == AVRCP::mediaTracker.a2dp_cid) {
                AVRCP::mediaTracker.avrcp_cid = 0;
                AVRCP::mediaTracker.a2dp_cid  = 0;
                LOG_INFO("Signaling connection released");
            }

            isConnected = false;
        } break;

        default:
            LOG_DEBUG("Event not handled: 0x%02X", hci_event_a2dp_meta_get_subevent_code(packet));
            break;
        }
    }

    void A2DP::A2DPImpl::connect()
    {
        if (isConnected) {
            disconnect();
        }

        LOG_INFO("Connecting A2DP profile");
        a2dp_source_establish_stream(device.address, &AVRCP::mediaTracker.a2dp_cid);
    }

    void A2DP::A2DPImpl::disconnect()
    {
        LOG_INFO("Disconnecting A2DP profile");
        a2dp_source_disconnect(AVRCP::mediaTracker.a2dp_cid);
        ownerService->bus.sendUnicast(std::make_shared<message::bluetooth::DisconnectResult>(device),
                                      service::name::bluetooth);
    }

    void A2DP::A2DPImpl::setDevice(const Devicei &dev)
    {
        device = dev;
        LOG_INFO("Device set!");
    }

    void A2DP::A2DPImpl::setOwnerService(sys::Service *service)
    {
        ownerService = service;
    }

    void A2DP::A2DPImpl::sendAudioEvent(audio::EventType event, audio::Event::DeviceState state)
    {
        auto evt       = std::make_shared<audio::Event>(event, state);
        auto msg       = std::make_shared<AudioEventRequest>(std::move(evt));
        ownerService->bus.sendUnicast(std::move(msg), service::name::evt_manager);
    }

    void A2DP::A2DPImpl::start()
    {
        if (!isConnected) {
            connect();
        }
        AVRCP::mediaTracker.stream_opened = 1;
        a2dp_source_start_stream(AVRCP::mediaTracker.a2dp_cid, AVRCP::mediaTracker.local_seid);
    }

    void A2DP::A2DPImpl::stop()
    {
        AVRCP::mediaTracker.stream_opened = 1;
        a2dp_source_pause_stream(AVRCP::mediaTracker.a2dp_cid, AVRCP::mediaTracker.local_seid);
    }

    void A2DP::A2DPImpl::setAudioDevice(std::shared_ptr<bluetooth::BluetoothAudioDevice> newAudioDevice)
    {
        A2DP::A2DPImpl::audioDevice = std::move(newAudioDevice);
    }

    void A2DP::A2DPImpl::deInit()
    {
        sdp_unregister_service(a2dpSdpRecordHandle);
        sdp_unregister_service(avrcpControllerSdpRecordHandle);
        sdp_unregister_service(avrcpServiceSdpRecordHandle);
        audioDevice.reset();

        LOG_DEBUG("A2DP deinitialized!");
    }
} // namespace bluetooth
