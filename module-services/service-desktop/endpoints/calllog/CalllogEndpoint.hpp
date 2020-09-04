#pragma once

#include "Endpoint.hpp"
#include "Service/Service.hpp"
#include "CalllogHelper.hpp"

using namespace parserFSM;

class CalllogEndpoint : public Endpoint
{
  private:
    std::unique_ptr<CalllogHelper> helper;

  public:
    CalllogEndpoint(sys::Service *_ownerServicePtr) : Endpoint(_ownerServicePtr)
    {
        debugName = "CalllogEndpoint";
        helper    = std::make_unique<CalllogHelper>(ownerServicePtr);
    }
    auto handle(Context &context) -> void override;
};
