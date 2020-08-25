#pragma once

#include "Endpoint.hpp"
#include "Service/Service.hpp"

using namespace ParserStateMachine;

class FactoryResetEndpoint : public Endpoint
{

  public:
    FactoryResetEndpoint(sys::Service *ownerServicePtr) : Endpoint(ownerServicePtr)
    {
        debugName = "FactoryResetEndpoint";
    }
    auto handle(Context &context) -> std::string override;
    auto makeFactoryReset(Context &context) -> bool;
};
