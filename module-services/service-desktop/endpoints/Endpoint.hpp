// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Context.hpp"
#include <parser/ParserUtils.hpp>

#include <json/json11.hpp>
#include <Common/Query.hpp>
#include <Service/Service.hpp>
#include <string>
#include <memory>

namespace parserFSM
{

    class Endpoint
    {

      public:
        Endpoint(sys::Service *_ownerServicePtr) : ownerServicePtr(_ownerServicePtr){};
        virtual ~Endpoint()                           = default;
        virtual auto handle(parserFSM::Context &context) -> void = 0;
        auto c_str() -> const char *
        {
            return debugName.c_str();
        }

      protected:
        std::string debugName         = "";
        sys::Service *ownerServicePtr = nullptr;
    };

} // namespace parserFSM
