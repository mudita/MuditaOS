#pragma once

#include "Endpoint.hpp"
#include "Service/Service.hpp"
#include "DeveloperModeHelper.hpp"

using namespace parserFSM;

class DeveloperModeEndpoint : public Endpoint
{
  private:
    std::unique_ptr<DeveloperModeHelper> helper;

  public:
    DeveloperModeEndpoint(sys::Service *_ownerServicePtr) : Endpoint(_ownerServicePtr)
    {
        debugName = "DeveloperModeEndpoint";
        helper    = std::make_unique<DeveloperModeHelper>(ownerServicePtr);
    }
    auto handle(Context &context) -> void override;
};
