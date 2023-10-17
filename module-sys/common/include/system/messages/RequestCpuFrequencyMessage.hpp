// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Service/Message.hpp>
#include <bsp/common.hpp>

namespace sys
{
    struct IsCpuPermanent : public sys::DataMessage
    {
      public:
        explicit IsCpuPermanent()
        {}
    };

    struct IsCpuPermanentResponse : public sys::ResponseMessage
    {
      public:
        explicit IsCpuPermanentResponse(bool permanent) : permanent(permanent)
        {}
        const bool permanent = false;
    };

    struct HoldCpuFrequencyPermanentlyMessage : public sys::DataMessage
    {
      public:
        explicit HoldCpuFrequencyPermanentlyMessage(bsp::CpuFrequencyMHz request) : request(request)
        {}
        const bsp::CpuFrequencyMHz request;
    };

    class HoldCpuFrequencyPermanentlyResponse : public sys::ResponseMessage
    {};

    class ReleaseCpuPermanentFrequencyMessage : public sys::DataMessage
    {};
} // namespace sys
