// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <service-cellular/ServiceCellular.hpp>
#include <service-cellular/State.hpp>

#include "SimCard.hpp"

namespace cellular
{
    namespace service
    {
        class Settings;
    }

    namespace internal
    {
        using service::Settings;
        using service::SimCard;
        using service::State;

        class ServiceCellularPriv
        {
            ServiceCellular *owner = nullptr;

            std::unique_ptr<SimCard> simCard;
            std::unique_ptr<State> state;

            State::PowerState nextPowerState = State::PowerState::Off;

            std::unique_ptr<Settings> settings{};

          public:
            ServiceCellularPriv(ServiceCellular *owner);
            ~ServiceCellularPriv();

            void startup();

          private:
            void initStaticData();

            void initSimCard();

            void connectSimCard();

            friend class ::ServiceCellular;
        };

    } // namespace internal
} // namespace cellular
