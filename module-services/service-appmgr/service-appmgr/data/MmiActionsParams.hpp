// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <service-appmgr/service-appmgr/Actions.hpp>

namespace app::manager::actions
{
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
            CommonNoMessage,

            ClirAccordingToSubscription,
            ClirEnabled,
            ClirDisabled,
            ClirNotProvisioned,
            ClirPermanentProvisioned,
            ClirUnknown,
            ClirTemporaryRestricted,
            ClirTemporaryAllowed,

            RegistrationSuccessful,
            RegistrationFailed,
            ErasureSuccessful,
            ErasureFailed,
            DisablingSuccessful,
            DisablingFailed,
            EnablingSuccessful,
            EnablingFailed
        };
        virtual auto getMessage() -> std::vector<MMIResultMessage> = 0;
        virtual void addMessage(const MMIResultMessage &message)   = 0;
        virtual auto getType() -> MMIType                          = 0;
    };

    class MMICustomResultParams : public IMMICustomResultParams
    {
      public:
        MMICustomResultParams(MMIType resultType) : type(resultType){};
        void addMessage(const MMIResultMessage &message) override;
        auto getMessage() -> std::vector<MMIResultMessage> override;
        auto getType() -> MMIType override;

      protected:
        MMIType type;
        std::vector<MMIResultMessage> messages;
    };

    class MMINoneSpecifiedResult : public MMICustomResultParams
    {
      public:
        MMINoneSpecifiedResult() : MMICustomResultParams(MMIType::NoneSpecified)
        {}
    };

    class MMICallForwardingResult : public MMICustomResultParams
    {
      public:
        MMICallForwardingResult() : MMICustomResultParams(MMIType::CallForwardingData)
        {}

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
        explicit MMIResultParams(MMIResult result);
        explicit MMIResultParams(std::shared_ptr<IMMICustomResultParams> cParams);
        [[nodiscard]] MMIResult getData() const noexcept;
        [[nodiscard]] auto getCustomMessage() const noexcept -> std::shared_ptr<IMMICustomResultParams>;

      private:
        MMIResult result;
        std::shared_ptr<IMMICustomResultParams> customResult = nullptr;
    };
} // namespace app::manager::actions
