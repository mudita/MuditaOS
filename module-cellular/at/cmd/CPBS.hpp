// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Result.hpp"
#include <at/Cmd.hpp>
#include <functional>

namespace at
{
    namespace cpbs
    {
        constexpr auto simStorage  = "\"SM\"";
        constexpr auto tokensCount = 3;

        enum class tokens
        {
            Storage = 0,
            Used    = 1,
            Total   = 2
        };
    } // namespace cpbs
    namespace result
    {
        struct CPBS : public Result
        {
            std::string storage;
            unsigned int used;
            unsigned int total;

            explicit CPBS(const Result &that);
        };
    } // namespace result

    namespace cmd
    {
        class CPBS : public Cmd
        {
          public:
            CPBS() noexcept;
            explicit CPBS(at::cmd::Modifier mod) noexcept;
            [[nodiscard]] auto parseCPBS(const Result &base_result) -> result::CPBS;
            void set(const std::string &storage = cpbs::simStorage);
        };
    } // namespace cmd
} // namespace at
