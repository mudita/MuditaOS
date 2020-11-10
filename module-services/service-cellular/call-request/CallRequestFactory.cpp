// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "CallRequestFactory.hpp"

#include <re2/re2.h>

#include <at/Commands.hpp>
#include <log/log.hpp>

namespace call_request
{

    void Factory::registerRequest(std::string regex, CreateCallback callback)
    {
        requestMap.insert(std::pair<std::string, CreateCallback>(regex, callback));
    }
    std::unique_ptr<IRequest> Factory::create(void)
    {

        re2::StringPiece input(request);

        for (auto element : requestMap) {
            re2::RE2 reg(element.first);
            if (re2::RE2::FullMatch(input, reg)) {
                return element.second(request);
            }
        }

        return std::make_unique<CallRequest>(request);
    }

} // namespace call_request
