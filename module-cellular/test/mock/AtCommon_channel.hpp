// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Modem/BaseChannel.hpp"

namespace at
{

    class ChannelMock : public BaseChannel
    {
      public:
        virtual Result ResultMock()
        {
            return Result();
        }

        auto cmd(const std::string &cmd,
                 std::chrono::milliseconds timeout = at::default_timeout,
                 size_t rxCount                    = 0) noexcept -> Result override
        {
            return ResultMock();
        }
        auto cmd(const at::AT &at) noexcept -> Result override
        {
            return ResultMock();
        }
        auto cmd(const at::Cmd &at) -> Result override
        {
            return ResultMock();
        }

        void cmd_init() override
        {}

        void cmd_send(std::string cmd) override
        {}

        void cmd_post() override
        {}

        std::vector<std::string> cmd_receive() override
        {
            return {};
        }
    };

    class FailingChannel : public ChannelMock
    {
        virtual Result ResultMock()
        {
            auto r = Result();
            r.code = Result::Code::ERROR;
            return r;
        }
    };

    class GenericChannel : public ChannelMock
    {
      public:
        GenericChannel(at::Result::Code code, std::vector<std::string> response) : result(code, std::move(response))
        {}

      private:
        virtual Result ResultMock()
        {
            return result;
        }

        const Result result;
    };

    /// provides CSCS bad response
    class CSCS_badChannel : public ChannelMock
    {
        virtual Result ResultMock()
        {
            auto r     = Result();
            r.code     = Result::Code::ERROR;
            r.response = {"LOL", "XD"};
            return r;
        }
    };

    /// standard bad CSCA values I get from modem (with result OK)
    class CSCA_emptyData : public ChannelMock
    {
        virtual Result ResultMock()
        {
            auto r     = Result();
            r.code     = Result::Code::OK;
            r.response = {"+CSCA: \"\","};
            return r;
        }
    };

    /// provides proper CSCS reponse
    class CSCS_successChannel : public ChannelMock
    {
      public:
        std::string smsCenterAddress = "\"0700\"";
        std::string smsTypeOfAddress = "124";

        auto ResultMock() -> Result override
        {
            auto r = Result();
            r.code = Result::Code::OK;
            // space below side to CSCS matters
            r.response = {"+CSCA: " + smsCenterAddress + "," + smsTypeOfAddress, "OK"};
            return r;
        }
    };
} // namespace at
