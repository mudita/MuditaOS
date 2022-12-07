// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <endpoints/BaseHelper.hpp>

#include <ctime>

namespace sdesktop::endpoints
{

    class SecurityEndpointHelper : public BaseHelper
    {
      public:
        explicit SecurityEndpointHelper(sys::Service *p) : BaseHelper(p)
        {}

        static constexpr auto PasscodeLength = 4;

      private:
        auto preventBlockingDevice() -> bool;
        auto processPut(Context &context) -> ProcessResult final;
        auto processGet(Context &context) -> ProcessResult final;

        auto processStatus(Context &context) -> http::Code;
        auto getPhoneLockTime(Context &context) -> time_t;
        auto processConfiguration(Context &context) -> http::Code;

        auto passCodeArrayToVecOfInts(const json11::Json::array &passCode) -> std::vector<unsigned int>;
    };

} // namespace sdesktop::endpoints
