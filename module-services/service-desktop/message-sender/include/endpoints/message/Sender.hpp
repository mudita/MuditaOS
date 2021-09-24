// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <string>
#include <json11.hpp>

extern "C"
{
#include <FreeRTOS.h>
#include <queue.h>
}

namespace sdesktop::endpoints::sender
{

    void setSendQueueHandle(xQueueHandle handle);
    void putToSendQueue(const json11::Json &msg);

} // namespace sdesktop::endpoints::sender
