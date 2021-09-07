// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include "A2DPImpl.hpp"
extern "C"
{
#include "classic/avrcp.h"
#include "classic/avrcp_browsing_controller.h"
#include "classic/avrcp_browsing_target.h"
#include "classic/avrcp_controller.h"
#include "classic/avrcp_media_item_iterator.h"
#include "classic/avrcp_target.h"
#include <btstack.h>
#include <btstack_defines.h>
}

namespace bluetooth
{
    class AVRCP
    {
      public:
        constexpr static const uint8_t subunitInfo[] = {0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3,
                                                        4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7};
        constexpr static uint32_t companyId          = 0x112233;
        constexpr static uint8_t companiesNum        = 1;
        constexpr static uint8_t companies[]         = {
            0x00, 0x19, 0x58 // BT SIG registered CompanyID
        };

        constexpr static uint8_t eventsNum     = 6;
        constexpr static uint8_t events[]      = {AVRCP_NOTIFICATION_EVENT_PLAYBACK_STATUS_CHANGED,
                                             AVRCP_NOTIFICATION_EVENT_TRACK_CHANGED,
                                             AVRCP_NOTIFICATION_EVENT_PLAYER_APPLICATION_SETTING_CHANGED,
                                             AVRCP_NOTIFICATION_EVENT_NOW_PLAYING_CONTENT_CHANGED,
                                             AVRCP_NOTIFICATION_EVENT_AVAILABLE_PLAYERS_CHANGED,
                                             AVRCP_NOTIFICATION_EVENT_ADDRESSED_PLAYER_CHANGED};
        static constexpr int SDP_BUFFER_LENGTH = 200;
        struct PlaybackStatusInfo
        {
            uint8_t track_id[8];
            uint32_t song_length_ms;
            avrcp_playback_status_t status;
            uint32_t song_position_ms; // 0xFFFFFFFF if not supported
        };

        static std::array<uint8_t, SDP_BUFFER_LENGTH> sdpTargetServiceBuffer;
        static std::array<uint8_t, SDP_BUFFER_LENGTH> sdpControllerServiceBuffer;
        static sys::Service *ownerService;

        static avrcp_track_t tracks[3];
        static int currentTrackIndex;
        static PlaybackStatusInfo playInfo;
        static MediaContext mediaTracker;

        static void packetHandler(uint8_t packetType, uint16_t channel, uint8_t *packet, uint16_t size);
        static void targetPacketHandler(uint8_t packetType, uint16_t channel, uint8_t *packet, uint16_t size);
        static void controllerPacketHandler(uint8_t packetType, uint16_t channel, uint8_t *packet, uint16_t size);
        static void init(sys::Service *service);
    };
} // namespace Bt
