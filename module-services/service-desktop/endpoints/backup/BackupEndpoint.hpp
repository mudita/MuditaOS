// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <string> // for string

#include "Endpoint.hpp"    // for Endpoint
#include "ParserUtils.hpp" // for parserFSM
#include "ServiceDesktop.hpp"
#include "Service/Common.hpp" // for ReturnCodes

namespace parserFSM
{
    class Context;
} // namespace parserFSM
namespace sys
{
    class Service;
} // namespace sys

using namespace parserFSM;

class BackupEndpoint : public Endpoint
{
  public:
    BackupEndpoint(sys::Service *ownerServicePtr) : Endpoint(ownerServicePtr)
    {
        debugName = "BackupEndpoint";
    }
    auto handle(Context &context) -> void override;
    auto request(Context &context) -> sys::ReturnCodes;
    auto upload(Context &context) -> sys::ReturnCodes;
};
