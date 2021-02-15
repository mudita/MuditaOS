// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <string>
#include <vector>

#include "service-cellular/CellularMessage.hpp"
#include "service-cellular/CellularServiceAPI.hpp"
#include "service-cellular/ServiceCellular.hpp"
#include "response.hpp"

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
    at::Result::Code setVoLTEState(VoLTEState state);
    VoLTEState getVoLTEState();

    at::Result::Code getPreferredVoiceDomain(VoiceDomainPreference &pref);
    at::Result::Code setPreferredVoiceDomain(VoiceDomainPreference pref);

    bool setOperatorAutoSelect();
    std::string getCurrentOperator() const;
    at::Result::Code getPreferredSMSDomain(SMSDomainPreference &pref);
    at::Result::Code setPreferredSMSDomain(SMSDomainPreference pref);
    bool setOperator(at::response::cops::CopsMode mode, at::response::cops::NameFormat format, const std::string &name);

    at::Result::Code setIMSState(at::response::qcfg_ims::IMSState state);
    std::optional<std::pair<at::response::qcfg_ims::IMSState, at::response::qcfg_ims::VoLTEIMSState>> getIMSState();

    std::string printVoLTEDebug();

  private:
    ServiceCellular &cellularService;
};
