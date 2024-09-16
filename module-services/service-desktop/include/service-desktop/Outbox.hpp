// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

struct Outbox
{
    enum class EntryType
    {
        INVALID,
        MESSAGE,
        THREAD,
        CONTACT
    };

    enum class EntryChange
    {
        INVALID,
        CREATED,
        UPDATED,
        DELETED
    };

    struct NotificationEntry
    {
        uint32_t uid            = 0;
        EntryType entryType     = EntryType::INVALID;
        EntryChange entryChange = EntryChange::INVALID;
        uint32_t recordId       = 0;
    };
};
