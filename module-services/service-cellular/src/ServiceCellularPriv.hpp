// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <service-cellular/ServiceCellular.hpp>
#include <service-cellular/State.hpp>

#include "SMSSendHandler.hpp"
#include "SimCard.hpp"
#include "NetworkTime.hpp"
#include "SimContacts.hpp"
#include "ImeiGetHandler.hpp"
#include "TetheringHandler.hpp"
#include "VolteHandler.hpp"
#include "VolteHandlerImpl.hpp"
#include "ModemResetHandler.hpp"
#include "CSQHandler.hpp"
#include "volte/VolteCapabilityHandler.hpp"
#include "ussd/USSDHandler.hpp"

namespace cellular::internal
{
    using cellular::ussd::USSDHandler;
    using service::CSQHandler;
    using service::ModemResetHandler;
    using service::NetworkTime;
    using service::SimCard;
    using service::SimContacts;
    using service::State;
    using service::TetheringHandler;
    using service::VolteCapabilityHandler;
    using service::VolteHandler;
    class ServiceCellularPriv
    {
        ServiceCellular *owner;

        sms::SMSSendHandler outSMSHandler;
        std::unique_ptr<SimCard> simCard;
        std::unique_ptr<State> state;
        std::unique_ptr<NetworkTime> networkTime;
        std::unique_ptr<SimContacts> simContacts;
        std::unique_ptr<service::ImeiGetHandler> imeiGetHandler;
        std::unique_ptr<TetheringHandler> tetheringHandler;
        std::unique_ptr<VolteHandler<DLCChannel, ModemResponseParserImpl>> volteHandler;
        std::unique_ptr<ModemResetHandler> modemResetHandler;
        std::unique_ptr<CSQHandler> csqHandler;
        std::unique_ptr<VolteCapabilityHandler> volteCapability;
        std::unique_ptr<USSDHandler> ussdHandler;

        State::PowerState nextPowerState = State::PowerState::Off;
        std::uint8_t multiPartSMSUID     = 0;

      public:
        explicit ServiceCellularPriv(ServiceCellular *owner);

        void connectSimCard();
        void connectNetworkTime();
        void connectSimContacts();
        void connectImeiGetHandler();
        void connectCSQHandler();

        void requestNetworkTimeSettings();
        void setInitialMultiPartSMSUID(std::uint8_t uid);
        std::function<void(std::uint8_t uid)> saveNewMultiPartSMSUIDCallback;

        void privInit(at::BaseChannel *channel);
        void privDeinit();

      private:
        void initSimCard();
        void initSMSSendHandler();
        void initTetheringHandler();
        void initModemResetHandler();
        void initCSQHandler();
        void initUSSDHandler();

        /** Send SMS action used by the SMSSendHandler
         * \param record SMS record to send
         */
        bool onSendSMS(SMSRecord &record);

        friend class ::ServiceCellular;
    };
} // namespace cellular::internal
