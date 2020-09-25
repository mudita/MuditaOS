#pragma once

#include "Endpoint.hpp"
#include "Service/Service.hpp"

using namespace parserFSM;

class FilesystemEndpoint : public Endpoint
{
  public:
    FilesystemEndpoint(sys::Service *ownerServicePtr) : Endpoint(ownerServicePtr)
        {
            debugName = "FilesystemEndpoint";
        }
    auto handle(Context &context) -> void override;
    auto run(Context &context) -> sys::ReturnCodes;
    auto getUpdates(Context &context) -> sys::ReturnCodes;
};
