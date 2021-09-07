// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <service-evtmgr/WorkerEventCommon.hpp>

namespace purephone
{
    class WorkerEvent : public WorkerEventCommon
    {
      public:
        explicit WorkerEvent(sys::Service *service);

        void requestSliderPositionRead();

      private:
        static constexpr auto headsetQueueName            = "qHeadset";
        static constexpr auto simNotifyQueueName          = "qSIM";
        static constexpr auto magnetometerQueueName       = "qMagnetometer";
        static constexpr auto magnetometerNotifyQueueName = "qMagnetometerNotify";
        static constexpr auto torchQueueName              = "qTorch";
        static constexpr auto lightSensorQueueName        = "qLightSensor";

        static constexpr auto headsetQueueSize            = 10;
        static constexpr auto simNotifyQueueSize          = 5;
        static constexpr auto magnetometerQueueSize       = 5;
        static constexpr auto magnetometerNotifyQueueSize = 1;
        static constexpr auto torchQueueSize              = 5;
        static constexpr auto lightSensorQueueSize        = 5;

        void addProductQueues(std::list<sys::WorkerQueueInfo> &queuesList) final;
        void initProductHardware() final;
        void deinitProductHardware() final;
        bool handleMessage(std::uint32_t queueID) override;

        void handleMagnetometerEvent();

        /**
         * @brief This method is responsible for translating a headset key to keyboard key code.
         * @param code key code
         */
        bsp::KeyCodes headsetKeyToKeyboardKey(uint8_t code);

        enum class EventQueues
        {
            queueHeadsetIRQ = static_cast<int>(WorkerEventQueues::queueRTC) + 1,
            queueCellular,
            queueMagnetometerIRQ,
            queueMagnetometerNotify,
            queueTorch,
            queueLightSensor,
        };
    };
} // namespace purephone
