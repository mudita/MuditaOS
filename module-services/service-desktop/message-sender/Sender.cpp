// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <endpoints/message/Sender.hpp>
#include <endpoints/message/Common.hpp>

namespace
{
    xQueueHandle sendQueue;
}

void sdesktop::endpoints::sender::setSendQueueHandle(QueueHandle_t handle)
{
    sendQueue = handle;
}

void sdesktop::endpoints::sender::putToSendQueue(const json11::Json &msg)
{

    if (uxQueueSpacesAvailable(sendQueue) != 0) {
        auto responseString = message::buildResponse(msg).release();
        xQueueSend(sendQueue, &responseString, portMAX_DELAY);
    }
}
