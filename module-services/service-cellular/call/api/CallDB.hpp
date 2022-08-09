// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "CalllogRecord.hpp"
#include <PhoneNumber.hpp>

namespace sys
{
    class Service;
}
namespace CellularCall
{
    class Call;
}

namespace call::api
{
    class DB
    {
      public:
        virtual void startCall(CalllogRecord &rec)                                = 0;
        virtual void endCall(const CalllogRecord &rec)                            = 0;
        virtual bool isNumberInFavourites(const utils::PhoneNumber::View &number) = 0;
        virtual ~DB()                                                             = default;
    };

} // namespace call::api

class CallDB : public call::api::DB
{
    sys::Service *owner;

  public:
    explicit CallDB(sys::Service *);
    void startCall(CalllogRecord &rec) override;
    void endCall(const CalllogRecord &rec) override;
    bool isNumberInFavourites(const utils::PhoneNumber::View &number) override;
};
