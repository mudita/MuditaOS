// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <service-desktop/OutboxNotifications.hpp>

auto OutboxNotifications::getNotificationEntries() const -> std::vector<Outbox::NotificationEntry>
{
    return notificationEntries;
}

void OutboxNotifications::removeNotificationEntries(const std::vector<uint32_t> &uidsOfNotificationsToBeRemoved)
{
    for (auto const &entryUid : uidsOfNotificationsToBeRemoved) {
        notificationEntries.erase(
            std::remove_if(notificationEntries.begin(),
                           notificationEntries.end(),
                           [&](const Outbox::NotificationEntry &entry) { return entry.uid == entryUid; }),
            notificationEntries.end());
    }
}

void OutboxNotifications::clearNotifications()
{
    notificationEntries.clear();
    notificationCurrentUid = 0;
}

void OutboxNotifications::newNotificationHandler(db::NotificationMessage *notificationMessage)
{
    auto entryType = Outbox::EntryType::INVALID;
    switch (notificationMessage->interface) {
    case db::Interface::Name::Contact:
        entryType = Outbox::EntryType::CONTACT;
        break;
    case db::Interface::Name::SMS:
        entryType = Outbox::EntryType::MESSAGE;
        break;
    case db::Interface::Name::SMSThread:
        entryType = Outbox::EntryType::THREAD;
        break;
    default:
        break;
    }

    auto entryChange = Outbox::EntryChange::INVALID;
    switch (notificationMessage->type) {
    case db::Query::Type::Create:
        entryChange = Outbox::EntryChange::CREATED;
        break;
    case db::Query::Type::Update:
        entryChange = Outbox::EntryChange::UPDATED;
        break;
    case db::Query::Type::Delete:
        entryChange = Outbox::EntryChange::DELETED;
        break;
    default:
        break;
    }

    if (entryType != Outbox::EntryType::INVALID && entryChange != Outbox::EntryChange::INVALID &&
        notificationMessage->recordId) {
        Outbox::NotificationEntry newNotificationEntry = {
            notificationCurrentUid++, entryType, entryChange, *notificationMessage->recordId};
        notificationEntries.emplace_back(newNotificationEntry);
    }
}
