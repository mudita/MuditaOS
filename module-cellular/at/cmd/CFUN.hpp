// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Result.hpp"
#include <at/Cmd.hpp>
#include <functional>
#include <PhoneNumber.hpp>

namespace at
{
    namespace cfun
    {
        enum class Functionality
        {
            Minimum   = 0,
            Full      = 1,
            DisableRF = 4
        };

        enum class Reset
        {
            DoNotReset,
            ResetTheME
        };
    } // namespace cfun
    namespace result
    {
        /// please see documentation:
        /// QuectelEC2526EC21ATCommandsManualV13.1100970659
        /// page: 118 for more information
        struct CFUN : public Result
        {
            at::cfun::Functionality functionality;
            explicit CFUN(const Result &);
        };
    } // namespace result

    namespace cmd
    {
        class CFUN : public Cmd
        {
          public:
            CFUN() noexcept;
            explicit CFUN(at::cmd::Modifier mod) noexcept;
            [[nodiscard]] auto parseCFUN(const Result &base_result) -> result::CFUN;
            void set(at::cfun::Functionality fuctionality, at::cfun::Reset reset = at::cfun::Reset::DoNotReset);
        };
    } // namespace cmd

} // namespace at
