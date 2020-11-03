// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <module-cellular/at/URCHandler.hpp>
#include <module-cellular/at/URC_QIND.hpp>
#include "ServiceFota.hpp"

using namespace at::urc;

/**
 * ServiceFota helper for handling URC messages
 */
class FotaURCHandler : public URCHandler
{
  public:
    FotaURCHandler(FotaService::Service &fotaService) : fotaService(fotaService)
    {}

    void Handle(QIND &urc) final;

  private:
    FotaService::Service &fotaService;
};
