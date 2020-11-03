// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <module-cellular/at/URCHandler.hpp>

#include <module-cellular/at/URC_CLIP.hpp>
#include <module-cellular/at/URC_CREG.hpp>
#include <module-cellular/at/URC_CMTI.hpp>
#include <module-cellular/at/URC_CUSD.hpp>
#include <module-cellular/at/URC_CTZE.hpp>
#include <module-cellular/at/URC_QIND.hpp>
#include <module-cellular/at/URC_POWERED_DOWN.hpp>
#include <module-cellular/at/URC_RESPONSE.hpp>

using namespace at::urc;

#include "messages/CellularMessage.hpp"
#include "api/CellularServiceAPI.hpp"
#include "ServiceCellular.hpp"

/**
 * ServiceCellular helper for handling URC messages
 */
class CellularURCHandler : public URCHandler
{
  public:
    CellularURCHandler(ServiceCellular &cellularService) : cellularService(cellularService)
    {}

    void Handle(CLIP &urc) final;
    void Handle(CREG &urc) final;
    void Handle(CMTI &urc) final;
    void Handle(CUSD &urc) final;
    void Handle(CTZE &urc) final;
    void Handle(QIND &urc) final;
    void Handle(POWERED_DOWN &urc) final;
    void Handle(URC_RESPONSE &urc) final;

    /**
     * Gets the response that should be returned after handling URC
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
