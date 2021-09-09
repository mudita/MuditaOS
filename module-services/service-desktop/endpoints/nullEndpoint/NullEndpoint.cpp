// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "NullEndpoint.hpp"

using namespace parserFSM;

auto NullEndpoint::handle(parserFSM::Context &context) -> void
{
    context.setResponseStatus(http::Code::BadRequest);
    MessageHandler::putToSendQueue(context.createSimpleResponse());
    LOG_ERROR("Unknown Endpoint #%d", static_cast<int>(context.getEndpoint()));
}
