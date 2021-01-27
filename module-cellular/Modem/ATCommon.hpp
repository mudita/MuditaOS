// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <FreeRTOS.h>
#include <at/ErrorCode.hpp>
#include <mutex.hpp>
#include <task.h>
#include "BaseChannel.hpp"

namespace sys
{
    class Service;
}

namespace at
{
    class Channel : public BaseChannel
    {
      protected:
        constexpr static char cmdSeparator  = '\r';
        const std::array<char, 3> validTerm = {cmdSeparator, ',', '='};
        [[nodiscard]] auto formatCommand(const std::string &cmd) const -> std::string;

        cpp_freertos::MutexStandard mutex;
        TaskHandle_t blockedTaskHandle = nullptr;

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

        /// waits till ok or timeout
        virtual auto cmd(const std::string &cmd,
                         std::chrono::milliseconds timeout = at::default_timeout,
                         size_t rxCount                    = 0) -> Result final;
        virtual auto cmd(const at::AT &at) -> Result final;
        virtual auto cmd(const at::Cmd &at) -> Result final;
        /// check for OK, ERROR in string in last token
        virtual Result::Code at_check(const std::vector<std::string> &arr);
        /// check for +CME ERROR: errors if exists in last token
        virtual bool at_check_cmx_error(const std::string &CMX, const std::vector<std::string> &arr, uint32_t &errcode);
        virtual void cmd_log(std::string cmd, const Result &result, uint32_t timeout) final;
        virtual auto ProcessNewData(sys::Service *service) -> int
        {
            return 0;
        }
    };

}; // namespace at
