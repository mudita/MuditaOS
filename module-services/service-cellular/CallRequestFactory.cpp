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
        registerRequest(SupplementaryServicesRegex, SupplementaryServicesRequest::create);

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
            auto const &requestCreateCallback = element.second;
            re2::StringPiece input(request);
            re2::RE2 reg(element.first);

            std::string groupA, groupB, groupC, groupD, groupE;
            GroupMatch matchPack = {groupA, groupB, groupC, groupD, groupE};

            if (re2::RE2::FullMatch(input, reg, &groupA, &groupB, &groupC, &groupD, &groupE)) {
                return requestCreateCallback(request, matchPack);
            }
        }
        return std::make_unique<CallRequest>(request);
    }

} // namespace call_request
