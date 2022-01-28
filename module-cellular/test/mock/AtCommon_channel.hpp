// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "modem/ATCommon.hpp"
#include "modem/BaseChannel.hpp"

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

        void cmdInit() override
        {}

        int cmdSend(std::string cmd) override
        {
            return -1;
        }

        void cmdPost() override
        {}

        size_t cmdReceive(std::uint8_t *buffer, std::chrono::milliseconds timeoutMs) override
        {
            return {};
        }
    };

    class FailingChannel : public ChannelMock
    {
        auto ResultMock() -> Result override
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
        auto ResultMock() -> Result override
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
        auto ResultMock() -> Result override
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

    class QNWINFO_badChannel : public ChannelMock
    {
        auto ResultMock() -> Result override
        {
            auto r     = Result();
            r.code     = Result::Code::ERROR;
            r.response = {"LOL", "XD", "OK"};
            return r;
        }
    };

    class QNWINFO_emptyData : public ChannelMock
    {
        auto ResultMock() -> Result override
        {
            auto r     = Result();
            r.code     = Result::Code::OK;
            r.response = {"+QNWINFO: \"\",,\"\",", "OK"};
            return r;
        }
    };

    class QNWINFO_successNoQuoteChannel : public ChannelMock
    {
      public:
        std::string act  = "FDD LTE";
        int op           = 46001;
        std::string band = "LTE BAND 3";
        int channel      = 1650;

        auto ResultMock() -> Result override
        {
            auto r     = Result();
            r.code     = Result::Code::OK;
            r.response = {"+QNWINFO: " + act + "," + std::to_string(op) + "," + band + "," + std::to_string(channel),
                          "OK"};
            return r;
        }
    };

    class QNWINFO_successWithQuoteChannel : public ChannelMock
    {
      public:
        std::string act  = "FDD LTE";
        int op           = 46001;
        std::string band = "LTE BAND 3";
        int channel      = 1650;

        auto ResultMock() -> Result override
        {
            auto r     = Result();
            r.code     = Result::Code::OK;
            r.response = {"+QNWINFO: \"" + act + "\"," + std::to_string(op) + ",\"" + band + "\"," +
                              std::to_string(channel),
                          "OK"};
            return r;
        }
    };

    class QNWINFO_errorChannel : public ChannelMock
    {
      public:
        std::string act  = "\"FDD LTE\"";
        int op           = 46001;
        std::string band = "\"LTE BAND 3\"";
        int channel      = 1650;

        auto ResultMock() -> Result override
        {
            auto r     = Result();
            r.code     = Result::Code::ERROR;
            r.response = {"+QNWINFO: " + act + "," + std::to_string(op) + "," + band + "," + std::to_string(channel),
                          "ERROR"};
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

    /// provides proper CPBS response
    class CPBS_successChannel : public ChannelMock
    {
      public:
        const std::string storage = "\"SM\"";
        const std::string used    = "2";
        const std::string total   = "500";
        auto ResultMock() -> Result final
        {
            auto result     = Result();
            result.code     = Result::Code::OK;
            result.response = {"+CPBS: " + storage + "," + used + "," + total, "OK"};
            return result;
        }
    };

    /// provides invalid CPBS response
    class CPBS_toManyTokens : public ChannelMock
    {
      public:
        const std::string storage    = "\"SM\"";
        const std::string used       = "2";
        const std::string total      = "500";
        const std::string additional = "500";

        auto ResultMock() -> Result final
        {
            auto result     = Result();
            result.code     = Result::Code::OK;
            result.response = {"+CPBS: " + storage + "," + used + "," + total + "," + additional, "OK"};
            return result;
        }
    };

    /// provides invalid CPBS response
    class CPBS_toLittleTokens : public ChannelMock
    {
      public:
        const std::string storage = "\"SM\"";
        const std::string used    = "2";

        auto ResultMock() -> Result final
        {
            auto result     = Result();
            result.code     = Result::Code::OK;
            result.response = {"+CPBS: " + storage + "," + used, "OK"};
            return result;
        }
    };

    /// provides proper CPBR response
    class CPBR_successChannel : public ChannelMock
    {
      public:
        const std::string index1  = "1";
        const std::string number1 = "123456789";
        const std::string type1   = "161"; // proper type, national
        const std::string text1   = "Mock1";

        const std::string index2  = "2";
        const std::string number2 = "+48123456789";
        const std::string type2   = "145"; // proper type international
        const std::string text2   = "Mock2";

        auto ResultMock() -> Result final
        {
            auto result     = Result();
            result.code     = Result::Code::OK;
            result.response = {"+CPBR: " + index1 + "," + number1 + "," + type1 + "," + text1,
                               "+CPBR: " + index2 + "," + number2 + "," + type2 + "," + text2,
                               "OK"};
            return result;
        }
    };

    /// provides invalid CPBR response
    class CPBR_toLittleTokens : public ChannelMock
    {
      public:
        const std::string index1  = "1";
        const std::string number1 = "1234";
        const std::string type1   = "161"; // proper type, national

        auto ResultMock() -> Result final
        {
            auto result     = Result();
            result.code     = Result::Code::OK;
            result.response = {"+CPBR: " + index1 + "," + number1 + "," + type1, "OK"};
            return result;
        }
    };

    /// provides invalid CPBR response
    class CPBR_toManyTokens : public ChannelMock
    {
      public:
        const std::string index1     = "1";
        const std::string number1    = "1234";
        const std::string type1      = "161"; // proper type, national
        const std::string text1      = "Mock1";
        const std::string additional = "500";

        auto ResultMock() -> Result final
        {
            auto result     = Result();
            result.code     = Result::Code::OK;
            result.response = {"+CPBR: " + index1 + "," + number1 + "," + type1 + "," + text1, "," + additional, "OK"};
            return result;
        }
    };

    /// provides invalid CPBR response
    class CPBR_invalidType : public ChannelMock
    {
      public:
        const std::string index1  = "1";
        const std::string number1 = "1234";
        const std::string type1   = "7"; // invalid type
        const std::string text1   = "Mock1";

        auto ResultMock() -> Result final
        {
            auto result     = Result();
            result.code     = Result::Code::OK;
            result.response = {"+CPBR: " + index1 + "," + number1 + "," + type1 + "," + text1, "OK"};
            return result;
        }
    };

    /// provides proper QSIMSTAT response
    class QSIMSTAT_successChannel : public ChannelMock
    {
      public:
        const std::string enabled = "1";
        const std::string status  = "1";

        auto ResultMock() -> Result final
        {
            auto result     = Result();
            result.code     = Result::Code::OK;
            result.response = {"+QSIMSTAT: " + enabled + "," + status, "OK"};
            return result;
        }
    };

    /// provides invalid QSIMSTAT response
    class QSIMSTAT_toLittleTokens : public ChannelMock
    {
      public:
        const std::string enabled = "1";

        auto ResultMock() -> Result final
        {
            auto result     = Result();
            result.code     = Result::Code::OK;
            result.response = {"+QSIMSTAT: " + enabled, "OK"};
            return result;
        }
    };

    /// provides invalid QSIMSTAT response
    class QSIMSTAT_toManyTokens : public ChannelMock
    {
      public:
        const std::string enabled = "1";
        const std::string status  = "1";
        const std::string bad     = "1";

        auto ResultMock() -> Result final
        {
            auto result     = Result();
            result.code     = Result::Code::OK;
            result.response = {"+QSIMSTAT: " + enabled + "," + status + "," + bad, "OK"};
            return result;
        }
    };

    /// provides invalid QSIMSTAT response
    class QSIMSTAT_invalidEnabled : public ChannelMock
    {
      public:
        const std::string enabled = "78";
        const std::string status  = "1";

        auto ResultMock() -> Result final
        {
            auto result     = Result();
            result.code     = Result::Code::OK;
            result.response = {"+QSIMSTAT: " + enabled + "," + status, "OK"};
            return result;
        }
    };

    /// provides invalid QSIMSTAT response
    class QSIMSTAT_invalidStatus : public ChannelMock
    {
      public:
        const std::string enabled = "1";
        const std::string status  = "98";

        auto ResultMock() -> Result final
        {
            auto result     = Result();
            result.code     = Result::Code::OK;
            result.response = {"+QSIMSTAT: " + enabled + "," + status, "OK"};
            return result;
        }
    };

    /// provides invalid QSIMSTAT response
    class QCGFGUsbnet_toLittleTokens : public ChannelMock
    {
      public:
        auto ResultMock() -> Result final
        {
            auto result     = Result();
            result.code     = Result::Code::OK;
            result.response = {"+QCFG: \"usbnet\",", "OK"};
            return result;
        }
    };

    /// provides proper QCFGUsbnet response
    class QCGFGUsbnet_successChannel : public ChannelMock
    {
      public:
        auto ResultMock() -> Result final
        {
            auto result     = Result();
            result.code     = Result::Code::OK;
            result.response = {"+QCFG: \"usbnet\",1", "OK"};
            return result;
        }
    };

    /// provides invalid QCFGUsbnet response
    class QCFGUsbnet_toManyTokens : public ChannelMock
    {
      public:
        const std::string net = "1";
        const std::string bad = "1";

        auto ResultMock() -> Result final
        {
            auto result     = Result();
            result.code     = Result::Code::OK;
            result.response = {"+QCFG: \"usbnet\"," + net + "," + bad, "OK"};
            return result;
        }
    };

    /// provides invalid QCFGUsbnet response
    class QCFGUsbnet_invalidToken : public ChannelMock
    {
      public:
        const std::string net = "9";

        auto ResultMock() -> Result final
        {
            auto result     = Result();
            result.code     = Result::Code::OK;
            result.response = {"+QCFG: \"usbnet\"," + net, "OK"};
            return result;
        }
    };
} // namespace at
