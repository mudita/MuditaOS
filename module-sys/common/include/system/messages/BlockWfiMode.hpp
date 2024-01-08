// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "FreeRTOS.h"
#include "task.h"
#include <Service/Message.hpp>
#include <bsp/common.hpp>

namespace sys
{
    class BlockWfiModeMessage : public sys::DataMessage
    {
      public:
        BlockWfiModeMessage(std::string sentinelName, bool block, TaskHandle_t handle)
            : sys::DataMessage(MessageType::SystemManagerCpuFrequency), sentinelName(std::move(sentinelName)),
              block(block), handle(handle)
        {}

        [[nodiscard]] auto getRequest() const noexcept
        {
            return block;
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
        bool block;
        TaskHandle_t handle;
    };
} // namespace sys
