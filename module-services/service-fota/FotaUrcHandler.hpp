// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "service-fota/ServiceFota.hpp"

#include <module-cellular/at/UrcHandler.hpp>
#include <module-cellular/at/UrcQind.hpp>

/**
 * ServiceFota helper for handling Urc messages
 */
class FotaUrcHandler : public at::urc::UrcHandler
{
  public:
    FotaUrcHandler(FotaService::Service &fotaService) : fotaService(fotaService)
    {}

    void Handle(at::urc::Qind &urc) final;
    virtual void Handle(at::urc::Clip &urc){};
    virtual void Handle(at::urc::Creg &urc){};
    virtual void Handle(at::urc::Cmti &urc){};
    virtual void Handle(at::urc::Cusd &urc){};
    virtual void Handle(at::urc::Ctze &urc){};
    virtual void Handle(at::urc::Cpin &urc){};
    virtual void Handle(at::urc::Qiurc &urc){};
    virtual void Handle(at::urc::Ring &urc){};
    virtual void Handle(at::urc::PoweredDown &urc){};
    virtual void Handle(at::urc::UrcResponse &urc){};
    virtual void Handle(at::urc::QSimstat &urc){};

  private:
    FotaService::Service &fotaService;
};
