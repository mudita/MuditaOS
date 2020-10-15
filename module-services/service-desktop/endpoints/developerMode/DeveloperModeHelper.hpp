#pragma once

#include "Common/Query.hpp"
#include "Context.hpp"
#include "Service/Service.hpp"
#include "Service/Common.hpp"

namespace parserFSM
{

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
