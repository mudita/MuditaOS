// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <service-appmgr/Actions.hpp>

#include <PhoneNumber.hpp>

namespace app::manager::actions
{
    class CallParams : public ActionParams
    {
      public:
        explicit CallParams(const utils::PhoneNumber::View &number = utils::PhoneNumber().getView());

        [[nodiscard]] const utils::PhoneNumber::View &getNumber() const;

      private:
        utils::PhoneNumber::View number;
    };

    class CallRequestGeneralErrorParams : public app::manager::actions::ActionParams
    {
      public:
        struct Error
        {
            enum class Type
            {
                Error,
                CmeError,
                CmsError,
                ModemTimeout,
                UndefinedError,
                TransmissionError,
                ChannelNotReadyError
            } type;
        };

        explicit CallRequestGeneralErrorParams(Error error) : error{error}
        {}

        [[nodiscard]] auto getError() const noexcept
        {
            return error;
        }

      private:
        Error error;
    };
} // namespace app::manager::actions
