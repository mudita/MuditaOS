// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Service/Message.hpp>
#include <bsp/common.hpp>

namespace sys
{

    struct IsCpuPernament : public sys::DataMessage
    {
      public:
        explicit IsCpuPernament()
        {}
    };

    struct IsCpuPernamentResponse : public sys::ResponseMessage
    {
      public:
        explicit IsCpuPernamentResponse(bool pernament) : pernament(pernament)
        {}
        const bool pernament = false;
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
