// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "VolteAllowedUSList.hpp"
#include <log/log.hpp>
#include <array>
#include <algorithm>

namespace cellular::service
{

    constexpr auto allowedOperatorsCount = 12;
    const std::array<OperatorInfo, allowedOperatorsCount> allowedOperators{OperatorInfo{"310", "053"},
                                                                           OperatorInfo{"310", "120"},
                                                                           OperatorInfo{"310", "260"},
                                                                           OperatorInfo{"310", "530"},
                                                                           OperatorInfo{"310", "770"},
                                                                           OperatorInfo{"311", "490"},
                                                                           OperatorInfo{"311", "660"},
                                                                           OperatorInfo{"311", "880"},
                                                                           OperatorInfo{"311", "882"},
                                                                           OperatorInfo{"312", "190"},
                                                                           OperatorInfo{"312", "250"},
                                                                           OperatorInfo{"312", "530"}};

    auto VolteAllowedUSList::isVolteAllowed(const OperatorInfo &operatorInfo) -> bool
    {
        LOG_INFO("Trying to find MCC: %s, MNC: %s", operatorInfo.MCC.c_str(), operatorInfo.MNC.c_str());

        if (std::find(std::begin(allowedOperators), std::end(allowedOperators), operatorInfo) ==
            std::end(allowedOperators)) {
            LOG_ERROR("Unable to find. VoLTE not allowed.");
            return false;
        }
        return true;
    }
} // namespace cellular::service
