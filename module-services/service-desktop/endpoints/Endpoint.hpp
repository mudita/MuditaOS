#pragma once
#include "module-services/service-desktop/parser/ParserUtils.hpp"
#include "json/json11.hpp"
#include "Common/Query.hpp"
#include "Context.hpp"
#include "Service/Service.hpp"
#include <string>

namespace parserFSM
{

    class Endpoint
    {

      public:
        Endpoint(sys::Service *_ownerServicePtr) : ownerServicePtr(_ownerServicePtr){};
        virtual ~Endpoint()                           = default;
        virtual auto handle(Context &context) -> void = 0;
        auto c_str() -> const char *
        {
            return debugName.c_str();
        }

      protected:
        std::string debugName         = "";
        sys::Service *ownerServicePtr = nullptr;
    };

} // namespace parserFSM
