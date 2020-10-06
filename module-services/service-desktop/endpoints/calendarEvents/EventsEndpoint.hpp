#pragma once

#include "Endpoint.hpp"
#include "Service/Service.hpp"
#include "EventsHelper.hpp"

using namespace parserFSM;

class EventsEndpoint : public Endpoint
{
  private:
    std::shared_ptr<EventsHelper> helper;

  public:
    EventsEndpoint(sys::Service *_ownerServicePtr) : Endpoint(_ownerServicePtr)
    {
        debugName = "EventsEndpoint";
        helper    = std::make_shared<EventsHelper>(ownerServicePtr);
    }
    auto handle(Context &context) -> void override;
};
