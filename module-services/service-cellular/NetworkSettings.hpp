// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <string>
#include <vector>

#include "service-cellular/CellularMessage.hpp"
#include "service-cellular/CellularServiceAPI.hpp"
#include "service-cellular/ServiceCellular.hpp"
#include "response.hpp"

#include <at/cmd/QNWINFO.hpp>

/**
 * Internal (Mudita) VoLTE state, based on on a few
 * modem parameters
 */
enum class VoLTEState
{
    On,
    Off,
    Unknown
};

/**
 * Enum for voice domain preference
 * between circuit-switched (CS) and
 * packet-switched (PS)
 */
enum class VoiceDomainPreference : std::uint8_t
{
    CSOnly      = 0x00,
    PSOnly      = 0x01,
    CSPreferred = 0x02,
    PSPreferred = 0x03
};

enum class SMSDomainPreference : std::uint8_t
{
    PSNotAllowed = 0x00,
    PSPreferred  = 0x01
};

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
    std::string getCurrentOperatorName() const;
    std::optional<at::response::cops::Operator> getCurrentOperator() const;
    bool setOperator(at::response::cops::CopsMode mode, at::response::cops::NameFormat format, const std::string &name);

    enum class SimpleNAT
    {
        GSM,
        UMTS,
        LTE
    };
    std::optional<at::response::cops::AccessTechnology> getCurrentNAT() const;
    std::optional<at::result::QNWINFO> getCurrentNetworkInfo() const;

    static SimpleNAT toSimpleNAT(at::response::cops::AccessTechnology nat);

  private:
    ServiceCellular &cellularService;
};
