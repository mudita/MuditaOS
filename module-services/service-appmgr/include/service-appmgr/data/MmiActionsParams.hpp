// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <service-appmgr/Actions.hpp>

namespace mmiactions = app::manager::actions;

namespace app::manager::actions
{

    class MMINoneSpecifiedResult;
    class MMICallForwardingResult;
    class MMICustomResultParams;
    class MMICallBarringResult;
    class MMICallWaitingResult;
    class MMIClipResult;
    class MMIClirResult;
    class MMIImeiResult;

    class Visitor
    {
      public:
        virtual void visit(MMINoneSpecifiedResult &, std::string &)  = 0;
        virtual void visit(MMICallForwardingResult &, std::string &) = 0;
        virtual void visit(MMICustomResultParams &, std::string &)   = 0;
        virtual void visit(MMICallBarringResult &, std::string &)    = 0;
        virtual void visit(MMICallWaitingResult &, std::string &)    = 0;
        virtual void visit(MMIClipResult &, std::string &)           = 0;
        virtual void visit(MMIClirResult &, std::string &)           = 0;
        virtual void visit(MMIImeiResult &, std::string &)           = 0;
    };

    class IMMICustomResultParams
    {
      public:
        virtual ~IMMICustomResultParams() = default;
        enum class MMIType
        {
            NoneSpecified,
            CallForwardingNotification,
            CallForwardingData,
            Clir,
            Clip,
            Imei,
            CallBarringNotification,
            CallBarringData,
            CallWaitingNotification,
            CallWaitingData
        };

        enum class MMIResultMessage
        {
            NoneSpecifiedSuccess,
            NoneSpecifiedFailed,
            CommonFailure,
            CommonNoMessage,
            CommonMMINotSupported,
            CommonEnabled,
            CommonDisabled,
            CommonVoice,
            CommonData,
            CommonFax,
            CommonSync,
            CommonAsync,
            CommonAllDisabled,
            CommonDeactivated,
            CommonActivated,
            CommonQuery,

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
            CallForwardingDisabled,

            CallBarringActivated,
            CallBarringDeactivated,

            ClipActivted,
            ClipDeactivated,
            ClipNotProvisioned,
            ClipProvisioned,
            ClipUnknown,

            CallWaitingActivated,
            CallWaitingDeactivated,

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
        void accept(Visitor &v, std::string &displayMessage) override;
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
        void accept(Visitor &v, std::string &displayMessage) override;

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
        void accept(Visitor &v, std::string &displayMessage) override;
    };

    class MMICallBarringResult : public MMICustomResultParams
    {
      public:
        explicit MMICallBarringResult(MMIType type) : MMICustomResultParams(type)
        {}
        void addMessages(const std::pair<MMIResultMessage, MMIResultMessage> &message) noexcept;
        auto getMessages(void) noexcept -> std::vector<std::pair<MMIResultMessage, MMIResultMessage>>;
        void accept(Visitor &v, std::string &displayMessage) override;

      private:
        std::vector<std::pair<MMIResultMessage, MMIResultMessage>> data;
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

    class MMIClipResult : public MMICustomResultParams
    {
      public:
        MMIClipResult() : MMICustomResultParams(MMIType::Clip)
        {}
        void accept(Visitor &v, std::string &displayMessage) override;
    };

    class MMIImeiResult : public MMICustomResultParams
    {
      public:
        explicit MMIImeiResult(const std::string &imei) : MMICustomResultParams(MMIType::Imei), imei(imei)
        {}
        explicit MMIImeiResult() : MMICustomResultParams(MMIType::Imei)
        {}
        auto getImei() const noexcept -> std::string;
        void accept(Visitor &v, std::string &displayMessage) override;

      private:
        std::string imei;
    };

    class MMICallWaitingResult : public MMICustomResultParams
    {
      public:
        explicit MMICallWaitingResult(const MMIType type) : MMICustomResultParams(type)
        {}
        void addMessages(const std::pair<MMIResultMessage, MMIResultMessage> &message);
        auto getMessages() const noexcept -> std::vector<std::pair<MMIResultMessage, MMIResultMessage>>;
        void accept(Visitor &v, std::string &displayMessage) override;

      private:
        std::vector<std::pair<MMIResultMessage, MMIResultMessage>> messages;
    };
} // namespace app::manager::actions
