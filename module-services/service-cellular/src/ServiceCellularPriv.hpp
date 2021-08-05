// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <service-cellular/ServiceCellular.hpp>
#include <service-cellular/State.hpp>

#include "SMSSendHandler.hpp"
#include "SimCard.hpp"
#include "NetworkTime.hpp"
#include "SimContacts.hpp"
#include "ImeiGetHandler.hpp"

namespace cellular::internal
{
    using service::NetworkTime;
    using service::SimCard;
    using service::SimContacts;
    using service::State;

    class ServiceCellularPriv
    {
        ServiceCellular *owner;

        sms::SMSSendHandler outSMSHandler;
        std::unique_ptr<SimCard> simCard;
        std::unique_ptr<State> state;
        std::unique_ptr<NetworkTime> networkTime;
        std::unique_ptr<SimContacts> simContacts;
        std::unique_ptr<service::ImeiGetHandler> imeiGetHandler;
        State::PowerState nextPowerState = State::PowerState::Off;

      public:
        explicit ServiceCellularPriv(ServiceCellular *owner);

        void connectSimCard();
        void connectNetworkTime();
        void connectSimContacts();
        void connectImeiGetHandler();

        void requestNetworkTimeSettings();

      private:
        void initSimCard();
        void initSMSSendHandler();

        /** Send SMS action used by the SMSSendHandler
         * \param record SMS record to send
         */
        bool onSendSMS(SMSRecord &record);

        friend class ::ServiceCellular;
    };
} // namespace cellular::internal
