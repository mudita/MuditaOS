// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <endpoints/Endpoint.hpp>
#include <parser/ParserUtils.hpp>
#include <service-desktop/ServiceDesktop.hpp>

#include <string>

namespace parserFSM
{
    class Context;
} // namespace parserFSM
namespace sys
{
    class Service;
} // namespace sys

class RestoreEndpoint : public parserFSM::Endpoint
{
  public:
    RestoreEndpoint(sys::Service *ownerServicePtr) : Endpoint(ownerServicePtr)
    {
        debugName = "RestoreEndpoint";
    }
    auto handle(parserFSM::Context &context) -> void override;
};
