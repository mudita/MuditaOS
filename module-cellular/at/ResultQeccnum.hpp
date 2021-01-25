// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Result.hpp>

namespace at
{
    class ResultQueccnum : public at::Result
    {
      public:
        static constexpr auto prefix = "+QECCNUM:";

        enum class EccNumberType
        {
            WithSim,
            WithoutSim,
        };

        ResultQueccnum(){};

        ResultQueccnum(std::vector<std::string> response,
                       std::optional<StatusCode> statusCode,
                       const CmxError &errorCode);

        std::vector<std::string> getEccNumbers(EccNumberType eccType)
        {
            return std::vector<std::string>();
        };

      private:
        std::vector<std::string> eccNumbersSim;
        std::vector<std::string> eccNumbersNoSim;
    };
} // namespace at
