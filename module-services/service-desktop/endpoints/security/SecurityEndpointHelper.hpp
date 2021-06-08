// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <endpoints/Context.hpp>

#include <Service/Common.hpp>
#include <Service/Service.hpp>
#include <endpoints/BaseHelper.hpp>

namespace sys
{
    class Service;
} // namespace sys

namespace parserFSM
{
    class SecurityEndpointHelper : public BaseHelper
    {
      public:
        explicit SecurityEndpointHelper(sys::Service *p) : BaseHelper(p)
        {}

        static constexpr auto PasscodeLength = 4;

      private:
        auto processPut(Context &context) -> ProcessResult final;
        auto processGet(Context &context) -> ProcessResult final;

        auto processStatus(Context &context) -> http::Code;
        auto processConfiguration(Context &context) -> http::Code;

        auto passCodeStringToVecOfInts(const std::string &passCode) -> std::vector<unsigned int>;
    };
} // namespace parserFSM
