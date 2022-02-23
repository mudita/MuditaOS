// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "FreeRTOS.h"
#include "task.h"
#include <Service/Message.hpp>
#include <bsp/common.hpp>

namespace sys
{

    /// TODO move data to struct...
    class HoldCpuFrequencyMessage : public sys::DataMessage
    {
      public:
        HoldCpuFrequencyMessage(std::string sentinelName, bsp::CpuFrequencyMHz request, TaskHandle_t handle)
            : sys::DataMessage(MessageType::SystemManagerCpuFrequency), sentinelName(std::move(sentinelName)),
              frequencyRequested(request), handle(handle)
        {}

        [[nodiscard]] auto getRequest() const noexcept
        {
            return frequencyRequested;
        };

        [[nodiscard]] auto getName() const
        {
            return sentinelName;
        };

        [[nodiscard]] TaskHandle_t getHandle() const
        {
            return handle;
        }

      private:
        std::string sentinelName;
        bsp::CpuFrequencyMHz frequencyRequested = bsp::CpuFrequencyMHz::Level_0;
        TaskHandle_t handle;
    };

    class ReleaseCpuFrequencyMessage : public sys::DataMessage
    {
      public:
        explicit ReleaseCpuFrequencyMessage(std::string sentinelName)
            : sys::DataMessage(MessageType::SystemManagerCpuFrequency), sentinelName(std::move(sentinelName))
        {}

        [[nodiscard]] auto getName() const
        {
            return sentinelName;
        };

      private:
        std::string sentinelName;
    };

    struct IsCpuPernament : public sys::DataMessage
    {
      public:
        explicit IsCpuPernament() 
        {}
    };

    struct IsCpuPernamentResponse : public sys::ResponseMessage
    {
      public:
        explicit IsCpuPernamentResponse(bool pernament) : pernament(pernament)
        {}
        const bool pernament = false;
    };



    struct HoldCpuFrequencyPermanentlyMessage : public sys::DataMessage
    {
      public:
        explicit HoldCpuFrequencyPermanentlyMessage(bsp::CpuFrequencyMHz request) : request(request)
        {}
        const bsp::CpuFrequencyMHz request;
    };

    class HoldCpuFrequencyPermanentlyResponse : public sys::ResponseMessage
    {
    };

    class ReleaseCpuPermanentFrequencyMessage : public sys::DataMessage
    {};

} // namespace sys
