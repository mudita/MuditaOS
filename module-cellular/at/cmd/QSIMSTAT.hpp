// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Result.hpp"
#include <at/Cmd.hpp>
#include <functional>
#include <SimInsertedState.hpp>

namespace at
{
    namespace result
    {
        struct QSIMSTAT : public Result
        {
            at::SimInsertedStatusEnable enabled;
            at::SimInsertedStatus status;
            explicit QSIMSTAT(const Result &that);
        };
    } // namespace result

    namespace cmd
    {
        class QSIMSTAT : public Cmd
        {
          public:
            QSIMSTAT() noexcept;
            explicit QSIMSTAT(at::cmd::Modifier mod) noexcept;

            [[nodiscard]] auto parseQSIMSTAT(const Result &base_result) -> result::QSIMSTAT;

          private:
            enum class responseTokens
            {
                Enabled,
                SimInserted
            };
            void parseTokens(const std::vector<std::string> &tokens, result::QSIMSTAT &parsed);
        };
    } // namespace cmd

} // namespace at
