// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "service-desktop/DesktopEvent.hpp"
#include <endpoints/message/Sender.hpp>

namespace sdesktop
{
    void Event::send()
    {
        endpoints::sender::putToSendQueue(context.createSimpleResponse());
    }
} // namespace sdesktop
