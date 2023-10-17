// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "A2DP.hpp"
#include <Bluetooth/Device.hpp>
#include <Bluetooth/Result.hpp>
#include <log/log.hpp>
#include <Audio/AudioCommon.hpp>
#include <audio/BluetoothAudioDevice.hpp>
#include "MediaContext.hpp"
#include <memory>

extern "C"
{
#include <btstack.h>
#include <btstack_defines.h>
}

namespace bluetooth
{
    class AVRCP;
    class A2DP::A2DPImpl
    {
      public:
        auto init() -> Result::Code;
        void deInit();
        void connect();
        void disconnect();
        void start();
        void stop();
        void setDevice(const Devicei &dev);
        void setOwnerService(sys::Service *service);
        void setAudioDevice(std::shared_ptr<bluetooth::BluetoothAudioDevice> audioDevice);

      private:
        static void startTimer(MediaContext *context);
        static void stopTimer(MediaContext *context);
        static void audioTimeoutHandler(btstack_timer_source_t *timer);
        static void sourcePacketHandler(std::uint8_t packetType,
                                        std::uint16_t channel,
                                        std::uint8_t *packet,
                                        std::uint16_t size);
        static void hciPacketHandler(std::uint8_t packetType,
                                     std::uint16_t channel,
                                     std::uint8_t *packet,
                                     std::uint16_t size);
        static void sendMediaPacket();
        static void sendAudioEvent(audio::EventType event, audio::Event::DeviceState state);

        static constexpr std::uint32_t a2dpSdpRecordHandle            = 0x10001;
        static constexpr std::uint32_t avrcpServiceSdpRecordHandle    = 0x10002;
        static constexpr std::uint32_t avrcpControllerSdpRecordHandle = 0x10003;
        static constexpr auto sourceServiceBufferSize                 = 150;
        static constexpr auto sourceQueueLength                       = 5;
        static constexpr int audioTimeoutMs                           = 10;

        static std::uint8_t sdpSourceServiceBuffer[sourceServiceBufferSize];
        static std::uint8_t mediaSbcCodecCapabilities[];

        static std::shared_ptr<BluetoothAudioDevice> audioDevice;
        static sys::Service *ownerService;
        static Devicei device;

        static btstack_packet_callback_registration_t hciEventCallbackRegistration;

        static QueueHandle_t sourceQueue;

        static bool isConnected;
    };
} // namespace bluetooth
