// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "NotificationProvider.hpp"
#include <Service/Service.hpp>
#include <module-db/queries/notifications/QueryNotificationsGetAll.hpp>
#include <service-appmgr/Controller.hpp>
#include <service-appmgr/data/NotificationsChangedActionsParams.hpp>
#include <service-db/DBNotificationMessage.hpp>
#include <NotificationsRecord.hpp>

using namespace notifications;

NotificationProvider::NotificationProvider(sys::Service *ownerService, NotificationsConfiguration &notifcationConfig)
    : ownerService{ownerService}, notifcationConfig{notifcationConfig}
{}

template <NotificationType type, typename T>
bool NotificationProvider::handleNotSeenWithCounter(NotificationsRecord &&record)
{
    auto value = record.value;
    if (notifications.count(type) > 0) {
        if (value == 0) {
            notifications.erase(type);
            return true;
        }
        if (auto notification = static_cast<T *>(notifications[type].get()); value > notification->getValue()) {
            notifications[type] = std::make_shared<T>(value, std::move(record.contactRecord));
            return true;
        }
    }
    else if (value > 0) {
        notifications[type] = std::make_shared<T>(value, std::move(record.contactRecord));
        return true;
    }
    return false;
}

void NotificationProvider::handle(locks::PhoneLockTimeUpdate *msg)
{
    assert(msg);
    auto updateOnReceive = NotificationOnReceiveUpdate::FullRebuild;

    if (const auto &formattedTime = msg->getTime(); !formattedTime.empty()) {
        updateOnReceive = msg->getIsFirstData() ? NotificationOnReceiveUpdate::FullRebuild
                                                : NotificationOnReceiveUpdate::PartialRebuild;

        notifications[NotificationType::PhoneLock] = std::make_shared<PhoneLockNotification>(formattedTime);
    }
    else {
        notifications.erase(NotificationType::PhoneLock);
    }

    send(updateOnReceive);
}

void NotificationProvider::handle(db::query::notifications::GetAllResult *msg)
{
    assert(msg);
    auto records = *msg->getResult();

    bool notificationsChanged = false;
    for (auto &&record : records) {
        switch (record.key) {
        case NotificationsRecord::Key::Calls:
            notificationsChanged |=
                handleNotSeenWithCounter<NotificationType::NotSeenCall, NotSeenCallNotification>(std::move(record));
            break;
        case NotificationsRecord::Key::Sms:
            notificationsChanged |=
                handleNotSeenWithCounter<NotificationType::NotSeenSms, NotSeenSMSNotification>(std::move(record));
            break;
        default:
            break;
        }
    }

    notifcationConfig.updateCurrentList(listPolicy);

    if (notificationsChanged && listPolicy.updateListAllowed()) {
        send();
    }
}

void NotificationProvider::handle(db::NotificationMessage *msg)
{
    if (msg->interface == db::Interface::Name::Notifications && msg->type == db::Query::Type::Update) {
        requestNotSeenNotifications();
    }
}

void NotificationProvider::handle(sys::phone_modes::Tethering tethering)
{
    using Tethering           = sys::phone_modes::Tethering;
    bool notificationsChanged = false;

    if (tethering == Tethering::On && notifications.count(NotificationType::Tethering) == 0) {
        notifications[NotificationType::Tethering] = std::make_shared<notifications::TetheringNotification>();
        notificationsChanged                       = true;
    }
    else if (tethering == Tethering::Off && notifications.count(NotificationType::Tethering) != 0) {
        notifications.erase(NotificationType::Tethering);
        notificationsChanged = true;
    }

    if (notificationsChanged) {
        send();
    }
}

void NotificationProvider::handleSnooze(unsigned snoozeCount)
{
    if (snoozeCount > 0) {
        notifications[NotificationType::AlarmSnooze] =
            std::make_shared<notifications::AlarmSnoozeNotification>(snoozeCount);
    }
    else if (snoozeCount == 0) {
        notifications.erase(NotificationType::AlarmSnooze);
    }
    send();
}

void NotificationProvider::requestNotSeenNotifications()
{
    DBServiceAPI::GetQuery(
        ownerService, db::Interface::Name::Notifications, std::make_unique<db::query::notifications::GetAll>());
}

namespace
{
    using mapType    = NotificationProvider::Notifications;
    using returnType = std::shared_ptr<Notification>;
    struct get_second : public std::unary_function<mapType::value_type, returnType>
    {
        returnType operator()(const mapType::value_type &value) const
        {
            return value.second;
        }
    };
} // namespace

void NotificationProvider::send(NotificationOnReceiveUpdate updateOnReceive)
{
    notifcationConfig.updateCurrentList(listPolicy);

    std::list<std::shared_ptr<const Notification>> toSendNotifications;
    transform(notifications.begin(), notifications.end(), back_inserter(toSendNotifications), get_second());

    toSendNotifications.sort(
        [](const std::shared_ptr<const Notification> &lhs, const std::shared_ptr<const Notification> &rhs) {
            return (lhs->getPriority() > rhs->getPriority());
        });

    auto fastRefreshOnUpdate = updateOnReceive == NotificationOnReceiveUpdate::PartialRebuild;
    app::manager::Controller::sendAction(
        ownerService,
        app::manager::actions::NotificationsChanged,
        std::make_unique<app::manager::actions::NotificationsChangedParams>(
            std::move(toSendNotifications), listPolicy.showListWhenLocked(), fastRefreshOnUpdate));
}
