// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "NetworkSettings.hpp"

#include <unordered_map>

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
