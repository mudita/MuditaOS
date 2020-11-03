// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

namespace at::urc
{
    class CLIP;
    class CREG;
    class CMTI;
    class CUSD;
    class CTZE;
    class QIND;
    class POWERED_DOWN;
    class URC_RESPONSE;

    class URCHandler
    {
      public:
        virtual void Handle(CLIP &urc){};
        virtual void Handle(CREG &urc){};
        virtual void Handle(CMTI &urc){};
        virtual void Handle(CUSD &urc){};
        virtual void Handle(CTZE &urc){};
        virtual void Handle(QIND &urc){};
        virtual void Handle(POWERED_DOWN &urc){};
        virtual void Handle(URC_RESPONSE &urc){};
    };
} // namespace at::urc
