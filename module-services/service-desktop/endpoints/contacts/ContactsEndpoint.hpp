#pragma once

#include "Endpoint.hpp"
#include "Service/Service.hpp"
#include "ContactHelper.hpp"

using namespace parserFSM;

class ContactsEndpoint : public Endpoint
{
  private:
    std::unique_ptr<ContactHelper> helper;

  public:
    ContactsEndpoint(sys::Service *_ownerServicePtr) : Endpoint(_ownerServicePtr)
    {
        debugName = "ContactsEndpoint";
        helper    = std::make_unique<ContactHelper>(ownerServicePtr);
    }
    auto handle(Context &context) -> void override;
};
