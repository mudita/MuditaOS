// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <service-cellular/ServiceCellular.hpp>
#include <service-cellular/State.hpp>

#include "SimCard.hpp"
#include "NetworkTime.hpp"
#include "SimContacts.hpp"

namespace cellular::internal
{
    using service::NetworkTime;
    using service::SimCard;
    using service::SimContacts;
    using service::State;

    class ServiceCellularPriv
    {
        ServiceCellular *owner;

        std::unique_ptr<SimCard> simCard;
        std::unique_ptr<State> state;
        std::unique_ptr<NetworkTime> networkTime;
        std::unique_ptr<SimContacts> simContacts;
        State::PowerState nextPowerState = State::PowerState::Off;

      public:
        ServiceCellularPriv(ServiceCellular *owner);

        void connectSimCard();
        void connectNetworkTime();
        void connectSimContacts();

        void requestNetworkTimeSettings();

      private:
        void initSimCard();

        friend class ::ServiceCellular;
    };
} // namespace cellular::internal
