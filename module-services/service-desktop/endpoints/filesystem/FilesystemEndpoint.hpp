#pragma once

#include <module-services/service-desktop/endpoints/Endpoint.hpp>
#include "Service/Service.hpp"

using namespace parserFSM;

class FilesystemEndpoint : public Endpoint
{
  public:
    FilesystemEndpoint(sys::Service *ownerServicePtr) : Endpoint(ownerServicePtr)
    {
    }
    auto handle(Context &context) -> void override;
    auto run(Context &context) -> sys::ReturnCodes;
    auto getUpdates(Context &context) -> sys::ReturnCodes;
};
