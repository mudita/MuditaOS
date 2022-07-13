// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "PhoneModes/PhoneMode.hpp"
#include "PhoneModes/Tethering.hpp"

class CellularMux;
class ServiceCellular;

namespace call::api
{
    class Api
    {
      public:
        virtual bool answerIncomingCall()            = 0;
        virtual bool hangupCall()                     = 0;
        virtual bool rejectCall()                     = 0;
        virtual bool areCallsFromFavouritesEnabled()  = 0;
        virtual sys::phone_modes::PhoneMode getMode() = 0;
        virtual sys::phone_modes::Tethering getTethering() = 0;
        virtual ~Api()                                = default;
    };
} // namespace call::api

namespace cellular
{
    class Api : public call::api::Api
    {
      private:
        ServiceCellular *cellular = nullptr;
        CellularMux *cmux         = nullptr;

      public:
        Api() = default;
        Api(ServiceCellular *cellular, CellularMux *);

        bool answerIncomingCall() override;
        bool hangupCall() override;
        bool rejectCall() override;
        bool areCallsFromFavouritesEnabled() override;
        sys::phone_modes::PhoneMode getMode() override;
        sys::phone_modes::Tethering getTethering() override;
    };
} // namespace cellular
