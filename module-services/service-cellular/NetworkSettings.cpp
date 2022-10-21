// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "NetworkSettings.hpp"
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
