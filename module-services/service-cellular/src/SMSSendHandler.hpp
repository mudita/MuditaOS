// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <SMSRecord.hpp>

#include <memory>
#include <functional>

namespace sys::phone_modes
{
    enum class PhoneMode;
}

namespace cellular::internal::sms
{
    class SMSSendHandler;
    class State;

    using OptionalState = std::optional<std::unique_ptr<State>>;

    class State
    {
      public:
        explicit State(const SMSSendHandler &ctx) : context(ctx)
        {}
        virtual ~State()                                              = default;
        virtual OptionalState handle(std::optional<SMSRecord> record) = 0;
        virtual std::string toStr() const                             = 0;

        bool isNotification(std::optional<const SMSRecord> record);
        bool isRecordEmpty(std::optional<const SMSRecord> record);

      protected:
        const SMSSendHandler &context;
    };

    class IdleState : public State
    {
      public:
        explicit IdleState(const SMSSendHandler &ctx) : State(ctx)
        {}
        OptionalState handle(std::optional<SMSRecord> record) override;
        std::string toStr() const override;
    };

    class ProcessQueryState : public State
    {
      public:
        explicit ProcessQueryState(const SMSSendHandler &ctx) : State(ctx)
        {}
        OptionalState handle(std::optional<SMSRecord> record) override;
        std::string toStr() const override;
    };

    class SMSSendHandler
    {
      public:
        SMSSendHandler();

        /**
         * External action events
         */
        void handleDBNotification();
        void handleIncomingDbRecord(SMSRecord &record, bool onDelay);
        void handleNoMoreDbRecords();
        void sendMessageIfDelayed();

        /**
         * User defined callbacks/events
         */

        /// Called when SMSSendHandler wants to send db query
        std::function<bool(db::Interface::Name database, std::unique_ptr<db::Query> query)> onSendQuery;
        /// Called upon sending SMS
        std::function<bool(SMSRecord &record)> onSend;
        /// Called when SMSSendHandler wants to check phone mode
        std::function<bool()> onGetOfflineMode;
        /// Called when SMSSendHandler wants to check SIM state
        std::function<bool()> onSIMNotInitialized;

      private:
        void handleStateChange(OptionalState state);

        std::unique_ptr<State> currentState;
        std::function<void()> actionOnDelay;
        bool delayedMessage;
    };

} // namespace cellular::internal::sms
