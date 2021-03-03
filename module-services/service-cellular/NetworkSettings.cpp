// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "NetworkSettings.hpp"
#include "QMBNManager.hpp"
#include <unordered_map>
#include <at/ATFactory.hpp>

std::string NetworkSettings::getCurrentOperator() const
{
    auto channel = cellularService.cmux->get(TS0710::Channel::Commands);
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

std::vector<std::string> NetworkSettings::scanOperators(bool fullInfoList)
{
    std::vector<std::string> operatorNames;
    auto channel = cellularService.cmux->get(TS0710::Channel::Commands);
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
    auto channel = cellularService.cmux->get(TS0710::Channel::Commands);
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
    auto channel = cellularService.cmux->get(TS0710::Channel::Commands);
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
    auto channel = cellularService.cmux->get(TS0710::Channel::Commands);
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
    auto channel = cellularService.cmux->get(TS0710::Channel::Commands);
    if (!channel) {
        return at::Result::Code::ERROR;
    }

    QMBNManager qmbn(channel);
    auto ret = qmbn.writeNVByte(nv_paths::voice_domain_pref, static_cast<std::uint8_t>(pref));
    return ret;
}

at::Result::Code NetworkSettings::getPreferredSMSDomain(SMSDomainPreference &pref)
{
    auto channel = cellularService.cmux->get(TS0710::Channel::Commands);
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
    auto channel = cellularService.cmux->get(TS0710::Channel::Commands);
    if (!channel) {
        return at::Result::Code::ERROR;
    }

    QMBNManager qmbn(channel);
    auto ret = qmbn.writeNVByte(nv_paths::sms_domain_pref, static_cast<std::uint8_t>(pref));
    return ret;
}

at::Result::Code NetworkSettings::setIMSState(at::response::qcfg_ims::IMSState state)
{
    std::vector<std::string> operatorNames;
    auto channel = cellularService.cmux->get(TS0710::Channel::Commands);
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
    auto channel = cellularService.cmux->get(TS0710::Channel::Commands);
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

    auto channel = cellularService.cmux->get(TS0710::Channel::Commands);
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
    LOG_DEBUG("Modem soft reboot");
    cellularService.resetCellularModule(ServiceCellular::ResetType::SoftReset);
    return at::Result::Code::OK;
}

VoLTEState NetworkSettings::getVoLTEState()
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

    auto channel = cellularService.cmux->get(TS0710::Channel::Commands);
    if (channel) {
        auto resp = channel->cmd("AT+QVOLTEDBG");
        if (resp.code == at::Result::Code::OK) {
            std::string buffer = "";
            for (auto el : resp.response) {
                buffer = el + "\r\n";
            }
            LOG_DEBUG("VOLTEDEBUG:\r\n%s", buffer.c_str());
            return buffer;
        }
    }

    return {};
}
