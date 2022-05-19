// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "call/CellularCall.hpp"
#include "service-cellular/ServiceCellular.hpp"
#include "service-db/agents/settings/SystemSettings.hpp"

#include <CalllogRecord.hpp>
#include <PhoneNumber.hpp>
#include <Utils.hpp>
#include <log/log.hpp>

#include <cinttypes>
#include <ctime>
#include <memory>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>
#include "CallMachine.hpp"

namespace call
{

    Call::Call()
    {}

    Call::Call(ServiceCellular *owner,
               CellularMux *cmux,
               sys::TimerHandle timer,
               sys::TimerHandle timerRing,
               std::shared_ptr<sys::CpuSentinel> sentinel)
    {
        machine = std::make_unique<StateMachine>(Dependencies{std::make_unique<Audio>(owner),
                                                              std::make_unique<CallMulticast>(owner),
                                                              std::make_unique<CallGUI>(owner),
                                                              std::make_unique<CallDB>(owner),
                                                              std::make_unique<CallTimer>(std::move(timer)),
                                                              std::make_unique<TimerRing>(std::move(timerRing)),
                                                              std::make_unique<cellular::Api>(owner, cmux),
                                                              std::move(sentinel)});
    }

    Call::~Call()
    {}

    Call &Call::operator=(Call &&other) noexcept
    {
        if (not other.machine) {
            LOG_ERROR("operator= on wrong call");
            return *this;
        }
        machine = std::make_unique<StateMachine>(std::move(other.machine->di));
        return *this;
    }

    bool Call::handle(const call::event::RING &ring)
    {
        if (machine == nullptr) {
            return false;
        };
        return machine->machine.process_event(ring);
    }

    bool Call::handle(const call::event::CLIP &clip)
    {
        if (machine == nullptr) {
            return false;
        };
        return machine->machine.process_event(clip);
    }

    bool Call::handle(const call::event::StartCall &call)
    {
        if (machine == nullptr) {
            return false;
        };
        return machine->machine.process_event(call);
    }

    bool Call::handle(const call::event::Ended &ended)
    {
        if (machine == nullptr) {
            return false;
        };
        return machine->machine.process_event(ended);
    }

    bool Call::handle(const call::event::Reject &reject)
    {
        if (machine == nullptr) {
            return false;
        };
        return machine->machine.process_event(reject);
    }

    bool Call::handle(const call::event::AudioRequest &request)
    {
        if (machine == nullptr) {
            return false;
        };
        return machine->machine.process_event(request);
    }

    bool Call::handle(const call::event::ModeChange &change)
    {
        if (machine == nullptr) {
            return false;
        };
        machine->call.mode = change.mode;
        return true;
    }

    bool Call::handle(const call::event::TetheringChange &change)
    {
        if (machine == nullptr) {
            return false;
        };
        machine->call.tethering = change.tethering;
        return true;
    }

    bool Call::handle(const call::event::Answer &answer)
    {
        if (machine == nullptr) {
            return false;
        };
        return machine->machine.process_event(answer);
    }

    bool Call::handle(const call::event::OngoingTimer &tim)
    {
        if (machine == nullptr) {
            return false;
        };
        return machine->machine.process_event(tim);
    }

    bool Call::handle(const call::event::RingTimeout &tim)
    {
        if (machine == nullptr) {
            return false;
        };
        return machine->machine.process_event(tim);
    }

    bool Call::active() const
    {
        return machine->active();
    }

} // namespace CellularCall
