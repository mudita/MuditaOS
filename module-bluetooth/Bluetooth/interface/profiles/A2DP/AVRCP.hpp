// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
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
      private:
        static constexpr std::uint16_t maxVolumeValue = 0x7F; // From Bluetooth AVRCP 1.6.2 spec, p. 83
        static std::uint16_t targetVolumeToPercent(std::uint16_t volume);
        static std::uint16_t controllerVolumeToPercent(std::uint16_t volume);

      public:
        struct PlaybackStatusInfo
        {
            std::uint8_t track_id[8];
            std::uint32_t song_length_ms;
            avrcp_playback_status_t status = AVRCP_PLAYBACK_STATUS_STOPPED;
            std::uint32_t song_position_ms; // 0xFFFFFFFF if not supported
        };

        static void packetHandler(std::uint8_t packetType,
                                  std::uint16_t channel,
                                  std::uint8_t *packet,
                                  std::uint16_t size);
        static void targetPacketHandler(std::uint8_t packetType,
                                        std::uint16_t channel,
                                        std::uint8_t *packet,
                                        std::uint16_t size);
        static void controllerPacketHandler(std::uint8_t packetType,
                                            std::uint16_t channel,
                                            std::uint8_t *packet,
                                            std::uint16_t size);
        static void init(sys::Service *service);

        static constexpr std::uint8_t subunitInfo[]       = {0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3,
                                                       4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7};
        static constexpr std::uint32_t companyId          = 0x112233;
        static constexpr auto targetServiceBufferSize     = 200;
        static constexpr auto controllerServiceBufferSize = 200;

        static std::uint8_t sdpTargetServiceBuffer[targetServiceBufferSize];
        static std::uint8_t sdpControllerServiceBuffer[controllerServiceBufferSize];

        static sys::Service *ownerService;

        static PlaybackStatusInfo playInfo;
        static MediaContext mediaTracker;
    };
} // namespace bluetooth
