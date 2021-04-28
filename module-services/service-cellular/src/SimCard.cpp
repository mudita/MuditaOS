// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SimCard.hpp"
#include <service-cellular/ServiceCellular.hpp>

#include <common_data/EventStore.hpp>
#include <bsp/cellular/bsp_cellular.hpp>
#include <modem/ATCommon.hpp>
#include <at/ATFactory.hpp>
#include <at/UrcFactory.hpp>
#include <at/UrcCpin.hpp>

namespace
{
    using namespace cellular::service;

    constexpr const char *pinString[] = {"SC", "P2"};

    sim::Result convertErrorFromATResult(const at::Result atres)
    {
        if (std::holds_alternative<at::EquipmentErrorCode>(atres.errorCode)) {

            auto err = static_cast<int>(std::get<at::EquipmentErrorCode>(atres.errorCode));
            if ((err > static_cast<int>(sim::Result::AT_ERROR_Begin)) &&
                (err < static_cast<int>(sim::Result::AT_ERROR_End))) {
                return static_cast<sim::Result>(err);
            }
        }
        return sim::Result::Unknown;
    }

} // namespace

namespace cellular::service
{

    void SimCard::registerMessages(ServiceCellular *owner)
    {
        using namespace ::cellular::msg;
        owner->connect(typeid(request::sim::GetLockState), [&](sys::Message *) -> sys::MessagePointer {
            return std::make_shared<request::sim::GetLockState::Response>(isPinLocked());
        });
    }

    bool SimCard::ready() const
    {
        return channel;
    }

    void SimCard::selectSim(api::Sim slot)
    {
        sim = slot;
    }

    void SimCard::resetChannel(at::Channel *cmd)
    {
        channel = cmd;
    }

    std::optional<at::response::qpinc::AttemptsCounters> SimCard::getAttemptsCounters(sim::Pin pin) const
    {
        if (!ready()) {
            return std::nullopt;
        }

        auto resp = channel->cmd(at::factory(at::AT::QPINC) + "\"" + pinString[int(pin)] + "\"");
        at::response::qpinc::AttemptsCounters ret;
        if (at::response::parseQPINC(resp, ret)) {
            return ret;
        }

        return std::nullopt;
    }

    sim::Result SimCard::supplyPin(const std::string pin) const
    {
        if (auto pc = getAttemptsCounters(); pc) {
            if (pc.value().PinCounter > 0) {
                auto resp = channel->cmd(at::factory(at::AT::CPIN) + "\"" + pin + "\"");

                if (resp.code == at::Result::Code::OK) {
                    return sim::Result::OK;
                }
                else {
                    return convertErrorFromATResult(resp);
                }
            }
            else {
                if (pc.value().PukCounter > 0) {
                    return sim::Result::SIM_PUKRequired;
                }
                else {
                    return sim::Result::Locked;
                }
            }
        }
        return sim::Result::Unknown;
    }

    sim::Result SimCard::supplyPuk(const std::string puk, const std::string pin) const
    {
        if (auto pc = getAttemptsCounters(); pc) {
            if (pc.value().PukCounter != 0) {
                auto resp = channel->cmd(at::factory(at::AT::CPIN) + "\"" + puk + "\"" + ",\"" + pin + "\"");
                if (resp.code == at::Result::Code::OK) {
                    return sim::Result::OK;
                }
                else {
                    return convertErrorFromATResult(resp);
                }
            }
            else {
                return sim::Result::Locked;
            }
        }

        return sim::Result::Unknown;
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

    sim::Result SimCard::setPinLock(bool lock, const std::string &pin) const
    {
        if (auto pc = getAttemptsCounters(); pc) {
            if (pc.value().PukCounter != 0) {
                auto resp =
                    channel->cmd(at::factory(at::AT::CLCK) + "\"SC\"," + (lock ? "1" : "0") + ",\"" + pin + "\"");
                if (resp.code == at::Result::Code::OK) {
                    return sim::Result::OK;
                }
                else {
                    return convertErrorFromATResult(resp);
                }
            }
            else {
                return sim::Result::Locked;
            }
        }

        return sim::Result::Unknown;
    }

    sim::Result SimCard::changePin(const std::string oldPin, const std::string newPin) const
    {
        if (auto pc = getAttemptsCounters(); pc) {
            if (pc.value().PukCounter != 0) {
                auto resp = channel->cmd(at::factory(at::AT::CPWD) + "\"SC\", \"" + oldPin + "\",\"" + newPin + "\"");
                if (resp.code == at::Result::Code::OK) {
                    return sim::Result::OK;
                }
                else {
                    return convertErrorFromATResult(resp);
                }
            }
            else {
                return sim::Result::Locked;
            }
        }

        return sim::Result::Unknown;
    }

    std::optional<at::SimState> SimCard::simState() const
    {
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
        return at::SimState::Unknown;
    }

} // namespace cellular::service
