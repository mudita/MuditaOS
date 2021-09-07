// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "service-desktop/DesktopEvent.hpp"
#include <service-desktop/parser/MessageHandler.hpp>

namespace sdesktop
{
    void Event::send()
    {
        parserFSM::MessageHandler::putToSendQueue(context.createSimpleResponse());
    }
} // namespace sdesktop
