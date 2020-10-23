// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Service/Service.hpp>

namespace service
{

    class ServiceFileIndexer final : public sys::Service
    {
      public:
        ServiceFileIndexer(const std::string_view name);
        virtual ~ServiceFileIndexer();
        ServiceFileIndexer(const ServiceFileIndexer &) = delete;
        ServiceFileIndexer &operator=(const ServiceFileIndexer &) = delete;
        sys::Message_t DataReceivedHandler(sys::DataMessage *msg, sys::ResponseMessage *resp) override;
        sys::ReturnCodes InitHandler() override;
        sys::ReturnCodes DeinitHandler() override;
        sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override;
    };

}; // namespace service
