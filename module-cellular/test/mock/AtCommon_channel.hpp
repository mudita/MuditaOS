// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Modem/ATCommon.hpp"
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

        std::string cmd_receive() override
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

    /// provides proper CLCC response with one call on the list
    class CLCC_successChannel_oneCall : public ChannelMock
    {
      public:
        const std::string idx         = "1";
        const std::string dir         = "0";
        const std::string stateOfCall = "0";
        const std::string mode        = "0";
        const std::string multiparty  = "1";
        const std::string number      = "10086";
        const std::string type        = "129";

        auto ResultMock() -> Result final
        {
            auto result     = Result();
            result.code     = Result::Code::OK;
            result.response = {"+CLCC: " + idx + "," + dir + "," + stateOfCall + "," + mode + "," + multiparty + "," +
                                   number + "," + type,
                               "OK"};
            return result;
        }
    };

    /// provides proper CLCC response with two calls on the list
    class CLCC_successChannel_twoCalls : public ChannelMock
    {
      public:
        const std::string idx1{"1"};
        const std::string dir1{"0"};
        const std::string stateOfCall1{"0"};
        const std::string mode1{"1"};
        const std::string multiparty1{"0"};
        const std::string number1{""};
        const std::string type1{"129"};

        const std::string idx2{"2"};
        const std::string dir2{"0"};
        const std::string stateOfCall2{"0"};
        const std::string mode2{"0"};
        const std::string multiparty2{"0"};
        const std::string number2{"10086"};
        const std::string type2{"129"};

        const std::string tokensLTEMode{"1,0,0,1,0,\"\",129"};
        const std::string tokensEstablishCall{"2,0,0,0,0,\"10086\",129"};

        auto ResultMock() -> Result final
        {
            auto result     = Result();
            result.code     = Result::Code::OK;
            result.response = {"+CLCC: " + tokensLTEMode, "+CLCC: " + tokensEstablishCall, "OK"};
            return result;
        }
    };

    // Provides succesfull 'OK' response
    class OK_Channel : public ChannelMock
    {
      public:
        auto ResultMock() -> Result final
        {
            auto result     = Result();
            result.code     = Result::Code::OK;
            result.response = {"OK"};
            return result;
        }
    };

    // Provides bad 'OG' response
    class OG_Channel : public ChannelMock
    {
      public:
        auto ResultMock() -> Result final
        {
            auto result     = Result();
            result.code     = Result::Code::OK;
            result.response = {"OG"};
            return result;
        }
    };

    /// provides proper CFUN response
    class CFUN_successChannel : public ChannelMock
    {
      public:
        const std::string token = "1";

        auto ResultMock() -> Result final
        {
            auto result     = Result();
            result.code     = Result::Code::OK;
            result.response = {"+CFUN: " + token, "OK"};
            return result;
        }
    };

    /// provides invalid CFUN response
    class CFUN_invalidTokenChannel : public ChannelMock
    {
      public:
        const std::string token = "7";

        auto ResultMock() -> Result final
        {
            auto result     = Result();
            result.code     = Result::Code::OK;
            result.response = {"+CFUN: " + token, "OK"};
            return result;
        }
    };
} // namespace at
