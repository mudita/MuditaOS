// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

//
// Created by bartek on 14.09.2020.
//

#include "A2DP.hpp"
#include "A2DPImpl.hpp"
#include "AVDTP.hpp"
#include "AVRCP.hpp"
#include <Bluetooth/Device.hpp>
#include <Bluetooth/Error.hpp>
#include <log/log.hpp>
#include <service-bluetooth/BluetoothMessage.hpp>
#include <Audio/AudioCommon.hpp>
#include <service-audio/AudioMessage.hpp>
#include <service-evtmgr/Constants.hpp>
#include "service-bluetooth/messages/Connect.hpp"
#include "service-bluetooth/messages/Disconnect.hpp"
#include "service-bluetooth/Constants.hpp"
#include <audio/BluetoothAudioDevice.hpp>

extern "C"
{
#include "module-bluetooth/lib/btstack/src/btstack.h"
#include <btstack_defines.h>
}

namespace bluetooth
{
    A2DP::A2DP() : pimpl(std::make_unique<A2DPImpl>(A2DPImpl()))
    {}

    A2DP::~A2DP() = default;

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
        if (&other == this)
            return *this;

        pimpl       = std::move(other.pimpl);
        other.pimpl = nullptr;

        return *this;
    }

    auto A2DP::init() -> Error::Code
    {
        return pimpl->init();
    }

    void A2DP::setDevice(const Devicei &device)
    {
        pimpl->setDevice(device);
    }

    void A2DP::setOwnerService(const sys::Service *service)
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

    auto A2DP::startRinging() const noexcept -> Error::Code
    {
        LOG_ERROR("Can't ring in A2DP profile");
        return Error::SystemError;
    }

    auto A2DP::stopRinging() const noexcept -> Error::Code
    {
        return Error::SystemError;
    }

    auto A2DP::initializeCall() const noexcept -> Error::Code
    {
        return Error::SystemError;
    }

    void A2DP::setAudioDevice(std::shared_ptr<bluetooth::BluetoothAudioDevice> audioDevice)
    {
        pimpl->setAudioDevice(std::move(audioDevice));
    }
    auto A2DP::callTerminated() const noexcept -> Error::Code
    {
        return Error::SystemError;
    }
    auto A2DP::callAnswered() const noexcept -> Error::Code
    {
        return Error::SystemError;
    }
    auto A2DP::setIncomingCallNumber(const std::string &num) const noexcept -> Error::Code
    {
        LOG_INFO("Setting number in A2DP - ignoring");
        return Error::Success;
    }

    const sys::Service *A2DP::A2DPImpl::ownerService;
    QueueHandle_t A2DP::A2DPImpl::sourceQueue = nullptr;
    QueueHandle_t A2DP::A2DPImpl::sinkQueue   = nullptr;
    DeviceMetadata_t A2DP::A2DPImpl::metadata;
    btstack_packet_callback_registration_t A2DP::A2DPImpl::hciEventCallbackRegistration;
    std::array<uint8_t, 150> A2DP::A2DPImpl::sdpSourceServiceBuffer;
    std::array<uint8_t, 4> A2DP::A2DPImpl::mediaSbcCodecCapabilities = {
        (AVDTP_SBC_44100 << 4) | AVDTP_SBC_STEREO,
        0xFF, //(AVDTP_SBC_BLOCK_LENGTH_16 << 4) | (AVDTP_SBC_SUBBANDS_8 << 2) | AVDTP_SBC_ALLOCATION_METHOD_LOUDNESS,
        2,
        53};
    std::shared_ptr<BluetoothAudioDevice> A2DP::A2DPImpl::audioDevice;

    bool A2DP::A2DPImpl::isConnected = false;
    Devicei A2DP::A2DPImpl::device;
    /* LISTING_START(MainConfiguration): Setup Audio Source and AVRCP Target services */

    auto A2DP::A2DPImpl::init() -> Error::Code
    {
        // request role change on reconnecting headset to always use them in slave mode
        hci_set_master_slave_policy(0);

        Profile::initL2cap();
        // Initialize  A2DP Source.
        a2dp_source_init();
        a2dp_source_register_packet_handler(&sourcePacketHandler);

        // Create stream endpoint.
        avdtp_stream_endpoint_t *local_stream_endpoint =
            a2dp_source_create_stream_endpoint(AVDTP_AUDIO,
                                               AVDTP_CODEC_SBC,
                                               mediaSbcCodecCapabilities.data(),
                                               mediaSbcCodecCapabilities.size(),
                                               AVDTP::sbcCodecConfiguration.data(),
                                               AVDTP::sbcCodecConfiguration.size());
        if (local_stream_endpoint == nullptr) {
            LOG_INFO("A2DP Source: not enough memory to create local stream endpoint\n");
            return bluetooth::Error::SystemError;
        }
        AVRCP::mediaTracker.local_seid = avdtp_local_seid(local_stream_endpoint);
        avdtp_source_register_delay_reporting_category(AVRCP::mediaTracker.local_seid);

        AVRCP::init(const_cast<sys::Service *>(ownerService));

        // Initialize SDP
        Profile::initSdp();

        // Create  A2DP Source service record and register it with SDP.
        sdpSourceServiceBuffer.fill(0);
        constexpr uint32_t a2dpSdpRecordHandle = 0x10001;
        a2dp_source_create_sdp_record(
            sdpSourceServiceBuffer.data(), a2dpSdpRecordHandle, AVDTP_SOURCE_FEATURE_MASK_PLAYER, nullptr, nullptr);
        if (const auto status = sdp_register_service(sdpSourceServiceBuffer.data()); status != ERROR_CODE_SUCCESS) {
            LOG_ERROR("Can't register service. Status %x", status);
        }

        // Create AVRCP target service record and register it with SDP.
        AVRCP::sdpTargetServiceBuffer.fill(0);
        uint16_t supportedFeatures = AVRCP_FEATURE_MASK_CATEGORY_PLAYER_OR_RECORDER;
#ifdef AVRCP_BROWSING_ENABLED
        supported_features |= AVRCP_FEATURE_MASK_BROWSING;
#endif
        constexpr uint32_t avrcpServiceSdpRecordHandle = 0x10002;
        avrcp_target_create_sdp_record(
            AVRCP::sdpTargetServiceBuffer.data(), avrcpServiceSdpRecordHandle, supportedFeatures, nullptr, nullptr);
        if (const auto status = sdp_register_service(AVRCP::sdpTargetServiceBuffer.data());
            status != ERROR_CODE_SUCCESS) {
            LOG_ERROR("Can't register service. Status %x", status);
        }

        // setup AVRCP Controller
        AVRCP::sdpControllerServiceBuffer.fill(0);
        uint16_t controllerSupportedFeatures              = AVRCP_FEATURE_MASK_CATEGORY_PLAYER_OR_RECORDER;
        constexpr uint32_t avrcpControllerSdpRecordHandle = 0x10003;
        avrcp_controller_create_sdp_record(AVRCP::sdpControllerServiceBuffer.data(),
                                           avrcpControllerSdpRecordHandle,
                                           controllerSupportedFeatures,
                                           nullptr,
                                           nullptr);
        if (const auto status = sdp_register_service(AVRCP::sdpControllerServiceBuffer.data());
            status != ERROR_CODE_SUCCESS) {
            LOG_ERROR("Can't register service. Status %x", status);
        }

        // Set local name with a template Bluetooth address, that will be automatically
        // replaced with a actual address once it is available, i.e. when BTstack boots
        // up and starts talking to a Bluetooth module.
        gap_set_class_of_device(0x200408);

        // Register for HCI events.
        hciEventCallbackRegistration.callback = &hciPacketHandler;
        hci_add_event_handler(&hciEventCallbackRegistration);

        LOG_INFO("Init done!");

        return bluetooth::Error::Success;
    }

    void A2DP::A2DPImpl::sendMediaPacket()
    {
        int numBytesInFrame = btstack_sbc_encoder_sbc_buffer_length();
        int bytesInStorage  = AVRCP::mediaTracker.sbc_storage_count;
        uint8_t numFrames   = bytesInStorage / numBytesInFrame;
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
        auto *context = static_cast<MediaContext *>(btstack_run_loop_get_timer_context(timer));
        btstack_run_loop_set_timer(&context->audio_timer, AUDIO_TIMEOUT_MS);
        btstack_run_loop_add_timer(&context->audio_timer);
        uint32_t now = btstack_run_loop_get_time_ms();

        uint32_t updatePeriodMs = AUDIO_TIMEOUT_MS;
        if (context->time_audio_data_sent_in_ms > 0) {
            updatePeriodMs = now - context->time_audio_data_sent_in_ms;
        }

        uint32_t numSamples = (updatePeriodMs * AVDTP::sampleRate) / 1000;
        context->acc_num_missed_samples += (updatePeriodMs * AVDTP::sampleRate) % 1000;

        while (context->acc_num_missed_samples >= 1000) {
            numSamples++;
            context->acc_num_missed_samples -= 1000;
        }
        context->time_audio_data_sent_in_ms = now;
        context->samples_ready += numSamples;

        if (context->sbc_ready_to_send != 0u) {
            return;
        }

        if (audioDevice != nullptr) {
            audioDevice->onDataSend();
        }

        if ((context->sbc_storage_count + btstack_sbc_encoder_sbc_buffer_length()) > context->max_media_payload_size) {
            // schedule sending
            context->sbc_ready_to_send = 1;
            a2dp_source_stream_endpoint_request_can_send_now(context->a2dp_cid, context->local_seid);
        }
    }

    void A2DP::A2DPImpl::startTimer(MediaContext *context)
    {
        LOG_DEBUG("Timer start");

        context->max_media_payload_size =
            btstack_min(a2dp_max_media_payload_size(context->a2dp_cid, context->local_seid), SBC_STORAGE_SIZE);
        context->sbc_storage_count = 0;
        context->sbc_ready_to_send = 0;
        context->streaming         = 1;
        btstack_run_loop_remove_timer(&context->audio_timer);
        btstack_run_loop_set_timer_handler(&context->audio_timer, audioTimeoutHandler);
        btstack_run_loop_set_timer_context(&context->audio_timer, context);
        btstack_run_loop_set_timer(&context->audio_timer, A2DP::A2DPImpl::AUDIO_TIMEOUT_MS);
        btstack_run_loop_add_timer(&context->audio_timer);
    }

    void A2DP::A2DPImpl::stopTimer(MediaContext *context)
    {
        LOG_DEBUG("Timer stop");

        context->time_audio_data_sent_in_ms = 0;
        context->acc_num_missed_samples     = 0;
        context->samples_ready              = 0;
        context->streaming                  = 1;
        context->sbc_storage_count          = 0;
        context->sbc_ready_to_send          = 0;
        btstack_run_loop_remove_timer(&context->audio_timer);
    }

    void A2DP::A2DPImpl::hciPacketHandler(uint8_t packetType, uint16_t channel, uint8_t *packet, uint16_t size)
    {
        if (packetType != HCI_EVENT_PACKET) {
            return;
        }
    }

    void A2DP::A2DPImpl::sourcePacketHandler(uint8_t packetType, uint16_t channel, uint8_t *packet, uint16_t size)
    {
        uint8_t status;
        uint8_t local_seid;
        bd_addr_t address;
        uint16_t cid;

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
            std::string deviceAddress{bd_addr_to_str(address)};
            if (status != ERROR_CODE_SUCCESS) {
                LOG_INFO("A2DP Source: Connection failed, status 0x%02x, cid 0x%02x, a2dp_cid 0x%02x \n",
                         status,
                         cid,
                         AVRCP::mediaTracker.a2dp_cid);
                AVRCP::mediaTracker.a2dp_cid = 0;

                auto &busProxy = const_cast<sys::Service *>(ownerService)->bus;
                busProxy.sendUnicast(std::make_shared<message::bluetooth::ConnectResult>(device, false),
                                     service::name::bluetooth);
                break;
            }
            AVRCP::mediaTracker.a2dp_cid = cid;
            AVRCP::mediaTracker.volume   = 64;

            LOG_INFO("A2DP Source: Connected, a2dp cid 0x%02x, local seid %d.\n",
                     AVRCP::mediaTracker.a2dp_cid,
                     AVRCP::mediaTracker.local_seid);
            isConnected    = true;
            auto &busProxy = const_cast<sys::Service *>(ownerService)->bus;
            busProxy.sendUnicast(std::make_shared<message::bluetooth::ConnectResult>(device, true),
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
            LOG_INFO("A2DP Source: Received SBC codec configuration, sampling frequency %u, a2dp_cid 0x%02x, local "
                     "seid 0x%02x, remote seid 0x%02x.\n",
                     AVDTP::sbcConfig.samplingFrequency,
                     cid,
                     AVRCP::mediaTracker.local_seid,
                     AVRCP::mediaTracker.remote_seid);

            // Adapt Bluetooth spec definition to SBC Encoder expected input
            auto allocation_method =
                a2dp_subevent_signaling_media_codec_sbc_configuration_get_allocation_method(packet);
            AVDTP::sbcConfig.allocationMethod = static_cast<btstack_sbc_allocation_method_t>(allocation_method - 1);
            auto channel_mode                 = static_cast<avdtp_channel_mode_t>(
                a2dp_subevent_signaling_media_codec_sbc_configuration_get_channel_mode(packet));

            AVDTP::sbcConfig.numChannels = 2;
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

            metadata = DeviceMetadata_t{
                .sampleRate      = static_cast<unsigned int>(AVDTP::sbcConfig.samplingFrequency),
                .channels        = static_cast<unsigned short>(AVDTP::sbcConfig.numChannels),
                .samplesPerFrame = static_cast<unsigned int>(btstack_sbc_encoder_num_audio_frames()),
            };

            break;
        }

        case A2DP_SUBEVENT_SIGNALING_DELAY_REPORTING_CAPABILITY:
            LOG_INFO("A2DP Source: remote supports delay report, remote seid %d\n",
                     avdtp_subevent_signaling_delay_reporting_capability_get_remote_seid(packet));
            break;
        case A2DP_SUBEVENT_SIGNALING_CAPABILITIES_DONE:
            LOG_INFO("A2DP Source: All capabilities reported, remote seid %d\n",
                     avdtp_subevent_signaling_capabilities_done_get_remote_seid(packet));
            break;

        case A2DP_SUBEVENT_SIGNALING_DELAY_REPORT:
            LOG_INFO("A2DP Source: Received delay report of %d.%0d ms, local seid %d\n",
                     avdtp_subevent_signaling_delay_report_get_delay_100us(packet) / 10,
                     avdtp_subevent_signaling_delay_report_get_delay_100us(packet) % 10,
                     avdtp_subevent_signaling_delay_report_get_local_seid(packet));
            break;

        case A2DP_SUBEVENT_STREAM_ESTABLISHED:
            a2dp_subevent_stream_established_get_bd_addr(packet, address);
            status = a2dp_subevent_stream_established_get_status(packet);
            if (status != 0u) {
                LOG_INFO("A2DP Source: Stream failed, status 0x%02x.\n", status);
                break;
            }

            local_seid = a2dp_subevent_stream_established_get_local_seid(packet);
            cid        = a2dp_subevent_stream_established_get_a2dp_cid(packet);
            LOG_INFO("A2DP_SUBEVENT_STREAM_ESTABLISHED:  a2dp_cid [expected 0x%02x, received 0x%02x], local_seid %d "
                     "(expected %d), remote_seid %d (expected %d)\n",
                     AVRCP::mediaTracker.a2dp_cid,
                     cid,
                     local_seid,
                     AVRCP::mediaTracker.local_seid,
                     a2dp_subevent_stream_established_get_remote_seid(packet),
                     AVRCP::mediaTracker.remote_seid);

            if (local_seid != AVRCP::mediaTracker.local_seid) {
                LOG_INFO("A2DP Source: Stream failed, wrong local seid %d, expected %d.\n",
                         local_seid,
                         AVRCP::mediaTracker.local_seid);
                break;
            }
            LOG_INFO("A2DP Source: Stream established, a2dp cid 0x%02x, local seid %d, remote seid %d.\n",
                     AVRCP::mediaTracker.a2dp_cid,
                     AVRCP::mediaTracker.local_seid,
                     a2dp_subevent_stream_established_get_remote_seid(packet));

            sourceQueue = xQueueCreate(5, sizeof(AudioData_t));
            sinkQueue   = nullptr;
            if (sourceQueue != nullptr) {
                sendAudioEvent(audio::EventType::BlutoothA2DPDeviceState, audio::Event::DeviceState::Connected);
            }
            else {
                LOG_ERROR("failed to create queue!");
            }

            break;

        case A2DP_SUBEVENT_STREAM_RECONFIGURED:
            status     = a2dp_subevent_stream_reconfigured_get_status(packet);
            local_seid = a2dp_subevent_stream_reconfigured_get_local_seid(packet);
            cid        = a2dp_subevent_stream_reconfigured_get_a2dp_cid(packet);

            LOG_INFO("A2DP Source: Reconfigured: a2dp_cid [expected 0x%02x, received 0x%02x], local_seid [expected %d, "
                     "received %d]\n",
                     AVRCP::mediaTracker.a2dp_cid,
                     cid,
                     AVRCP::mediaTracker.local_seid,
                     local_seid);
            LOG_INFO("Status 0x%02x\n", status);
            break;

        case A2DP_SUBEVENT_STREAM_STARTED:
            local_seid = a2dp_subevent_stream_started_get_local_seid(packet);
            cid        = a2dp_subevent_stream_started_get_a2dp_cid(packet);

            AVRCP::playInfo.status = AVRCP_PLAYBACK_STATUS_PLAYING;
            if (AVRCP::mediaTracker.avrcp_cid != 0u) {
                avrcp_target_set_playback_status(AVRCP::mediaTracker.avrcp_cid, AVRCP_PLAYBACK_STATUS_PLAYING);
            }
            startTimer(&AVRCP::mediaTracker);
            LOG_INFO(
                "A2DP Source: Stream started: a2dp_cid [expected 0x%02x, received 0x%02x], local_seid [expected %d, "
                "received %d]\n",
                AVRCP::mediaTracker.a2dp_cid,
                cid,
                AVRCP::mediaTracker.local_seid,
                local_seid);
            break;

        case A2DP_SUBEVENT_STREAMING_CAN_SEND_MEDIA_PACKET_NOW:
            local_seid = a2dp_subevent_streaming_can_send_media_packet_now_get_local_seid(packet);
            cid        = a2dp_subevent_signaling_media_codec_sbc_configuration_get_a2dp_cid(packet);
            sendMediaPacket();
            break;

        case A2DP_SUBEVENT_STREAM_SUSPENDED:
            local_seid = a2dp_subevent_stream_suspended_get_local_seid(packet);
            cid        = a2dp_subevent_stream_suspended_get_a2dp_cid(packet);

            AVRCP::playInfo.status = AVRCP_PLAYBACK_STATUS_PAUSED;
            if (AVRCP::mediaTracker.avrcp_cid != 0u) {
                avrcp_target_set_playback_status(AVRCP::mediaTracker.avrcp_cid, AVRCP_PLAYBACK_STATUS_PAUSED);
            }
            LOG_INFO(
                "A2DP Source: Stream paused: a2dp_cid [expected 0x%02x, received 0x%02x], local_seid [expected %d, "
                "received %d]\n",
                AVRCP::mediaTracker.a2dp_cid,
                cid,
                AVRCP::mediaTracker.local_seid,
                local_seid);

            stopTimer(&AVRCP::mediaTracker);
            break;

        case A2DP_SUBEVENT_STREAM_RELEASED: {
            AVRCP::playInfo.status = AVRCP_PLAYBACK_STATUS_STOPPED;
            cid                    = a2dp_subevent_stream_released_get_a2dp_cid(packet);
            local_seid             = a2dp_subevent_stream_released_get_local_seid(packet);
            LOG_INFO(
                "A2DP Source: Stream released: a2dp_cid [expected 0x%02x, received 0x%02x], local_seid [expected %d, "
                "received %d]\n",
                AVRCP::mediaTracker.a2dp_cid,
                cid,
                AVRCP::mediaTracker.local_seid,
                local_seid);
            sendAudioEvent(audio::EventType::BlutoothA2DPDeviceState, audio::Event::DeviceState::Disconnected);
            if (cid == AVRCP::mediaTracker.a2dp_cid) {
                AVRCP::mediaTracker.stream_opened = 0;
                LOG_INFO("A2DP Source: Stream released.\n");
            }
            if (AVRCP::mediaTracker.avrcp_cid != 0u) {

                avrcp_target_set_playback_status(AVRCP::mediaTracker.avrcp_cid, AVRCP_PLAYBACK_STATUS_STOPPED);
            }
            auto &busProxy = const_cast<sys::Service *>(ownerService)->bus;
            busProxy.sendUnicast(std::make_shared<message::bluetooth::DisconnectResult>(device),
                                 service::name::bluetooth);
            stopTimer(&AVRCP::mediaTracker);
            break;
        }
        case A2DP_SUBEVENT_SIGNALING_CONNECTION_RELEASED:
            cid = a2dp_subevent_signaling_connection_released_get_a2dp_cid(packet);
            if (cid == AVRCP::mediaTracker.a2dp_cid) {
                AVRCP::mediaTracker.avrcp_cid = 0;
                AVRCP::mediaTracker.a2dp_cid  = 0;
                LOG_INFO("A2DP Source: Signaling released.\n\n");
            }
            isConnected = false;
            break;
        default:
            break;
        }
    }

    void A2DP::A2DPImpl::connect()
    {
        if (isConnected) {
            disconnect();
        }
        LOG_INFO("Starting playback");
        a2dp_source_establish_stream(device.address, &AVRCP::mediaTracker.a2dp_cid);
    }

    void A2DP::A2DPImpl::disconnect()
    {
        LOG_INFO("Stopping playback");
        a2dp_source_disconnect(AVRCP::mediaTracker.a2dp_cid);
        auto &busProxy = const_cast<sys::Service *>(ownerService)->bus;
        busProxy.sendUnicast(std::make_shared<message::bluetooth::DisconnectResult>(device), service::name::bluetooth);
    }

    void A2DP::A2DPImpl::setDevice(const Devicei &dev)
    {
        device = dev;
        LOG_INFO("Device set!");
    }

    void A2DP::A2DPImpl::setOwnerService(const sys::Service *service)
    {
        ownerService = service;
    }

    void A2DP::A2DPImpl::sendAudioEvent(audio::EventType event, audio::Event::DeviceState state)
    {
        auto evt       = std::make_shared<audio::Event>(event, state);
        auto msg       = std::make_shared<AudioEventRequest>(std::move(evt));
        auto &busProxy = const_cast<sys::Service *>(ownerService)->bus;
        busProxy.sendUnicast(std::move(msg), service::name::evt_manager);
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

} // namespace bluetooth
