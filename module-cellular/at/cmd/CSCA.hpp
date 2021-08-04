// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Result.hpp"
#include <at/Cmd.hpp>
#include <functional>
#include <PhoneNumber.hpp>

namespace at
{
    namespace result
    {
        /// please see documentation:
        /// QuectelEC2526EC21ATCommandsManualV13.1100970659
        /// page: 118 for more information
        struct CSCA : public Result
        {
            std::string smsCenterAddress;
            std::string smsTypeOfAddress;
            explicit CSCA(const Result &);
            operator bool() const override;
        };
    } // namespace result

    namespace cmd
    {

        class PhoneView;

        class CSCA : public Cmd
        {
          public:
            CSCA() noexcept;
            explicit CSCA(at::cmd::Modifier mod) noexcept;
            [[nodiscard]] auto parseCSCA(const Result &base_result) -> result::CSCA;
            void set(const utils::PhoneNumber::View &smsCenterAddress, int smsTypeOfAddress);
        };
    } // namespace cmd
} // namespace at
