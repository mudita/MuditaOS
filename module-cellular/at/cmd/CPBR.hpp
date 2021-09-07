// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Result.hpp"
#include <at/Cmd.hpp>
#include <functional>

namespace at
{
    namespace cpbr
    {
        constexpr auto tokensCount = 4;
        enum class Tokens
        {
            Index = 0,
            Number,
            Type,
            Name
        };

        enum class ContactType
        {
            Unknown       = 129,
            International = 145,
            National      = 161
        };

        struct Contact
        {
            int index;
            std::string number;
            ContactType type;
            std::string name;
        };
    } // namespace cpbr
    namespace result
    {

        struct CPBR : public Result
        {
            std::vector<cpbr::Contact> contacts;
            explicit CPBR(const Result &that);
        };
    } // namespace result

    namespace cmd
    {
        class CPBR : public Cmd
        {
          public:
            CPBR() noexcept;
            explicit CPBR(at::cmd::Modifier mod) noexcept;
            [[nodiscard]] auto parseCPBR(const Result &base_result) -> result::CPBR;
            void setSimContactsReadRange(int firstIndex, int lastIndex);
        };
    } // namespace cmd
} // namespace at
