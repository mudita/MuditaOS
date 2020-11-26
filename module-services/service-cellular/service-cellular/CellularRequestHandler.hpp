// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "RequestHandler.hpp"
#include "service-cellular/ServiceCellular.hpp"

using namespace cellular;

class CellularRequestHandler : public RequestHandler
{
  public:
    CellularRequestHandler(ServiceCellular &serviceCellular) : cellular(serviceCellular)
    {}

    void handle(ImeiRequest &request, at::Result &result) final;
    void handle(UssdRequest &request, at::Result &result) final;
    void handle(CallRequest &request, at::Result &result) final;
    void handle(PasswordRegistrationRequest &request, at::Result &result) final;
    void handle(SupplementaryServicesRequest &request, at::Result &result) final;
    void handle(PinChangeRequest &request, at::Result &result) final;

  private:
    ServiceCellular &cellular;
    void sendMmiResult(bool result);
};
