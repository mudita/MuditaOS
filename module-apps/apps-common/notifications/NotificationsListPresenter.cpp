// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "NotificationsListPresenter.hpp"
#include <ListView.hpp>

using namespace gui;

namespace
{
    void setNotificationText(NotificationListItem *item,
                             const notifications::NotificationWithContact *notification,
                             const std::string &text)
    {
        if (notification->hasRecord() && !notification->getRecord().getFormattedName().empty()) {
            const auto &record = notification->getRecord();
            item->setName(record.getFormattedName());
        }
        else {
            item->setName(utils::translate(text), true);
        }
    }
} // namespace

NotificationsListPresenter::NotificationsListPresenter()
    : app::InternalModel<gui::NotificationListItem *>{}, gui::ListItemProvider{}
{}

unsigned int NotificationsListPresenter::requestRecordsCount()
{
    return internalData.size();
}

gui::ListItem *NotificationsListPresenter::getItem(gui::Order order)
{
    return getRecord(order);
}

void NotificationsListPresenter::requestRecords(uint32_t offset, uint32_t limit)
{
    setupModel(offset, limit);
    list->onProviderDataUpdate();
}

unsigned int NotificationsListPresenter::getMinimalItemSpaceRequired() const
{
    return style::notifications::itemHeight;
}

bool NotificationsListPresenter::hasDismissibleNotification() const noexcept
{
    for (const auto notification : internalData) {
        if (notification->isDismissible()) {
            return true;
        }
    }
    return false;
}

void NotificationsListPresenter::dismissAll()
{
    for (auto it = std::begin(internalData); it != std::end(internalData); it++) {
        if (auto item = *it; item->isDismissible()) {
            if (item->dismissCallback) {
                item->dismissCallback();
            }
        }
    }
}

bool NotificationsListPresenter::isEmpty() const noexcept
{
    return internalData.empty();
}

auto NotificationsListPresenter::create(const notifications::NotSeenSMSNotification *notification)
    -> NotificationListItem *
{
    auto item = new NotificationWithEventCounter(notifications::NotificationType::NotSeenSms, notification->getValue());
    setNotificationText(item, notification, "app_desktop_unread_messages");
    item->deleteByList = false;
    return item;
}
auto NotificationsListPresenter::create(const notifications::NotSeenCallNotification *notification)
    -> NotificationListItem *
{
    auto item =
        new NotificationWithEventCounter(notifications::NotificationType::NotSeenCall, notification->getValue());
    setNotificationText(item, notification, "app_desktop_missed_calls");
    item->deleteByList = false;
    return item;
}

auto NotificationsListPresenter::create(const notifications::TetheringNotification *notification)
    -> NotificationListItem *
{
    auto item =
        new NotificationWithOnOffButton(notifications::NotificationType::Tethering, gui::ButtonTriState::State::On);
    item->setName(utils::translate("Tethering"), false);
    item->deleteByList = false;
    return item;
}

auto NotificationsListPresenter::create(const notifications::AlarmSnoozeNotification *notification)
    -> NotificationListItem *
{
    auto item =
        new NotificationWithEventCounter(notifications::NotificationType::AlarmSnooze, notification->getValue());
    item->setName(utils::translate("app_desktop_alarm_snooze"), true);
    item->deleteByList = false;
    return item;
}

auto NotificationsListPresenter::create(const notifications::PhoneLockNotification *notification)
    -> NotificationListItem *
{
    auto item = new NotificationListItem(notifications::NotificationType::PhoneLock);

    item->setName(utils::translate("phone_lock_notification"), true, {{"$TIME", notification->getTime()}});
    item->deleteByList = false;
    return item;
}

void NotificationsListPresenter::updateData(app::manager::actions::NotificationsChangedParams *params,
                                            bool callAndSMSVisibility)
{
    if (list == nullptr) {
        LOG_ERROR("ListView object not provided");
    }

    list->prepareRebuildCallback = [this, toRemove = std::move(internalData)] {
        list->clear();
        for (auto item : toRemove) {
            delete item;
        }
    };

    for (const auto &notification : params->getNotifications()) {
        if (typeid(*notification) == typeid(notifications::NotSeenSMSNotification) && callAndSMSVisibility) {
            auto sms = static_cast<const notifications::NotSeenSMSNotification *>(notification.get());
            internalData.push_back(create(sms));
        }
        else if (typeid(*notification) == typeid(notifications::NotSeenCallNotification) && callAndSMSVisibility) {
            auto call = static_cast<const notifications::NotSeenCallNotification *>(notification.get());
            internalData.push_back(create(call));
        }
        else if (typeid(*notification) == typeid(notifications::TetheringNotification)) {
            auto tethering = static_cast<const notifications::TetheringNotification *>(notification.get());
            internalData.push_back(create(tethering));
        }
        else if (typeid(*notification) == typeid(notifications::PhoneLockNotification)) {
            auto phoneLock = static_cast<const notifications::PhoneLockNotification *>(notification.get());
            internalData.push_back(create(phoneLock));
        }
        else if (typeid(*notification) == typeid(notifications::AlarmSnoozeNotification)) {
            auto snooze = static_cast<const notifications::AlarmSnoozeNotification *>(notification.get());
            internalData.push_back(create(snooze));
        }
    }

    list->rebuildList(listview::RebuildType::InPlace);
    list->prepareRebuildCallback = nullptr;
}
