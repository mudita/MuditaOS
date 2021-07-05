// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <atomic>
#include <FreeRTOS.h>
#include <at/ErrorCode.hpp>
#include <mutex.hpp>
#include <task.h>
#include <message_buffer.h>
#include "BaseChannel.hpp"

namespace sys
{
    class Service;
}

namespace at
{
    constexpr auto delimiter                         = "\r\n"; /// use std::strlen()
    constexpr auto defaultBufferTimeoutMs            = std::chrono::milliseconds{50};
    inline constexpr size_t defaultReceiveBufferSize = 128; // receiveBuffer size used for single message processing
    inline constexpr size_t defaultMessageBufferSize =
        defaultReceiveBufferSize * 2; // MessageBuffer size for passing data between tasks

    struct AwaitingResponseFlag
    {
      private:
        std::atomic_bool isWaiting = false;

      public:
        void set()
        {
            isWaiting = true;
        }

        void clear()
        {
            isWaiting = false;
        }

        bool state() const
        {
            return isWaiting;
        }
    };

    class Channel : public BaseChannel
    {
      protected:
        constexpr static char cmdSeparator  = '\r';
        const std::array<char, 3> validTerm = {cmdSeparator, ',', '='};
        [[nodiscard]] auto formatCommand(const std::string &cmd) const -> std::string;

        MessageBufferHandle_t responseBuffer = nullptr;
        std::unique_ptr<uint8_t[]> receiveBuffer;
        AwaitingResponseFlag awaitingResponseFlag;
        cpp_freertos::MutexStandard mutex;

      public:
        static const std::string OK;
        static const std::string ERROR;
        static const std::string NO_CARRIER;
        static const std::string BUSY;
        static const std::string NO_ANSWER;
        static const std::string CME_ERROR;
        static const std::string CMS_ERROR;
        // other codes unused right now: Please see quectel QuectelEC2526EC21ATCommandsManualV1.3 page 21
        // const std::string Channel::CONNECT = "CONNECT";
        // const std::string Channel::RING = "RING";
        // const std::string Channel::NO_DIALTONE = "NO DIALTONE";

        explicit Channel(uint8_t *receiveBuffer)
            : receiveBuffer{receiveBuffer} {

              };

        /// waits till ok or timeout
        virtual auto cmd(const std::string &cmd,
                         std::chrono::milliseconds timeout = at::default_timeout,
                         size_t rxCount                    = 0) -> Result final;
        virtual auto cmd(const at::AT &at) -> Result final;
        virtual auto cmd(const at::Cmd &at) -> Result final;
        virtual void cmdLog(std::string cmd, const Result &result, std::chrono::milliseconds timeout) final;

        Result checkResult(bsp::cellular::CellularResultCode cellularResult);
    };
} // namespace at
