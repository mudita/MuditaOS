// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <SEGGER_RTT.h>
#include <sink/RTTSink.hpp>

namespace sink
{
    RTT::RTT(rtt_traits t) : trait(t)
    {
        buffer = std::unique_ptr<char[]>(new char[trait.size]);
        SEGGER_RTT_ConfigUpBuffer(
            trait.chanel_no, sink::rtt_traits::name, buffer.get(), trait.size, SEGGER_RTT_MODE_NO_BLOCK_SKIP);
    }

    void RTT::put(const char *data, size_t size)
    {
        SEGGER_RTT_Write(trait.chanel_no, data, size);
    }

} // namespace sink
