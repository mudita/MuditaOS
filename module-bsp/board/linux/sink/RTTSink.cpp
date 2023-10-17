// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "sink/RTTSink.hpp"
#include <cassert>

namespace sink
{
    RTT::RTT(rtt_traits t) : trait(t)
    {
        assert(false && "no rtt on linux - not implemented");
    }

    void RTT::put(const char *data, size_t size)
    {}

} // namespace sink
