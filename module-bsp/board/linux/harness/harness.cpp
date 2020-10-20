// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "harness.hpp"

int hwInit(xQueueHandle qHandle)
{
    return 0;
}
std::string hwRead()
{
    return "";
}
bool hwFlush()
{
    return false;
}
bool hwEmit(const std::string &)
{
    return false;
}

using namespace bsp::harness::gpio;

bool hwGpioRead(enum Pin el)
{
    return false;
}
void hwGpioWrite(enum Pin el, uint32_t state)
{}
