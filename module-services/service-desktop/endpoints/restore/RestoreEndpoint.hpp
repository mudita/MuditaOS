// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Endpoint.hpp"    // for Endpoint
#include "ParserUtils.hpp" // for parserFSM
#include "ServiceDesktop.hpp"

#include <string> // for string

namespace parserFSM
{
    class Context;
} // namespace parserFSM
namespace sys
{
    class Service;
} // namespace sys

using namespace parserFSM;

class RestoreEndpoint : public Endpoint
{
  public:
    RestoreEndpoint(sys::Service *ownerServicePtr) : Endpoint(ownerServicePtr)
    {
        debugName = "RestoreEndpoint";
    }
    auto handle(Context &context) -> void override;
};
