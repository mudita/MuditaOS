// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <at/ErrorCode.hpp>
#include <at/Result.hpp>

namespace at
{
    constexpr auto minATCmdRet = 5; /// minimum return from AT in command mode "<cr<lf>X<cr><lf>"

    class ATStream
    {

      private:
        at::Result result;
        bool atAtTheBegin    = false; /// case for commands like QPING
        bool beginChecked    = false;
        size_t lastPos       = 0;
        size_t lineCounter   = 0;
        size_t rxCount       = 0;
        std::string atBuffer = {};
        bool isATReady       = false;
        uint32_t errcode     = 0;

      protected:
        bool checkATBegin();
        bool checkATEnd();
        void checkError();
        bool parse();
        void countLines();

      public:
        ATStream(size_t rxCount = 0) : rxCount(rxCount)
        {}

        bool write(const std::string &buffer);
        bool isReady()
        {
            return isATReady;
        }
        void reset();
        at::Result getResult()
        {
            return result;
        }

        Result::Code parseState(const std::string &state, uint32_t &errcode);
    };

} // namespace at
