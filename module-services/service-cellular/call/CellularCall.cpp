// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "call/CellularCall.hpp"
#include "service-cellular/ServiceCellular.hpp"

#include <log/log.hpp>

#include <memory>
#include <utility>
#include "CallMachine.hpp"

namespace call
{
    Call::~Call() = default;

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
                                                              std::make_unique<cellular::Api>(owner),
                                                              std::move(sentinel)});
    }

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
        // triggering the sentinel here is necessary to speed up the state machine processing,
        // as the DND mode is based on quick hangup when the call arrives - thus we can hear some
        // calling signal if the hangup wasn't fast enough
        machine->di.sentinel->HoldMinimumFrequency(bsp::CpuFrequencyMHz::Level_6);
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

} // namespace call
