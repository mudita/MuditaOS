// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <endpoints/Endpoint.hpp>
#include "Service/Service.hpp"
#include "CalendarEventsHelper.hpp"

using namespace parserFSM;

class CalendarEventsEndpoint : public Endpoint
{
  private:
    std::string debugName = "CalendarEventsEndpoint";
    std::unique_ptr<CalendarEventsHelper> helper;

  public:
    CalendarEventsEndpoint(sys::Service *_ownerServicePtr) : Endpoint(_ownerServicePtr)
    {
        helper = std::make_unique<CalendarEventsHelper>(ownerServicePtr);
    }
    void handle(Context &context) override;
};
