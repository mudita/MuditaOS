// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <PhoneNumber.hpp>

namespace sys
{
    class Service;
}

namespace call::api
{
    class GUI
    {
      public:
        virtual void notifyRING()                                       = 0;
        virtual void notifyCLIP(const utils::PhoneNumber::View &number) = 0;
        virtual ~GUI()                                                  = default;
    };
}; // namespace call::api

class CallGUI : public call::api::GUI
{
    struct CallMeta;
    sys::Service *owner;

  public:
    explicit CallGUI(sys::Service *);

    void notifyRING() override;
    void notifyCLIP(const utils::PhoneNumber::View &number) override;
};
