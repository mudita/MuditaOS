// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <endpoints/outbox/OutboxHelper.hpp>
#include <endpoints/message/Sender.hpp>
#include <endpoints/Context.hpp>
#include <endpoints/JsonKeyNames.hpp>
#include "service-desktop/ServiceDesktop.hpp"

namespace sdesktop::endpoints
{
    auto OutboxHelper::toJson(const Outbox::NotificationEntry &entry) -> json11::Json
    {
        auto notificationEntry = json11::Json::object{{json::outbox::uid, entry.uid},
                                                      {json::outbox::type, static_cast<int>(entry.entryType)},
                                                      {json::outbox::change, static_cast<int>(entry.entryChange)},
                                                      {json::outbox::record_id, entry.recordId}};
        return notificationEntry;
    }

    void OutboxHelper::processGetRequest(Context &context)
    {
        context.setResponseStatus(http::Code::BadRequest);

        if (context.getBody()[json::messages::category].string_value() == json::outbox::entries) {
            getNotifications(context);
        }

        sender::putToSendQueue(context.createSimpleResponse());
    }

    void OutboxHelper::processDeleteRequest(Context &context)
    {
        context.setResponseStatus(http::Code::BadRequest);

        if (context.getBody()[json::outbox::entries].is_array()) {
            removeNotifications(context);
        }

        sender::putToSendQueue(context.createSimpleResponse());
    }

    void OutboxHelper::getNotifications(Context &context)
    {
        auto ownerService = dynamic_cast<ServiceDesktop *>(ownerServicePtr);
        if (ownerService == nullptr) {
            LOG_ERROR("Downcasting error!");
            return;
        }
        const auto notificationEntries = ownerService->getNotificationEntries();

        json11::Json::array notificationsArray;
        notificationsArray.reserve(notificationEntries.size());
        for (const auto &entry : notificationEntries) {
            notificationsArray.emplace_back(toJson(entry));
        }
        context.setResponseBody(json11::Json::object{{json::entries, std::move(notificationsArray)}});
        context.setResponseStatus(http::Code::OK);
    }

    void OutboxHelper::removeNotifications(Context &context)
    {
        auto ownerService = dynamic_cast<ServiceDesktop *>(ownerServicePtr);
        if (ownerService == nullptr) {
            LOG_ERROR("Downcasting error!");
            return;
        }

        auto entriesToBeRemoved = context.getBody()[json::entries].array_items();
        std::vector<int> uidsOfEntriesToBeRemoved;
        uidsOfEntriesToBeRemoved.reserve(entriesToBeRemoved.size());
        for (const auto &entryUid : entriesToBeRemoved) {
            uidsOfEntriesToBeRemoved.emplace_back(entryUid.int_value());
        }

        ownerService->removeNotificationEntries(uidsOfEntriesToBeRemoved);
        context.setResponseStatus(http::Code::OK);
    }

} // namespace sdesktop::endpoints
