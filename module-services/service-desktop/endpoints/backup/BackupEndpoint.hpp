// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Endpoint.hpp"
#include "ParserUtils.hpp"
#include "ServiceDesktop.hpp"

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
