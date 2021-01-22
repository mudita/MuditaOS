// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <endpoints/Endpoint.hpp>
#include "Service/Service.hpp"
#include "CalendarEventsHelper.hpp"

class CalendarEventsEndpoint : public parserFSM::Endpoint
{
  private:
    std::string debugName = "CalendarEventsEndpoint";
    std::unique_ptr<parserFSM::CalendarEventsHelper> helper;

  public:
    CalendarEventsEndpoint(sys::Service *_ownerServicePtr) : Endpoint(_ownerServicePtr)
    {
        helper = std::make_unique<parserFSM::CalendarEventsHelper>(ownerServicePtr);
    }
    void handle(parserFSM::Context &context) override;
};
