// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <string>

namespace cellular::service
{
    struct OperatorInfo
    {
        OperatorInfo(const std::string &mcc, const std::string &mnc) : MCC(mcc), MNC(mnc)
        {}
        std::string MCC;
        std::string MNC;

        friend bool operator==(const OperatorInfo &lhs, const OperatorInfo &rhs)
        {
            return lhs.MCC == rhs.MCC && lhs.MNC == rhs.MNC;
        }
    };
} // namespace cellular::service
