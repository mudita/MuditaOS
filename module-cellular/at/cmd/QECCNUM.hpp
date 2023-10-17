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
        /// more details: EC25&EC21_AT_Commands_Manual_V1.3
        class QECCNUM : public Result
        {
          public:
            explicit QECCNUM(const Result &);

            std::vector<std::string> eccNumbersNoSim;
            std::vector<std::string> eccNumbersSim;
        };
    } // namespace result

    namespace cmd
    {
        class QECCNUM : public Cmd
        {
          public:
            enum class Mode
            {
                Query,
                Add,
                Delete
            };

            enum class NumberType
            {
                WithSim,
                WithoutSim,
            };

            QECCNUM() noexcept;
            explicit QECCNUM(Mode mode, NumberType numberType, const std::vector<std::string> &number) noexcept;

            [[nodiscard]] auto parseQECCNUM(const Result &base_result) -> result::QECCNUM;

          private:
            void setRequestParameters(Mode mode, NumberType numberType, const std::vector<std::string> &numbers);

            static constexpr auto qeccnumCmd      = "+QECCNUM";
            static constexpr auto commandPostfix  = ":";
            static constexpr auto dataSeparator   = ',';
            static constexpr auto stringDelimiter = '\"';
        };
    } // namespace cmd

} // namespace at
