// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <service-appmgr/service-appmgr/Actions.hpp>

namespace mmiactions = app::manager::actions;

namespace app::manager::actions
{

    class MMINoneSpecifiedResult;
    class MMICallForwardingResult;
    class MMICustomResultParams;

    class Visitor
    {
      public:
        virtual void visit(MMINoneSpecifiedResult &, std::string &)  = 0;
        virtual void visit(MMICallForwardingResult &, std::string &) = 0;
        virtual void visit(MMICustomResultParams &, std::string &)   = 0;
    };

    class IMMICustomResultParams
    {
      public:
        enum class MMIType
        {
            NoneSpecified,
            CallForwardingNotification,
            CallForwardingData,
            Clir
        };

        enum class MMIResultMessage
        {
            NoneSpecifiedSuccess,
            NoneSpecifiedFailed,
            CommonFailure,
            CommonMMINotSupported,
            CommonNoMessage,

            ClirAccordingToSubscription,
            ClirEnabled,
            ClirDisabled,
            ClirNotProvisioned,
            ClirPermanentProvisioned,
            ClirUnknown,
            ClirTemporaryRestricted,
            ClirTemporaryAllowed,

            // Call forwarding
            RegistrationSuccessful,
            RegistrationFailed,
            ErasureSuccessful,
            ErasureFailed,
            DisablingSuccessful,
            DisablingFailed,
            EnablingSuccessful,
            EnablingFailed,
            CallForwardingDisabled

        };

        virtual auto getMessage() const -> std::vector<MMIResultMessage> = 0;
        virtual void addMessage(const MMIResultMessage &message)         = 0;
        virtual auto getMessageType() const noexcept -> MMIType          = 0;
        virtual void accept(Visitor &v, std::string &displayMessage)     = 0;
    };

    class MMICustomResultParams : public IMMICustomResultParams
    {
      public:
        MMICustomResultParams(MMIType resultType) : type(resultType){};
        virtual void addMessage(const MMIResultMessage &message) override;
        virtual auto getMessage() const -> std::vector<MMIResultMessage> override;
        virtual auto getMessageType() const noexcept -> MMIType override;
        virtual void accept(Visitor &v, std::string &displayMessage) override;

      protected:
        MMIType type;
        std::vector<MMIResultMessage> messages;
    };

    class MMINoneSpecifiedResult : public MMICustomResultParams
    {
      public:
        MMINoneSpecifiedResult() : MMICustomResultParams(MMIType::NoneSpecified)
        {}
        virtual void accept(Visitor &v, std::string &displayMessage) override;
    };

    class MMICallForwardingResult : public MMICustomResultParams
    {
      public:
        explicit MMICallForwardingResult(MMIType resultType) : MMICustomResultParams(resultType)
        {}

        MMICallForwardingResult(
            MMIType resultType, std::string voice, std::string fax, std::string sync, std::string async)
            : MMICustomResultParams(resultType), voice(voice), fax(fax), sync(sync), async(async)
        {}

        auto getData() const -> std::tuple<std::string, std::string, std::string, std::string>;
        virtual void accept(Visitor &v, std::string &displayMessage) override;

      private:
        std::string voice;
        std::string fax;
        std::string sync;
        std::string async;
    };

    class MMIClirResult : public MMICustomResultParams
    {
      public:
        MMIClirResult() : MMICustomResultParams(MMIType::Clir)
        {}
    };

    class MMIParams : public ActionParams
    {
      public:
        explicit MMIParams(std::string mmiData);

        [[nodiscard]] std::string getData() const;

      private:
        std::string mmiData;
    };

    class MMIResultParams : public ActionParams
    {
      public:
        enum class MMIResult
        {
            Success,
            Failed
        };

        explicit MMIResultParams(MMIResult result, std::shared_ptr<MMICustomResultParams> customResult = nullptr);

        [[nodiscard]] MMIResult getData() const noexcept;
        [[nodiscard]] auto getCustomData() const noexcept -> std::shared_ptr<MMICustomResultParams>;

      private:
        MMIResult result;
        std::shared_ptr<MMICustomResultParams> customResult = nullptr;
    };
} // namespace app::manager::actions
