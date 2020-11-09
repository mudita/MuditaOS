// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "service-cellular/SimCard.hpp"
#include <log/log.hpp>
#include <variant>
#include "Result.hpp"
#include "UrcCpin.hpp" //for Cpin parseState

/// Helper function to convert from one enum to another (only with part of them). Base on continuous range of sim errors
/// in AT.
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

//! Get information about attempts of PIN and PUK for standard sim card (eg. not PIN2)
/*!
 *
 * @return  As optional SimCard::AttemptsCounters, in case of error nullopt. Should be noted that in some case could
 * return SIMFailure which could mean 0 attempts (happen if lock during session, on modem/sim reboot again return 0,0);
 */
std::optional<SimCard::AttemptsCounters> SimCard::getAttemptsCounters()
{
    auto channel = cellularService.cmux->get(TS0710::Channel::Commands);
    if (channel) {
        auto resp                          = channel->cmd(at::factory(at::AT::QPINC) + "\"SC\"\r");
        const std::string_view AT_QPINC_SC = "+QPINC: \"SC\",";
        if (resp.code == at::Result::Code::OK) {
            if (resp.response.size()) {
                for (auto el : resp.response) {
                    if (el.compare(0, AT_QPINC_SC.length(), AT_QPINC_SC) == 0) {
                        std::vector<std::string> tokens = utils::split(el, ",");
                        if (tokens.size() == 3) {
                            AttemptsCounters ret;
                            utils::toNumeric(tokens[1], ret.PinCounter);
                            utils::toNumeric(tokens[2], ret.PukCounter);
                            return ret;
                        }
                    }
                }
            }
        }
    }

    return std::nullopt;
}

/// Supply pin for modem
/// \param pin digits as a string from 4-8 digits
/// \return return OK on success in other case see details in SimCardResult
SimCardResult SimCard::supplyPin(const std::string pin)
{
    if (auto pc = getAttemptsCounters(); pc) {
        if (pc.value().PinCounter > 0) {
            auto channel = cellularService.cmux->get(TS0710::Channel::Commands);
            if (channel) {
                auto resp = channel->cmd(at::factory(at::AT::CPIN) + "\"" + pin + "\"\r");

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
/// Supply pin for modem
/// \param puk puk as standard 8 digits
/// \param pin, new pin digits as a string from 4-8 digits
/// \return return OK on success in other case see details in SimCardResult
SimCardResult SimCard::supplyPuk(const std::string puk, const std::string pin)
{
    if (auto pc = getAttemptsCounters(); pc) {
        if (pc.value().PukCounter != 0) {
            auto channel = cellularService.cmux->get(TS0710::Channel::Commands);
            if (channel) {
                auto resp = channel->cmd(at::factory(at::AT::CPIN) + "\"" + puk + "\"" + ",\"" + pin + "\"\r");
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

/// return whethe the pin needs to be provided, only for standard pin.
/// \return true if need pin to unlock SIM card functionality
bool SimCard::isPinLocked()
{
    auto channel = cellularService.cmux->get(TS0710::Channel::Commands);
    if (channel) {
        auto resp = channel->cmd(at::factory(at::AT::CLCK) + "\"SC\",2\r");
        if (resp.code == at::Result::Code::OK) {
            //+CLCK:0
            const std::string_view AT_CLCK = "+CLCK:";
            if (resp.response.size()) {
                for (auto el : resp.response) {
                    if (el.compare(0, AT_CLCK.length(), AT_CLCK) == 0) {
                        auto lockinfo                   = el.substr(AT_CLCK.length());
                        std::vector<std::string> tokens = utils::split(lockinfo, ",");
                        if (tokens.size() >= 1) {
                            int val = 0;
                            utils::toNumeric(tokens[0], val);
                            return val;
                        }
                    }
                }
            }
        } // resp OK
    }

    return true;
}
/// Set whether to provide pin. Always need to provide actual pin for sim card, only for standard PIN
/// \param lock true for lock SIM card
/// \param pin actual pin for SIM card
/// \return
SimCardResult SimCard::setPinLock(bool lock, std::string pin)
{
    if (auto pc = getAttemptsCounters(); pc) {
        if (pc.value().PukCounter != 0) {
            auto channel = cellularService.cmux->get(TS0710::Channel::Commands);
            if (channel) {
                auto resp =
                    channel->cmd(at::factory(at::AT::CLCK) + "\"SC\"," + (lock ? "1" : "0") + ",\"" + pin + "\"\r");
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

/// Change pin, only for standard pin. To get effect of change pin, SIM cart or modem should be restarted
/// simplest solution is to call AT+CFUN=0/1
/// \param oldPin
/// \param newPin
/// \return return OK on success, else see SimCardResult
SimCardResult SimCard::changePin(std::string oldPin, std::string newPin)
{
    if (auto pc = getAttemptsCounters(); pc) {
        if (pc.value().PukCounter != 0) {
            auto channel = cellularService.cmux->get(TS0710::Channel::Commands);
            if (channel) {
                auto resp = channel->cmd(at::factory(at::AT::CPWD) + "\"SC\", \"" + oldPin + "\",\"" + newPin + "\"\r");
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

/// Return SIM state based on CPIN AT commands
/// \return at::SimState
at::SimState SimCard::simState()
{
    std::string buf;
    return simStateWithMessage(buf);
}
at::SimState SimCard::simStateWithMessage(std::string &message)
{

    auto channel = cellularService.cmux->get(TS0710::Channel::Commands);
    if (channel) {
        auto resp                      = channel->cmd(at::factory(at::AT::GET_CPIN) + "\r");
        const std::string_view AT_CPIN = "+CPIN:";
        if (resp.code == at::Result::Code::OK) {
            if (resp.response.size()) {
                for (auto el : resp.response) {
                    if (el.compare(0, AT_CPIN.length(), AT_CPIN) == 0) {
                        message = utils::trim(el.substr(AT_CPIN.length()));
                        return at::urc::Cpin::parseState(message);
                    }
                }
            }
        }
    }
    return at::SimState::Unknown;
}