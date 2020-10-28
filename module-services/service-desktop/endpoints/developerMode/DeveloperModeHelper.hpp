// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <string> // for allocator, string

#include "Common/Query.hpp"
#include "Context.hpp"
#include "Service/Service.hpp"
#include "Service/Common.hpp" // for ReturnCodes

namespace sys
{
    class Service;
} // namespace sys

namespace parserFSM
{
    class Context;

    class DeveloperModeHelper
    {
        sys::Service *ownerServicePtr = nullptr;

      public:
        DeveloperModeHelper(sys::Service *_ownerServicePtr) : ownerServicePtr(_ownerServicePtr){};
        auto processGetRequest(Context &context) -> sys::ReturnCodes;
    };

    namespace json::developerMode
    {
        const inline std::string keyPressed = "keyPressed";
    }
} // namespace parserFSM
