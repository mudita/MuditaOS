// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ResultQeccnum.hpp"
#include "Utils.hpp"

at::ResultQueccnum::ResultQueccnum(std::vector<std::string> response,
                                   std::optional<StatusCode> statusCode,
                                   const at::Result::CmxError &errorCode)
    : Result(response, statusCode, errorCode)
{
    for (auto &responseLine : getResponse()) {
        if (responseLine.find(prefix) == 0) {
            auto parsed = utils::split(std::string(responseLine, std::string(prefix).size()), ',');
            if (parsed.size() <= 1) {
                continue;
            }
            int category = 0;
            if (utils::toNumeric(parsed.front(), category)) {
                if (category == 0) {
                    eccNumbersNoSim = std::vector<std::string>(parsed.begin() + 1, parsed.end());
                }
                else if (category == 1) {
                    eccNumbersSim = std::vector<std::string>(parsed.begin() + 1, parsed.end());
                }
            }
        }
    }
}
