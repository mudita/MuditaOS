// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SimCard.hpp"
#include <log/log.hpp>
#include <variant>
#include "Result.hpp"
#include "UrcCpin.hpp" //for Cpin parseState
#include "UrcFactory.hpp"

SimCardResult SimCard::convertErrorFromATResult(at::Result atres)
{
    if (std::holds_alternative<at::EquipmentErrorCode>(atres.errorCode)) {

        auto cerr = static_cast<int>(std::get<at::EquipmentErrorCode>(atres.errorCode));
        if ((cerr > static_cast<int>(SimCardResult::AT_ERROR_Begin)) &&
            (cerr < static_cast<int>(SimCardResult::AT_ERROR_End))) {
            return static_cast<SimCardResult>(cerr);
        }
    }
    return SimCardResult::Unknown;
}

std::optional<std::vector<std::string>> SimCard::getTokensForATCommand(const at::Result &resp, std::string_view head)
{
    if (resp.code == at::Result::Code::OK) {
        if (resp.response.size()) {
            for (auto el : resp.response) {

                if (el.compare(0, head.length(), head) == 0) {
                    auto body = el.substr(head.length());
                    return utils::split(body, ",");
                }
            }
        }
    }
    return std::nullopt;
}

std::optional<SimCard::AttemptsCounters> SimCard::getAttemptsCounters()
{
    auto channel = cellularService.cmux->get(TS0710::Channel::Commands);
    if (channel) {
        auto resp                          = channel->cmd(at::factory(at::AT::QPINC) + "\"SC\"");
        const std::string_view AT_QPINC_SC = "+QPINC: \"SC\",";
        if (auto tokens = getTokensForATCommand(resp, AT_QPINC_SC); tokens) {
            constexpr int QPINC_TokensCount = 3;
            if ((*tokens).size() == QPINC_TokensCount) {
                AttemptsCounters ret;
                utils::toNumeric((*tokens)[1], ret.PinCounter);
                utils::toNumeric((*tokens)[2], ret.PukCounter);
                return ret;
            }
        }
    }

    return std::nullopt;
}

SimCardResult SimCard::supplyPin(const std::string pin)
{
    if (auto pc = getAttemptsCounters(); pc) {
        if (pc.value().PinCounter > 0) {
            if (auto channel = cellularService.cmux->get(TS0710::Channel::Commands); channel) {
                auto resp = channel->cmd(at::factory(at::AT::CPIN) + "\"" + pin + "\"");

                if (resp.code == at::Result::Code::OK) {
                    return SimCardResult::OK;
                }
                else {
                    return convertErrorFromATResult(resp);
                }
            }
        }
        else {
            if (pc.value().PukCounter > 0) {
                return SimCardResult::SIM_PUKRequired;
            }
            else {
                return SimCardResult::Locked;
            }
        }
    }
    return SimCardResult::Unknown;
}

SimCardResult SimCard::supplyPuk(const std::string puk, const std::string pin)
{
    if (auto pc = getAttemptsCounters(); pc) {
        if (pc.value().PukCounter != 0) {
            if (auto channel = cellularService.cmux->get(TS0710::Channel::Commands); channel) {
                auto resp = channel->cmd(at::factory(at::AT::CPIN) + "\"" + puk + "\"" + ",\"" + pin + "\"");
                if (resp.code == at::Result::Code::OK) {
                    return SimCardResult::OK;
                }
                else {
                    return convertErrorFromATResult(resp);
                }
            }
        }
        else {
            return SimCardResult::Locked;
        }
    }

    return SimCardResult::Unknown;
}

bool SimCard::isPinLocked()
{
    if (auto channel = cellularService.cmux->get(TS0710::Channel::Commands); channel) {
        auto resp                      = channel->cmd(at::factory(at::AT::CLCK) + "\"SC\",2\r");
        const std::string_view AT_CLCK = "+CLCK:";
        if (auto tokens = getTokensForATCommand(resp, AT_CLCK); tokens) {
            if ((*tokens).size() != 0) {
                int val = 0;
                utils::toNumeric((*tokens)[0], val);
                return val != 0;
            }
        }
    }

    return true;
}

SimCardResult SimCard::setPinLock(bool lock, std::string pin)
{
    if (auto pc = getAttemptsCounters(); pc) {
        if (pc.value().PukCounter != 0) {
            auto channel = cellularService.cmux->get(TS0710::Channel::Commands);
            if (channel) {
                auto resp =
                    channel->cmd(at::factory(at::AT::CLCK) + "\"SC\"," + (lock ? "1" : "0") + ",\"" + pin + "\"");
                if (resp.code == at::Result::Code::OK) {
                    return SimCardResult::OK;
                }
                else {
                    return convertErrorFromATResult(resp);
                }
            }
        }
        else {
            return SimCardResult::Locked;
        }
    }

    return SimCardResult::Unknown;
}

SimCardResult SimCard::changePin(std::string oldPin, std::string newPin)
{
    if (auto pc = getAttemptsCounters(); pc) {
        if (pc.value().PukCounter != 0) {
            auto channel = cellularService.cmux->get(TS0710::Channel::Commands);
            if (channel) {
                auto resp = channel->cmd(at::factory(at::AT::CPWD) + "\"SC\", \"" + oldPin + "\",\"" + newPin + "\"");
                if (resp.code == at::Result::Code::OK) {
                    return SimCardResult::OK;
                }
                else {
                    return convertErrorFromATResult(resp);
                }
            }
        }
        else {
            return SimCardResult::Locked;
        }
    }

    return SimCardResult::Unknown;
}

std::optional<at::SimState> SimCard::simState()
{
    std::string buf;
    return simStateWithMessage(buf);
}

std::optional<at::SimState> SimCard::simStateWithMessage(std::string &message)
{
    if (auto channel = cellularService.cmux->get(TS0710::Channel::Commands); channel) {
        auto resp = channel->cmd(at::factory(at::AT::GET_CPIN));
        if (resp.code == at::Result::Code::OK) {
            if (resp.response.size()) {
                for (auto el : resp.response) {
                    auto urc  = at::urc::UrcFactory::Create(el);
                    auto cpin = std::unique_ptr<at::urc::Cpin>{static_cast<at::urc::Cpin *>(urc.release())};
                    if (cpin) {
                        return cpin->getState();
                    }
                }
            }
        }
    }
    return at::SimState::Unknown;
}
