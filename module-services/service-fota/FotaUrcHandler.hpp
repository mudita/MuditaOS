// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "service-fota/ServiceFota.hpp"

#include <module-cellular/at/UrcHandler.hpp>
#include <module-cellular/at/UrcQind.hpp>

using namespace at::urc;

/**
 * ServiceFota helper for handling Urc messages
 */
class FotaUrcHandler : public UrcHandler
{
  public:
    FotaUrcHandler(FotaService::Service &fotaService) : fotaService(fotaService)
    {}

    void Handle(Qind &urc) final;
    virtual void Handle(Clip &urc){};
    virtual void Handle(Creg &urc){};
    virtual void Handle(Cmti &urc){};
    virtual void Handle(Cusd &urc){};
    virtual void Handle(Ctze &urc){};
    virtual void Handle(Cpin &urc){};
    virtual void Handle(Qiurc &urc){};
    virtual void Handle(PoweredDown &urc){};
    virtual void Handle(UrcResponse &urc){};

  private:
    FotaService::Service &fotaService;
};
