// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AVRCP.hpp"

#include <service-audio/AudioServiceName.hpp>
#include <service-bluetooth/ServiceBluetoothName.hpp>
#include <service-bluetooth/messages/AudioVolume.hpp>
#include <service-bluetooth/messages/AudioNotify.hpp>

namespace bluetooth
{
    std::uint8_t AVRCP::sdpTargetServiceBuffer[targetServiceBufferSize];
    std::uint8_t AVRCP::sdpControllerServiceBuffer[controllerServiceBufferSize];

    sys::Service *AVRCP::ownerService = nullptr;

    AVRCP::PlaybackStatusInfo AVRCP::playInfo;
    MediaContext AVRCP::mediaTracker;

    void AVRCP::packetHandler(std::uint8_t packetType,
                              [[maybe_unused]] std::uint16_t channel,
                              std::uint8_t *packet,
                              [[maybe_unused]] std::uint16_t size)
    {
        bd_addr_t event_addr;
        std::uint16_t local_cid;

        if (packetType != HCI_EVENT_PACKET || hci_event_packet_get_type(packet) != HCI_EVENT_AVRCP_META) {
            return;
        }

        const auto subevent_code = hci_event_avrcp_meta_get_subevent_code(packet);

        switch (subevent_code) {
        case AVRCP_SUBEVENT_CONNECTION_ESTABLISHED: {
            local_cid = avrcp_subevent_connection_established_get_avrcp_cid(packet);
            const auto status = avrcp_subevent_connection_established_get_status(packet);

            if (status != ERROR_CODE_SUCCESS) {
                LOG_ERROR("Connection failed, local cid 0x%02X, status 0x%02X", local_cid, status);
                break;
            }

            AVRCP::mediaTracker.avrcp_cid = local_cid;
            avrcp_subevent_connection_established_get_bd_addr(packet, event_addr);

            avrcp_target_support_event(AVRCP::mediaTracker.avrcp_cid, AVRCP_NOTIFICATION_EVENT_PLAYBACK_STATUS_CHANGED);
            avrcp_target_support_event(AVRCP::mediaTracker.avrcp_cid, AVRCP_NOTIFICATION_EVENT_TRACK_CHANGED);
            avrcp_target_support_event(AVRCP::mediaTracker.avrcp_cid,
                                       AVRCP_NOTIFICATION_EVENT_NOW_PLAYING_CONTENT_CHANGED);

            avrcp_controller_enable_notification(AVRCP::mediaTracker.avrcp_cid,
                                                 AVRCP_NOTIFICATION_EVENT_VOLUME_CHANGED);

            avrcp_target_set_unit_info(AVRCP::mediaTracker.avrcp_cid, AVRCP_SUBUNIT_TYPE_AUDIO, AVRCP::companyId);
            avrcp_target_set_subunit_info(AVRCP::mediaTracker.avrcp_cid,
                                          AVRCP_SUBUNIT_TYPE_AUDIO,
                                          static_cast<const std::uint8_t *>(AVRCP::subunitInfo),
                                          sizeof(AVRCP::subunitInfo));
            avrcp_target_set_playback_status(AVRCP::mediaTracker.avrcp_cid, AVRCP::playInfo.status);
        } break;

        case AVRCP_SUBEVENT_CONNECTION_RELEASED: {
            LOG_INFO("Disconnected, avrcp_cid 0x%02X", avrcp_subevent_connection_released_get_avrcp_cid(packet));
            AVRCP::mediaTracker.avrcp_cid = 0;
        } break;

        default:
            LOG_DEBUG("Event not handled: 0x%02X", subevent_code);
            break;
        }
    }

    void AVRCP::targetPacketHandler(std::uint8_t packetType,
                                    [[maybe_unused]] std::uint16_t channel,
                                    std::uint8_t *packet,
                                    [[maybe_unused]] std::uint16_t size)
    {
        if (packetType != HCI_EVENT_PACKET || hci_event_packet_get_type(packet) != HCI_EVENT_AVRCP_META) {
            return;
        }

        const auto subevent_code = hci_event_avrcp_meta_get_subevent_code(packet);

        switch (subevent_code) {
        case AVRCP_SUBEVENT_PLAY_STATUS_QUERY: {
            const auto status = avrcp_target_play_status(AVRCP::mediaTracker.avrcp_cid,
                                                         AVRCP::playInfo.song_length_ms,
                                                         AVRCP::playInfo.song_position_ms,
                                                         AVRCP::playInfo.status);
            if (status != ERROR_CODE_SUCCESS) {
                LOG_ERROR("Play status query failed, status: 0x%02X", status);
            }
        } break;

        case AVRCP_SUBEVENT_OPERATION: {
            const auto operation_id =
                static_cast<avrcp_operation_id_t>(avrcp_subevent_operation_get_operation_id(packet));

            switch (operation_id) {
            case AVRCP_OPERATION_ID_PLAY: {
                LOG_INFO("AVRCP operation: PLAY");
                ownerService->bus.sendUnicast(std::make_shared<message::bluetooth::AudioStart>(), service::name::audio);
            } break;

            case AVRCP_OPERATION_ID_PAUSE: {
                LOG_INFO("AVRCP operation: PAUSE");
                ownerService->bus.sendUnicast(std::make_shared<message::bluetooth::AudioPause>(), service::name::audio);
            } break;

            case AVRCP_OPERATION_ID_STOP: {
                LOG_INFO("AVRCP operation: STOP");
                const auto status = a2dp_source_disconnect(AVRCP::mediaTracker.a2dp_cid);
                if (status != ERROR_CODE_SUCCESS) {
                    LOG_ERROR("Stop request failed, status: 0x%02X", status);
                }
            } break;

            default:
                LOG_INFO("Operation not handled: 0x%02X ", operation_id);
                break;
            }
        }

        default:
            LOG_DEBUG("Event not handled: 0x%02X", subevent_code);
            break;
        }
    }

    void AVRCP::controllerPacketHandler(std::uint8_t packetType,
                                        [[maybe_unused]] std::uint16_t channel,
                                        std::uint8_t *packet,
                                        [[maybe_unused]] std::uint16_t size)
    {
        if (packetType != HCI_EVENT_PACKET || hci_event_packet_get_type(packet) != HCI_EVENT_AVRCP_META) {
            return;
        }

        if (AVRCP::mediaTracker.avrcp_cid == 0) {
            return;
        }

        const auto subevent_code = hci_event_avrcp_meta_get_subevent_code(packet);

        switch (subevent_code) {
        case AVRCP_SUBEVENT_NOTIFICATION_VOLUME_CHANGED: { // BT device requested volume change
            const auto volume = avrcp_subevent_notification_volume_changed_get_absolute_volume(packet);
            ownerService->bus.sendUnicast(std::make_shared<message::bluetooth::A2DPVolume>(volume),
                                          service::name::bluetooth);
            LOG_INFO("AVRCP Controller: BT device volume changed to %u%% (%u)",
                     AVRCP::controllerVolumeToPercent(volume),
                     volume);
        } break;

        case AVRCP_SUBEVENT_NOTIFICATION_EVENT_BATT_STATUS_CHANGED: { // see avrcp_battery_status_t
            LOG_INFO("AVRCP Controller: Notification Battery Status %d",
                     avrcp_subevent_notification_event_batt_status_changed_get_battery_status(packet));
        } break;

        case AVRCP_SUBEVENT_NOTIFICATION_STATE: {
            LOG_INFO("AVRCP Controller: Notification %s - %s",
                     avrcp_event2str(avrcp_subevent_notification_state_get_event_id(packet)),
                     avrcp_subevent_notification_state_get_enabled(packet) != 0 ? "enabled" : "disabled");
        } break;

        default:
            break;
        }
    }

    void AVRCP::init(sys::Service *service)
    {
        AVRCP::ownerService = service;

        avrcp_init();
        avrcp_register_packet_handler(&packetHandler);

        avrcp_target_init();
        avrcp_target_register_packet_handler(&targetPacketHandler);

        avrcp_controller_init();
        avrcp_controller_register_packet_handler(&controllerPacketHandler);
    }

    std::uint16_t AVRCP::targetVolumeToPercent(std::uint16_t volume)
    {
        return volume * AVRCP::maxVolumeValue / 100;
    }

    std::uint16_t AVRCP::controllerVolumeToPercent(std::uint16_t volume)
    {
        return volume * 100 / AVRCP::maxVolumeValue;
    }
} // namespace bluetooth
