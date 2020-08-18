#pragma once

#include "Endpoint.hpp"
#include "Service/Service.hpp"
#include "ContactHelper.hpp"

using namespace ParserStateMachine;

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
    auto handle(Context &context) -> std::string override;
};
