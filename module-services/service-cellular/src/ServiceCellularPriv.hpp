// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <service-cellular/ServiceCellular.hpp>
#include <service-cellular/State.hpp>

#include "SimCard.hpp"

namespace cellular::internal
{
    using service::SimCard;
    using service::State;

    class ServiceCellularPriv
    {
        ServiceCellular *owner;

        std::unique_ptr<SimCard> simCard;
        std::unique_ptr<State> state;

        State::PowerState nextPowerState = State::PowerState::Off;

      public:
        ServiceCellularPriv(ServiceCellular *owner);

        void connectSimCard();

      private:
        void initSimCard();

        friend class ::ServiceCellular;
    };
} // namespace cellular::internal
