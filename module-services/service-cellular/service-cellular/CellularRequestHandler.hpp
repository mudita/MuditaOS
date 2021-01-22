// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "RequestHandler.hpp"
#include "service-cellular/ServiceCellular.hpp"

class CellularRequestHandler : public cellular::RequestHandler
{
  public:
    CellularRequestHandler(ServiceCellular &serviceCellular) : cellular(serviceCellular)
    {}

    void handle(cellular::ImeiRequest &request, at::Result &result) final;
    void handle(cellular::UssdRequest &request, at::Result &result) final;
    void handle(cellular::CallRequest &request, at::Result &result) final;
    void handle(cellular::PasswordRegistrationRequest &request, at::Result &result) final;
    void handle(cellular::SupplementaryServicesRequest &request, at::Result &result) final;
    void handle(cellular::PinChangeRequest &request, at::Result &result) final;
    void handle(cellular::ClirRequest &request, at::Result &result) final;

  private:
    ServiceCellular &cellular;
    void sendMmiResult(bool result);
};
