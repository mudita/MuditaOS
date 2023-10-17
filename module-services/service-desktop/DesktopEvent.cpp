// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "service-desktop/DesktopEvent.hpp"
#include <endpoints/message/Sender.hpp>

namespace sdesktop
{
    void Event::send()
    {
        endpoints::sender::putToSendQueue(context.createSimpleResponse());
    }
} // namespace sdesktop
