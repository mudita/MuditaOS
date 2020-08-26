#pragma once

#include "Endpoint.hpp"
#include "Service/Service.hpp"
#include "MessageHelper.hpp"

using namespace parserFSM;

class MessagesEndpoint : public Endpoint
{
  private:
    std::shared_ptr<MessageHelper> helper;

  public:
    MessagesEndpoint(sys::Service *_ownerServicePtr) : Endpoint(_ownerServicePtr)
    {
        debugName = "MessagesEndpoint";
        helper    = std::make_shared<MessageHelper>(ownerServicePtr);
    }
    auto handle(Context &context) -> void override;
};
