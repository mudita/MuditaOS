// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Sink.hpp"
#include <cstdint>
#include <memory>

namespace sink
{
    struct rtt_traits
    {
        static constexpr auto name = "RTT";
        uint8_t chanel_no;
        size_t size;
    };

    class RTT : public Sink
    {
        std::unique_ptr<char[]> buffer;
        rtt_traits trait;

      public:
        explicit RTT(rtt_traits t);
        void put(const char *data, size_t size) final;
    };

} // namespace sink
