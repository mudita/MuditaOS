// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "service-cellular/CallRequestFactory.hpp"

#include <re2/re2.h>

#include <at/Commands.hpp>
#include <log/log.hpp>

namespace call_request
{
    constexpr inline auto IMEIRegex = "(\\*#06#)";
    constexpr inline auto USSDRegex = "^[\\*].*[\\#]$";

    Factory::Factory(const std::string &data) : request(data)
    {
        registerRequest(IMEIRegex, IMEIRequest::create);

        /*It have to be last check due to 3GPP TS 22.030*/
        registerRequest(USSDRegex, USSDRequest::create);
    }

    void Factory::registerRequest(std::string regex, CreateCallback callback)
    {
        requestMap.insert(std::make_pair(regex, callback));
    }

    std::unique_ptr<IRequest> Factory::create()
    {
        for (const auto &element : requestMap) {
            re2::StringPiece input(request);
            re2::RE2 reg(element.first);
            if (re2::RE2::FullMatch(input, reg)) {
                return element.second(request);
            }
        }
        return std::make_unique<CallRequest>(request);
    }

} // namespace call_request
