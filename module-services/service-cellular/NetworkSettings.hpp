// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <string>
#include <vector>

#include "service-cellular/CellularMessage.hpp"
#include "service-cellular/CellularServiceAPI.hpp"
#include "service-cellular/ServiceCellular.hpp"
#include "response.hpp"

class NetworkSettings
{

  public:
    explicit NetworkSettings(ServiceCellular &cellularService) : cellularService(cellularService)
    {}
    /**
     * Scan for operator, return string list see param fullInfoList
     * @param fullInfoList for tru scan wit details (as space separated) in case of false scan only
     * for full names without duplicates
     * @return
     */
    std::vector<std::string> scanOperators(bool fullInfoList = false);

    bool setOperatorAutoSelect();
    std::string getCurrentOperator() const;
    bool setOperator(at::response::cops::CopsMode mode, at::response::cops::NameFormat format, const std::string &name);

  private:
    ServiceCellular &cellularService;
};
