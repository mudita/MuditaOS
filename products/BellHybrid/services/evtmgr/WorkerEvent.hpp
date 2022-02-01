// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <service-evtmgr/WorkerEventCommon.hpp>

namespace bell
{
    class WorkerEvent : public WorkerEventCommon
    {
      public:
        explicit WorkerEvent(sys::Service *service);
        void deinitProductHardware() final;

      private:
        void addProductQueues(std::list<sys::WorkerQueueInfo> &queuesList) final;
        void initProductHardware() final;
        void processKeyEvent(bsp::KeyEvents event, bsp::KeyCodes code) final;
        bool handleMessage(std::uint32_t queueID) override;
        static constexpr auto rotaryEncoderQueueSize = 64U;
        static constexpr auto rotaryEncoderQueueName = "qRotaryEncoder";
    };
} // namespace bell
