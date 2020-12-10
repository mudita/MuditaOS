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
            CallForwarding
        };
        enum class MMIResultMessage
        {
            NoneSpecifiedSuccess,
            NoneSpecifiedFailed,
            RegistrationSuccessful,
            RegistrationFailed
        };
        virtual auto getMessage() -> std::vector<MMIResultMessage> = 0;
    };

    class MMICustomResultParams : public IMMICustomResultParams
    {
      public:
        MMICustomResultParams(MMIType resultType) : type(resultType);
        addMessage(const MMIResultMessage &message);
        auto getMessage() -> std::vector<MMIResultMessage> override;

      protected:
        MMIType type;
        std::vector<MMIResultMessage> message;
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
        MMICallForwardingResult() : MMICustomResultParams(MMIType::CallForwarding)
        {}

      private:
        std::string voice;
        std::string fax;
        std::string sync;
        std::string async;
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
        [[nodiscard]] MMIResult getData() const noexcept;

      private:
        MMIResult result;
        std::shared_ptr<IMMICustomResultParams> customResult;
    };
} // namespace app::manager::actions
