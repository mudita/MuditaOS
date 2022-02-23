// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <service-db/DBCalllogMessage.hpp>

#include <CalllogRecord.hpp>

#include <utility> // for move
DBCalllogMessage::DBCalllogMessage(MessageType messageType, const CalllogRecord &rec)
    : DBMessage(messageType), record(rec)
{}

DBCalllogResponseMessage::DBCalllogResponseMessage(std::unique_ptr<std::vector<CalllogRecord>> rec,
                                                   uint32_t retCode,
                                                   uint32_t limit,
                                                   uint32_t offset,
                                                   uint32_t count,
                                                   MessageType respTo)
    : DBResponseMessage(retCode, count, respTo), records(std::move(rec)), limit(limit), offset(offset)
{
    this->count = count;
}
