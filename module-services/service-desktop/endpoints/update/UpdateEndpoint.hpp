#pragma once

#include "Endpoint.hpp"
#include "Service/Service.hpp"

using namespace ParserStateMachine;

class UpdateEndpoint : public Endpoint
{

  public:
    UpdateEndpoint(sys::Service *ownerServicePtr) : Endpoint(ownerServicePtr)
    {
        debugName = "UpdateEndpoint";
    }
    auto handle(Context &context) -> std::string override;
    auto run(Context &context) -> sys::ReturnCodes;
    auto getUpdates(Context &context) -> sys::ReturnCodes;
};
