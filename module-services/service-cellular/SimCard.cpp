// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SimCard.hpp"
#include <log/log.hpp>
#include <variant>
#include "Result.hpp"
#include "UrcCpin.hpp" //for Cpin parseState
#include "UrcFactory.hpp"

SimCardResult SimCard::convertErrorFromATResult(const at::Result atres) const
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

std::optional<at::response::qpinc::AttemptsCounters> SimCard::getAttemptsCounters(const std::string &type) const
{
    auto channel = cellularService.cmux->get(TS0710::Channel::Commands);
    if (channel) {
        auto resp = channel->cmd(at::factory(at::AT::QPINC) + "\"" + type + "\"");
        at::response::qpinc::AttemptsCounters ret;
        if (at::response::parseQPINC(resp, ret)) {
            return ret;
        }
    }

    return std::nullopt;
}

SimCardResult SimCard::supplyPin(const std::string pin) const
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

SimCardResult SimCard::supplyPuk(const std::string puk, const std::string pin) const
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

bool SimCard::isPinLocked() const
{
    if (auto channel = cellularService.cmux->get(TS0710::Channel::Commands); channel) {
        auto resp = channel->cmd(at::factory(at::AT::CLCK) + "\"SC\",2\r");
        int val   = 0;
        if (at::response::parseCLCK(resp, val)) {
            return val != 0;
        }
    }
    return true;
}

SimCardResult SimCard::setPinLock(bool lock, const std::string &pin) const
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

SimCardResult SimCard::changePin(const std::string oldPin, const std::string newPin) const
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

std::optional<at::SimState> SimCard::simState() const
{
    std::string buf;
    return simStateWithMessage(buf);
}

std::optional<at::SimState> SimCard::simStateWithMessage(std::string &message) const
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

std::string SimCard::pinToString(std::vector<unsigned int> v)
{
    std::string buf;
    std::transform(v.begin(), v.end(), std::back_inserter(buf), [](auto &&c) { return '0' + c; });
    return buf;
}
