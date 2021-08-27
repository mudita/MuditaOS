// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
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
#include <module-cellular/at/UrcRing.hpp>

/**
 * ServiceCellular helper for handling Urc messages
 */
class CellularUrcHandler : public at::urc::UrcHandler
{
  public:
    explicit CellularUrcHandler(ServiceCellular &cellularService) : cellularService(cellularService)
    {}

    void Handle(at::urc::Clip &urc) final;
    void Handle(at::urc::Creg &urc) final;
    void Handle(at::urc::Cmti &urc) final;
    void Handle(at::urc::Cusd &urc) final;
    void Handle(at::urc::Ctze &urc) final;
    void Handle(at::urc::Qind &urc) final;
    void Handle(at::urc::Cpin &urc) final;
    void Handle(at::urc::Qiurc &urc) final;
    void Handle(at::urc::Ring &urc) final;
    void Handle(at::urc::PoweredDown &urc) final;
    void Handle(at::urc::UrcResponse &urc) final;

    /**
     * Gets the response that should be returned after handling Urc
     * @return
     */
    std::optional<std::shared_ptr<sys::Message>> getResponse()
    {
        return std::move(response);
    };

  private:
    ServiceCellular &cellularService;
    std::optional<std::unique_ptr<sys::Message>> response;
};
