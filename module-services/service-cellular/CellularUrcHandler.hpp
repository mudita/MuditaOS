// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "service-cellular/CellularMessage.hpp"
#include "service-cellular/CellularServiceAPI.hpp"
#include "service-cellular/ServiceCellular.hpp"

#include <module-cellular/at/UrcClip.hpp>
#include <module-cellular/at/UrcCmti.hpp>
#include <module-cellular/at/UrcCreg.hpp>
#include <module-cellular/at/UrcCtze.hpp>
#include <module-cellular/at/UrcCusd.hpp>
#include <module-cellular/at/UrcCpin.hpp>
#include <module-cellular/at/UrcPoweredDown.hpp>
#include <module-cellular/at/UrcQind.hpp>
#include <module-cellular/at/UrcResponse.hpp>
#include <module-cellular/at/UrcQiurc.hpp>

using namespace at::urc;

/**
 * ServiceCellular helper for handling Urc messages
 */
class CellularUrcHandler : public UrcHandler
{
  public:
    CellularUrcHandler(ServiceCellular &cellularService) : cellularService(cellularService)
    {}

    void Handle(Clip &urc) final;
    void Handle(Creg &urc) final;
    void Handle(Cmti &urc) final;
    void Handle(Cusd &urc) final;
    void Handle(Ctze &urc) final;
    void Handle(Qind &urc) final;
    void Handle(Cpin &urc) final;
    void Handle(Qiurc &urc) final;
    void Handle(PoweredDown &urc) final;
    void Handle(UrcResponse &urc) final;

    /**
     * Gets the response that should be returned after handling Urc
     * @return
     */
    std::optional<std::shared_ptr<CellularMessage>> getResponse()
    {
        return std::move(response);
    };

  private:
    ServiceCellular &cellularService;
    std::optional<std::unique_ptr<CellularMessage>> response;
};
