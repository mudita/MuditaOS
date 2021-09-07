// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "DBNotificationsHandler.hpp"

#include <module-db/queries/messages/threads/QueryThreadsGetCount.hpp>
#include <module-db/queries/calllog/QueryCalllogGetCount.hpp>
#include <service-appmgr/Controller.hpp>
#include <service-db/DBNotificationMessage.hpp>

using namespace app;

namespace
{
    bool requestUnreadThreadsCount(app::Application *app)
    {
        const auto [succeed, _] = DBServiceAPI::GetQuery(
            app, db::Interface::Name::SMSThread, std::make_unique<db::query::ThreadGetCount>(EntryState::UNREAD));
        return succeed;
    }

    bool requestUnreadCallsCount(app::Application *app)
    {
        const auto [succeed, _] = DBServiceAPI::GetQuery(
            app, db::Interface::Name::Calllog, std::make_unique<db::query::CalllogGetCount>(EntryState::UNREAD));
        return succeed;
    }
} // namespace

DBNotificationsHandler::DBNotificationsHandler(Application *ownerApp)
    : ownerApp(ownerApp), notifications{{Type::notReadSMS, 0}, {Type::notReadCall, 0}}
{}

void DBNotificationsHandler::setNotification(Type type, int value)
{
    notifications[type] = value;
}

bool DBNotificationsHandler::hasNotification(Type type) const
{
    try {
        return notifications.at(type) > 0;
    }
    catch (const std::out_of_range &e) {
        LOG_ERROR("Uninitialized notification type");
    }
    return false;
}

bool DBNotificationsHandler::handle(db::NotificationMessage *msg)
{
    if (msg->type == db::Query::Type::Read) {
        return false;
    }

    if (auto msgInterface = msg->interface; msgInterface == db::Interface::Name::Calllog) {
        return requestUnreadCallsCount(ownerApp);
    }
    else if (msgInterface == db::Interface::Name::SMSThread || msgInterface == db::Interface::Name::SMS) {
        return requestUnreadThreadsCount(ownerApp);
    }
    return false;
}

bool DBNotificationsHandler::handle(db::QueryResult *msg)
{
    if (auto response = dynamic_cast<db::query::ThreadGetCountResult *>(msg);
        response != nullptr && response->getState() == EntryState::UNREAD) {
        notifications[Type::notReadSMS] = response->getCount();
        return true;
    }
    if (auto response = dynamic_cast<db::query::CalllogGetCountResult *>(msg);
        response != nullptr && response->getState() == EntryState::UNREAD) {
        notifications[Type::notReadCall] = response->getCount();
        return true;
    }
    return false;
}

void DBNotificationsHandler::initHandler()
{
    requestUnreadThreadsCount(ownerApp);
    requestUnreadCallsCount(ownerApp);
}
