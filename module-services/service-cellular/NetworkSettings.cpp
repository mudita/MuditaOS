// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "NetworkSettings.hpp"
#include "QMBNManager.hpp"
#include <unordered_map>
#include <at/ATFactory.hpp>
#include <at/cmd/QNWINFO.hpp>

std::string NetworkSettings::getCurrentOperatorName() const
{
    auto channel = cellularService.cmux->get(CellularMux::Channel::Commands);
    if (channel) {
        at::Cmd buildCmd = at::factory(at::AT::COPS) + "?";
        auto resp        = channel->cmd(buildCmd);
        at::response::cops::CurrentOperatorInfo ret;
        if ((resp.code == at::Result::Code::OK) && (at::response::parseCOPS(resp, ret))) {
            if (auto _operator = ret.getOperator(); _operator) {
                return _operator->getNameByFormat(ret.getFormat());
            }
        }
    }

    return {};
}

std::optional<at::response::cops::Operator> NetworkSettings::getCurrentOperator() const
{
    auto channel = cellularService.cmux->get(CellularMux::Channel::Commands);
    if (channel) {
        at::Cmd buildCmd = at::factory(at::AT::COPS) + "?";
        auto resp        = channel->cmd(buildCmd);
        at::response::cops::CurrentOperatorInfo ret;
        if ((resp.code == at::Result::Code::OK) && (at::response::parseCOPS(resp, ret))) {
            return ret.getOperator();
        }
    }

    return {};
}

namespace
{
    constexpr auto cdma1x         = "CDMA1X";
    constexpr auto cdma1xAndHdr   = "CDMA1X AND HDR";
    constexpr auto cmda1xAndEhrdp = "CDMA1X AND EHRPD";
    constexpr auto hdr            = "HDR";
    constexpr auto hdrEhrpd       = "HDR-EHRPD";
    constexpr auto gsm            = "GSM";
    constexpr auto edge           = "EDGE";
    constexpr auto wcdma          = "WCDMA";
    constexpr auto hsdpa          = "HSDPA";
    constexpr auto hsupa          = "HSUPA";
    constexpr auto hspaPlus       = "HSPA+";
    constexpr auto tdscdma        = "TDSCDMA";
    constexpr auto tddLte         = "TDD LTE";
    constexpr auto fddLte         = "FDD LTE";

    std::map<std::string_view, at::response::cops::AccessTechnology> technologyMap = {
        {cdma1x, at::response::cops::AccessTechnology::CDMA},
        {cdma1xAndHdr, at::response::cops::AccessTechnology::CDMA},
        {cmda1xAndEhrdp, at::response::cops::AccessTechnology::CDMA},
        {hdr, at::response::cops::AccessTechnology::UTRAN},
        {hdrEhrpd, at::response::cops::AccessTechnology::UTRAN},
        {gsm, at::response::cops::AccessTechnology::GSM},
        {edge, at::response::cops::AccessTechnology::GSM_W_EGPRS},
        {wcdma, at::response::cops::AccessTechnology::UTRAN},
        {hsdpa, at::response::cops::AccessTechnology::UTRAN_W_HSDPA},
        {hsupa, at::response::cops::AccessTechnology::UTRAN_W_HSUPA},
        {hspaPlus, at::response::cops::AccessTechnology::UTRAN},
        {tdscdma, at::response::cops::AccessTechnology::UTRAN},
        {tddLte, at::response::cops::AccessTechnology::E_UTRAN},
        {fddLte, at::response::cops::AccessTechnology::E_UTRAN}};
} // namespace

std::optional<at::result::QNWINFO> NetworkSettings::getCurrentNetworkInfo() const
{
    auto channel = cellularService.cmux->get(CellularMux::Channel::Commands);
    if (channel) {
        auto command = at::cmd::QNWINFO();
        auto result  = channel->cmd(command);
        if (auto response = command.parseQNWINFO(result); response) {
            return response;
        }
    }
    return {};
}

NetworkSettings::SimpleNAT NetworkSettings::toSimpleNAT(at::response::cops::AccessTechnology nat)
{
    switch (nat) {
    case at::response::cops::AccessTechnology::GSM:
    case at::response::cops::AccessTechnology::GSM_W_EGPRS:
        return SimpleNAT::GSM;
    case at::response::cops::AccessTechnology::UTRAN:
    case at::response::cops::AccessTechnology::UTRAN_W_HSDPA:
    case at::response::cops::AccessTechnology::UTRAN_W_HSUPA:
    case at::response::cops::AccessTechnology::UTRAN_W_HSDPA_and_HSUPA:
    case at::response::cops::AccessTechnology::CDMA:
        return SimpleNAT::UMTS;
    case at::response::cops::AccessTechnology::E_UTRAN:
        return SimpleNAT::LTE;
    }
    return SimpleNAT::GSM;
}

std::optional<at::response::cops::AccessTechnology> NetworkSettings::getCurrentNAT() const
{
    const auto currentOperator = getCurrentOperator();
    if (currentOperator && currentOperator->technology) {
        return currentOperator->technology;
    }

    const auto currentNetworkInfo = getCurrentNetworkInfo();
    if (currentNetworkInfo) {
        auto it = technologyMap.find(currentNetworkInfo->act);
        if (it != technologyMap.end()) {
            return it->second;
        }
    }

    return {};
}

std::vector<std::string> NetworkSettings::scanOperators(bool fullInfoList)
{
    std::vector<std::string> operatorNames;
    auto channel = cellularService.cmux->get(CellularMux::Channel::Commands);
    if (channel) {
        at::Cmd buildCmd = at::factory(at::AT::COPS) + "=?";

        auto resp = channel->cmd(buildCmd);

        std::vector<at::response::cops::Operator> ret;

        if ((resp.code == at::Result::Code::OK) && (at::response::parseCOPS(resp, ret))) {
            std::vector<at::response::cops::Operator> uniqueOperators;

            if (fullInfoList) {

                std::transform(ret.begin(),
                               ret.end(),
                               std::back_inserter(operatorNames),
                               [](at::response::cops::Operator op) -> std::string {
                                   return op.longName + " " + op.numericName + " " + utils::enumToString(op.status) +
                                          " " + ((op.technology) ? utils::enumToString(*op.technology) : "");
                               });
            }
            else {
                /// remove duplicated operator by numeric value to save one name in original form, eg.
                /// (2,"PLAY","PLAY","26006",2),(1,"Play","Play","26006",0),
                std::sort(
                    ret.begin(), ret.end(), [](at::response::cops::Operator op1, at::response::cops::Operator op2) {
                        return op1.numericName > op2.numericName;
                    });

                std::unique_copy(ret.begin(),
                                 ret.end(),
                                 std::back_inserter(uniqueOperators),
                                 [](at::response::cops::Operator op1, at::response::cops::Operator op2) {
                                     return op1.numericName == op2.numericName;
                                 });

                std::transform(uniqueOperators.begin(),
                               uniqueOperators.end(),
                               std::back_inserter(operatorNames),
                               [](at::response::cops::Operator op) -> std::string { return op.longName; });
            }
        }
    }

    return operatorNames;
}

bool NetworkSettings::setOperatorAutoSelect()
{
    auto channel = cellularService.cmux->get(CellularMux::Channel::Commands);
    if (!channel) {
        return false;
    }

    at::Cmd buildCmd =
        at::factory(at::AT::COPS) + "=" + utils::to_string(static_cast<int>(at::response::cops::CopsMode::Automatic));
    auto resp = channel->cmd(buildCmd);
    return (resp.code == at::Result::Code::OK);
}
bool NetworkSettings::setOperator(at::response::cops::CopsMode mode,
                                  at::response::cops::NameFormat format,
                                  const std::string &name)
{
    auto channel = cellularService.cmux->get(CellularMux::Channel::Commands);
    if (!channel) {
        return false;
    }

    at::Cmd buildCmd = at::factory(at::AT::COPS) + "=" + utils::to_string(static_cast<int>(mode)) + "," +
                       utils::to_string(static_cast<int>(format)) + ",\"" + name + "\"";

    auto resp = channel->cmd(buildCmd);
    return (resp.code == at::Result::Code::OK);
}

at::Result::Code NetworkSettings::getPreferredVoiceDomain(VoiceDomainPreference &pref)
{
    auto channel = cellularService.cmux->get(CellularMux::Channel::Commands);
    if (!channel) {
        return at::Result::Code::ERROR;
    }

    QMBNManager qmbn(channel);
    std::uint8_t retval;
    auto ret = qmbn.readNVByte(nv_paths::voice_domain_pref, retval);
    if (ret == at::Result::Code::OK) {
        pref = static_cast<VoiceDomainPreference>(retval);
    }

    return ret;
}

at::Result::Code NetworkSettings::setPreferredVoiceDomain(VoiceDomainPreference pref)
{
    auto channel = cellularService.cmux->get(CellularMux::Channel::Commands);
    if (!channel) {
        return at::Result::Code::ERROR;
    }

    QMBNManager qmbn(channel);
    auto ret = qmbn.writeNVByte(nv_paths::voice_domain_pref, static_cast<std::uint8_t>(pref));
    return ret;
}

at::Result::Code NetworkSettings::getPreferredSMSDomain(SMSDomainPreference &pref)
{
    auto channel = cellularService.cmux->get(CellularMux::Channel::Commands);
    if (!channel) {
        return at::Result::Code::ERROR;
    }

    QMBNManager qmbn(channel);
    std::uint8_t retval;
    auto ret = qmbn.readNVByte(nv_paths::sms_domain_pref, retval);
    if (ret == at::Result::Code::OK) {
        pref = static_cast<SMSDomainPreference>(retval);
    }

    return ret;
}

at::Result::Code NetworkSettings::setPreferredSMSDomain(SMSDomainPreference pref)
{
    auto channel = cellularService.cmux->get(CellularMux::Channel::Commands);
    if (!channel) {
        return at::Result::Code::ERROR;
    }

    QMBNManager qmbn(channel);
    auto ret = qmbn.writeNVByte(nv_paths::sms_domain_pref, static_cast<std::uint8_t>(pref));
    return ret;
}

at::Result::Code NetworkSettings::setIMSState(at::response::qcfg_ims::IMSState state)
{
    auto channel = cellularService.cmux->get(CellularMux::Channel::Commands);
    if (!channel) {
        return at::Result::Code::ERROR;
    }
    at::Cmd buildCmd = at::factory(at::AT::QCFG_IMS) + "," + utils::to_string(static_cast<int>(state));
    auto resp        = channel->cmd(buildCmd);
    return resp.code;
}

std::optional<std::pair<at::response::qcfg_ims::IMSState, at::response::qcfg_ims::VoLTEIMSState>> NetworkSettings::
    getIMSState()
{
    std::vector<std::string> operatorNames;
    auto channel = cellularService.cmux->get(CellularMux::Channel::Commands);
    if (channel) {

        at::Cmd buildCmd = at::factory(at::AT::QCFG_IMS);
        auto resp        = channel->cmd(buildCmd);
        std::pair<at::response::qcfg_ims::IMSState, at::response::qcfg_ims::VoLTEIMSState> ret;
        if ((resp.code == at::Result::Code::OK) && (at::response::parseQCFG_IMS(resp, ret))) {
            return ret;
        }
    }

    return std::nullopt;
}

at::Result::Code NetworkSettings::setVoLTEState(VoLTEState state)
{
    /**
     * VoLTE On scenario
     * 1^) auto select off
     * 2) voice domain pref to PS,  [sms domain pref to PS - optional]
     * 3) NV enable VoLTE
     * 4^) select ROW_Generic_3GPP, also could be Volte_OpenMkt-Comercial-CMCC  << could need reboot
     * 5) set QCFG ims to 1
     * 6) If need reboot modem AT+CFUN=1,1
     *
     * ^ optional if not work, should be tested with operator
     *
     *VoLTE off switch
     * 1) "ims" QCFG to 0
     * 2) autoselect of MBN to On
     * 3) Reboot
     */

    auto channel = cellularService.cmux->get(CellularMux::Channel::Commands);
    if (!channel) {
        return at::Result::Code::ERROR;
    }

    QMBNManager qmbn(channel);
    if (state == VoLTEState::On) {
        setPreferredVoiceDomain(VoiceDomainPreference::PSPreferred);
        qmbn.setAutoSelect(at::response::qmbncfg::MBNAutoSelect::On);
        setIMSState(at::response::qcfg_ims::IMSState::Enable);
    }
    else {
        setIMSState(at::response::qcfg_ims::IMSState::Disable);
        qmbn.setAutoSelect(at::response::qmbncfg::MBNAutoSelect::On);
    }
    LOG_DEBUG("Modem hard reboot");
    return at::Result::Code::OK;
}

VoLTEState NetworkSettings::getVoLTEConfigurationState()
{
    /** VoLTE on could be detect in general by one parameter,
     * based on enable IP multimedia services ON (via QCFG="ims")
     * but could be completed based on
     * NV state of VoLTE Disable
     *
     * This is information about configuration setup, not information
     * about whether the VoLTE actually works in the selected network
     *
     */
    if (auto state = getIMSState(); state) {
        /// check disable in any other state VoLTE is likely
        return state->first == at::response::qcfg_ims::IMSState::Disable ? VoLTEState::Off : VoLTEState::On;
    }
    return VoLTEState::Unknown;
}

std::string NetworkSettings::printVoLTEDebug()
{
    auto channel = cellularService.cmux->get(CellularMux::Channel::Commands);
    if (channel) {
        auto resp = channel->cmd("AT+QVOLTEDBG");
        if (resp.code == at::Result::Code::OK) {
            std::string buffer = "";
            for (auto el : resp.response) {
                buffer = el + "\r\n";
            }
            return buffer;
        }
    }
    return {};
}
