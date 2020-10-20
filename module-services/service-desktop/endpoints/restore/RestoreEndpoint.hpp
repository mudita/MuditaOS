// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Endpoint.hpp"
#include "ParserUtils.hpp"
#include "ServiceDesktop.hpp"

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
