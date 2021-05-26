// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SimCard.hpp"

#include <common_data/EventStore.hpp>
#include <bsp/cellular/bsp_cellular.hpp>
#include <modem/BaseChannel.hpp>
#include <at/ATFactory.hpp>
#include <at/UrcFactory.hpp>
#include <at/UrcCpin.hpp>

namespace cellular
{
    namespace internal
    {
        constexpr const char *pinString[] = {"SC", "P2"};

        service::sim::Result convertErrorFromATResult(const at::Result atres)
        {
            if (std::holds_alternative<at::EquipmentErrorCode>(atres.errorCode)) {

                auto err = static_cast<int>(std::get<at::EquipmentErrorCode>(atres.errorCode));
                if ((err > static_cast<int>(service::sim::Result::AT_ERROR_Begin)) &&
                    (err < static_cast<int>(service::sim::Result::AT_ERROR_End))) {
                    return static_cast<service::sim::Result>(err);
                }
            }
            return service::sim::Result::Unknown;
        }

        std::string simCodeToString(const cellular::api::SimCode &v)
        {
            std::string buf;
            std::transform(v.begin(), v.end(), std::back_inserter(buf), [](auto &&c) { return '0' + c; });
            return buf;
        }

    } // namespace internal

    namespace service
    {
        bool SimCard::ready() const
        {
            return channel;
        }

        void SimCard::setChannel(at::BaseChannel *channel)
        {
            this->channel = channel;
        }

        bool SimCard::handleSetActiveSim(api::SimSlot sim)
        {
            Store::GSM::get()->selected = static_cast<Store::GSM::SIM>(sim);
            bsp::cellular::sim::simSelect();
            bsp::cellular::sim::hotSwapTrigger();
            return true;
        }

        bool SimCard::handleIsPinLocked() const
        {
            return isPinLocked();
        }

        unsigned int SimCard::getAttemptsPin(sim::Pin pin) const
        {
            return attemptsPin[static_cast<int>(pin)];
        }

        unsigned int SimCard::getAttemptsPuk(sim::Pin pin) const
        {
            return attemptsPuk[static_cast<int>(pin)];
        }

        bool SimCard::handleChangePin(const api::SimCode &oldPin, const api::SimCode &pin)
        {
            const auto _oldPin = internal::simCodeToString(oldPin);
            const auto _pin    = internal::simCodeToString(pin);
            return processPinResult(changePin(_oldPin, _pin));
        }

        bool SimCard::handleUnblockWithPuk(const api::SimCode &puk, const api::SimCode &pin)
        {
            const auto _puk = internal::simCodeToString(puk);
            const auto _pin = internal::simCodeToString(pin);
            return processPinResult(supplyPuk(_puk, _pin));
        }

        bool SimCard::handleSetPinLock(const api::SimCode &pin, api::SimLockState lock)
        {
            const auto _pin = internal::simCodeToString(pin);
            return processPinResult(setPinLock(_pin, lock == cellular::api::SimLockState::Enabled));
        }

        bool SimCard::handlePinUnlock(const api::SimCode &pin)
        {
            const auto _pin = internal::simCodeToString(pin);
            return processPinResult(supplyPin(_pin));
        }

        void SimCard::handleSimStateChanged(at::SimState state)
        {
            handleSimState(state);
        }

        void SimCard::handleSimState(at::SimState state)
        {
            switch (state) {
            case at::SimState::Ready:
                Store::GSM::get()->sim = Store::GSM::get()->selected;
                if (onSimReady)
                    onSimReady(true);
                break;
            case at::SimState::NotReady:
                Store::GSM::get()->sim = Store::GSM::SIM::SIM_FAIL;
                if (onSimReady)
                    onSimReady(false);
                break;
            case at::SimState::SimPin:
                [[fallthrough]];
            case at::SimState::SimPin2: {
                auto pin = (state == at::SimState::SimPin) ? sim::Pin::PIN1 : sim::Pin::PIN2;
                if (readAttemptsCounters(pin)) {
                    if (attemptsPuk[static_cast<int>(pin)] != 0) {
                        if (onNeedPin)
                            onNeedPin(attemptsPin[static_cast<int>(pin)]);
                        break;
                    }
                }
                if (onSimBlocked)
                    onSimBlocked();
                break;
            }
            case at::SimState::SimPuk:
                [[fallthrough]];
            case at::SimState::SimPuk2: {
                auto pin = (state == at::SimState::SimPuk) ? sim::Pin::PIN1 : sim::Pin::PIN2;
                if (readAttemptsCounters(pin)) {
                    if (attemptsPuk[static_cast<int>(pin)] != 0) {
                        if (onNeedPuk)
                            onNeedPuk(attemptsPuk[static_cast<int>(pin)]);
                        break;
                    }
                }
                if (onSimBlocked)
                    onSimBlocked();
                break;
            }
            case at::SimState::Locked:
                Store::GSM::get()->sim = Store::GSM::SIM::SIM_FAIL;
                if (onSimBlocked)
                    onSimBlocked();
                break;
            case at::SimState::PhNetPin:
                [[fallthrough]];
            case at::SimState::PhNetPuk:
                [[fallthrough]];
            case at::SimState::PhNetSPin:
                [[fallthrough]];
            case at::SimState::PhNetSPuk:
                [[fallthrough]];
            case at::SimState::PhSpPin:
                [[fallthrough]];
            case at::SimState::PhSpPuk:
                [[fallthrough]];
            case at::SimState::PhCorpPin:
                [[fallthrough]];
            case at::SimState::PhCorpPuk:
                [[fallthrough]];
            case at::SimState::Unknown:
                LOG_ERROR("SimState not supported");
                Store::GSM::get()->sim = Store::GSM::SIM::SIM_UNKNOWN;
                break;
            }
            if (onSimEvent)
                onSimEvent();
        }

        bool SimCard::readAttemptsCounters(sim::Pin pin)
        {
            if (!ready()) {
                return false;
            }

            auto pinid = static_cast<int>(pin);
            auto resp  = channel->cmd(at::factory(at::AT::QPINC) + "\"" + internal::pinString[pinid] + "\"");
            at::response::qpinc::AttemptsCounters ret;
            if (at::response::parseQPINC(resp, ret)) {
                attemptsPin[pinid] = ret.PinCounter;
                attemptsPuk[pinid] = ret.PukCounter;
                return true;
            }

            return false;
        }

        bool SimCard::processPinResult(sim::Result result)
        {
            if (result == sim::Result::IncorrectPassword) {
                if (auto state = simState(); state) {
                    handleSimState(*state);
                }
            }
            else if (result != sim::Result::OK) {
                if (onUnhandledCME)
                    onUnhandledCME(static_cast<unsigned int>(result));
            }
            // refetch attempts counters after access
            readAttemptsCounters();
            return result == sim::Result::OK;
        }

        sim::Result SimCard::supplyPin(const std::string &pin)
        {
            return sendCommand(sim::LockType::PIN, at::factory(at::AT::CPIN) + "\"" + pin + "\"");
        }

        sim::Result SimCard::changePin(const std::string &oldPin, const std::string &newPin)
        {
            return sendCommand(sim::LockType::PIN,
                               at::factory(at::AT::CPWD) + "\"SC\", \"" + oldPin + "\",\"" + newPin + "\"");
        }

        sim::Result SimCard::supplyPuk(const std::string &puk, const std::string &pin)
        {
            return sendCommand(sim::LockType::PUK, at::factory(at::AT::CPIN) + "\"" + puk + "\"" + ",\"" + pin + "\"");
        }

        sim::Result SimCard::setPinLock(const std::string &pin, bool lock)
        {
            return sendCommand(sim::LockType::PIN,
                               at::factory(at::AT::CLCK) + "\"SC\"," + (lock ? "1" : "0") + ",\"" + pin + "\"");
        }

        bool SimCard::isPinLocked() const
        {
            auto resp = channel->cmd(at::factory(at::AT::CLCK) + "\"SC\",2\r");
            int val   = 0;
            if (at::response::parseCLCK(resp, val)) {
                return val != 0;
            }
            return true;
        }

        std::optional<at::SimState> SimCard::simState() const
        {
            auto resp = channel->cmd(at::factory(at::AT::GET_CPIN));
            if (resp.code == at::Result::Code::OK) {
                if (resp.response.size()) {
                    for (auto el : resp.response) {
                        auto urc = at::urc::UrcFactory::Create(el);
                        if (auto cpin = dynamic_cast<at::urc::Cpin *>(urc.get())) {
                            return cpin->getState();
                        }
                    }
                }
            }
            return at::SimState::Unknown;
        }

        sim::Result SimCard::sendCommand(sim::LockType check, const at::Cmd &cmd)
        {
            if (readAttemptsCounters()) {
                switch (check) {
                case sim::LockType::PIN:
                    if (attemptsPin[0] == 0)
                        return sim::Result::Locked;
                    break;
                case sim::LockType::PUK:
                    if (attemptsPuk[0] == 0)
                        return sim::Result::Locked;
                    break;
                }
            }
            else {
                return sim::Result::Unknown;
            }

            if (auto resp = channel->cmd(cmd); resp.code != at::Result::Code::OK) {
                return internal::convertErrorFromATResult(resp);
            }

            return sim::Result::OK;
        }
    } // namespace service
} // namespace cellular
