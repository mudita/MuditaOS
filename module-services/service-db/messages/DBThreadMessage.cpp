// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "DBThreadMessage.hpp"

#include "ThreadRecord.hpp" // for ThreadRecord

#include "messages/DBMessage.hpp" // for DBMessage, DBResponseMessage
#include <utility>                // for move

DBThreadMessage::DBThreadMessage(MessageType messageType, const ThreadRecord &rec) : DBMessage(messageType), record(rec)
{}

DBThreadMessageGet::DBThreadMessageGet(MessageType messageType, uint32_t contactID)
    : DBMessage(messageType), contactID(contactID)
{}

DBThreadGetCountMessage::DBThreadGetCountMessage(EntryState state)
    : DBMessage(MessageType::DBThreadGetCount), state(state)
{}

DBThreadResponseMessage::DBThreadResponseMessage(std::unique_ptr<std::vector<ThreadRecord>> rec,
                                                 uint32_t retCode,
                                                 uint32_t limit,
                                                 uint32_t offset,
                                                 uint32_t count,
                                                 MessageType respTo)
    : DBResponseMessage(retCode, count, respTo), records(std::move(rec)), limit(limit), offset(offset)
{
    this->count = count;
}
