#pragma once

#include "Endpoint.hpp"
#include "Service/Service.hpp"

using namespace parserFSM;

class FactoryResetEndpoint : public Endpoint
{

  public:
    FactoryResetEndpoint(sys::Service *ownerServicePtr) : Endpoint(ownerServicePtr)
    {
        debugName = "FactoryResetEndpoint";
    }
    auto handle(Context &context) -> void override;
    auto makeFactoryReset(Context &context) -> bool;
};
