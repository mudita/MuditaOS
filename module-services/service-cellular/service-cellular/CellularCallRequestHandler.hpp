// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "CallRequestHandler.hpp"
#include "service-cellular/ServiceCellular.hpp"

using namespace call_request;

class CellularCallRequestHandler : public CallRequestHandler
{
  public:
    CellularCallRequestHandler(ServiceCellular &serviceCellular) : cellular(serviceCellular)
    {}

    void handle(IMEIRequest &request, at::Result &result) final;
    void handle(MmiRequest &request, at::Result &result) final;
    void handle(USSDRequest &request, at::Result &result) final;
    void handle(CallRequest &request, at::Result &result) final;
  private:
    ServiceCellular &cellular;
};
