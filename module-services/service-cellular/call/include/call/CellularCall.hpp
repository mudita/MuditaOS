// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "api/CallAudio.hpp"
#include "api/CallGUI.hpp"
#include "api/CallDB.hpp"
#include "api/CallTimer.hpp"
#include "api/CallMulticast.hpp"
#include "api/ModemCallApi.hpp"
#include <service-cellular/CellularMessage.hpp>
#include <Interface/CalllogRecord.hpp>
#include <SystemManager/CpuSentinel.hpp>
#include <PhoneNumber.hpp>
#include <Tables/CalllogTable.hpp>
#include <time/time_conversion.hpp>
#include <utf8/UTF8.hpp>
#include <Utils.hpp>

#include <cstdint>
#include <memory>
#include <functional>
#include <iosfwd>
#include <string>
#include <sys/types.h>
#include "CallEvents.hpp"

class ServiceCellular;

namespace call
{
    struct StateMachine;

    struct CallData
    {
        CalllogRecord record             = {};
        sys::phone_modes::PhoneMode mode = sys::phone_modes::PhoneMode::Connected;
        sys::phone_modes::Tethering tethering = sys::phone_modes::Tethering::Off;
    };

    struct Dependencies
    {
        std::shared_ptr<call::api::Audio> audio;
        std::shared_ptr<call::api::Multicast> multicast;
        std::shared_ptr<call::api::GUI> gui;
        std::shared_ptr<call::api::DB> db;
        std::shared_ptr<call::api::Timer> timer;
        std::shared_ptr<call::api::TimerRing> ringTimer;
        std::shared_ptr<call::api::Api> modem;
        std::shared_ptr<sys::CpuSentinel> sentinel;
    };

    class Call
    {
      private:
        std::unique_ptr<StateMachine> machine;

      public:
        Call();
        Call(ServiceCellular *owner,
             CellularMux *cmux,
             sys::TimerHandle timer,
             sys::TimerHandle timerRing,
             std::shared_ptr<sys::CpuSentinel> sentinel);
        ~Call();
        Call &operator=(Call &&other) noexcept;

        bool handle(const call::event::Answer &);
        bool handle(const call::event::RING &);
        bool handle(const call::event::StartCall &call);
        bool handle(const call::event::CLIP &);
        bool handle(const call::event::AudioRequest &);
        bool handle(const call::event::ModeChange &);
        bool handle(const call::event::TetheringChange &change);
        bool handle(const call::event::OngoingTimer &);
        bool handle(const call::event::RingTimeout &);
        bool handle(const call::event::Ended &);
        bool handle(const call::event::Reject &);

        /// return if call procesing is in any other state than `Idle`
        /// if so - we have ongoing call in handling
        bool active() const;
    };
} // namespace CellularCall
