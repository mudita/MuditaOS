// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Result.hpp"
#include <at/Cmd.hpp>

namespace at
{
    namespace result
    {
        /// please see documentation:
        /// QuectelEC2526EC21ATCommandsManualV13.1100970659
        /// page: 85 for more information
        /// +QNWINFO: <Act>,<oper>,<band>,<channel>
        struct QNWINFO : public Result
        {
            std::string act;  /// The Access Technology Selected
            int op = 0;       /// The operator in numeric format
            std::string band; /// The band selectedChannel
            int channel = 0;  /// Channel ID
            explicit QNWINFO(const Result &);
            operator bool() const override;
        };

        enum QNWINFOTokens
        {
            act,
            op,
            band,
            channel
        };
    } // namespace result

    namespace cmd
    {

        class QNWINFO : public Cmd
        {
          public:
            QNWINFO() noexcept;
            explicit QNWINFO(at::cmd::Modifier mod) noexcept;
            [[nodiscard]] auto parseQNWINFO(const Result &base_result) -> result::QNWINFO;
        };
    } // namespace cmd
} // namespace at
