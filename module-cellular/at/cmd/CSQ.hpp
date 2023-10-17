// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Result.hpp"
#include <at/Cmd.hpp>
#include <functional>

namespace at
{
    namespace csq
    {
        constexpr auto tokensCount = 2;

        enum class Tokens
        {
            Csq,
            Ber
        };
    } // namespace csq
    namespace result
    {
        struct CSQ : public Result
        {
            uint32_t csq;
            uint32_t ber;
            explicit CSQ(const Result &);
        };
    } // namespace result

    namespace cmd
    {
        class CSQ : public Cmd
        {
          public:
            CSQ() noexcept;
            explicit CSQ(at::cmd::Modifier mod) noexcept;
            [[nodiscard]] auto parseCSQ(const Result &base_result) -> result::CSQ;
        };
    } // namespace cmd

} // namespace at
