// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "NotificationsModel.hpp"
#include <ListView.hpp>

using namespace gui;

namespace
{
    void setNotificationText(NotificationListItem *item,
                             const notifications::NotificationWithContact *notification,
                             const std::string &text)
    {
        if (notification->hasRecord()) {
            const auto &record = notification->getRecord();
            item->setName(record.getFormattedName());
        }
        else {
            item->setName(utils::translate(text), true);
        }
    }

    bool hasTetheringNotification(app::manager::actions::NotificationsChangedParams *params)
    {
        const auto &notifications = params->getNotifications();
        const auto it = std::find_if(std::begin(notifications), std::end(notifications), [](const auto &notification) {
            return notification->getType() == notifications::NotificationType::Tethering;
        });
        return it != std::end(notifications);
    }
} // namespace

NotificationsModel::NotificationsModel(NotificationsListPlacement listPlacement)
    : app::InternalModel<gui::NotificationListItem *>{}, gui::ListItemProvider{}, listPlacement{listPlacement}
{}

unsigned int NotificationsModel::requestRecordsCount()
{
    return internalData.size();
}

gui::ListItem *NotificationsModel::getItem(gui::Order order)
{
    return getRecord(order);
}

void NotificationsModel::requestRecords(uint32_t offset, uint32_t limit)
{
    setupModel(offset, limit);
    list->onProviderDataUpdate();
}

unsigned int NotificationsModel::getMinimalItemSpaceRequired() const
{
    return style::notifications::itemHeight;
}

bool NotificationsModel::hasDismissibleNotification() const noexcept
{
    for (const auto notification : internalData) {
        if (notification->isDismissible()) {
            return true;
        }
    }
    return false;
}

void NotificationsModel::dismissAll(const InputEvent &event)
{
    for (auto it = std::begin(internalData); it != std::end(internalData); it++) {
        if (auto item = *it; item->isDismissible()) {
            item->onInput(event);
        }
    }
}

bool NotificationsModel::isEmpty() const noexcept
{
    return internalData.empty();
}

auto NotificationsModel::create(const notifications::NotSeenSMSNotification *notification) -> NotificationListItem *
{
    auto item = new NotificationWithEventCounter(notifications::NotificationType::NotSeenSms,
                                                 utils::to_string(notification->getValue()));
    setNotificationText(item, notification, "app_desktop_unread_messages");
    item->deleteByList = false;
    return item;
}
auto NotificationsModel::create(const notifications::NotSeenCallNotification *notification) -> NotificationListItem *
{
    auto item = new NotificationWithEventCounter(notifications::NotificationType::NotSeenCall,
                                                 utils::to_string(notification->getValue()));
    setNotificationText(item, notification, "app_desktop_missed_calls");
    item->deleteByList = false;
    return item;
}

auto NotificationsModel::create(const notifications::TetheringNotification *notification) -> NotificationListItem *
{
    auto item = new NotificationWithOnOffButton(notifications::NotificationType::Tethering, gui::ButtonState::On);
    item->setName(utils::translate("Tethering"), false);
    item->deleteByList = false;
    return item;
}

void NotificationsModel::updateData(app::manager::actions::NotificationsChangedParams *params)
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

    const auto showOnLocked =
        (listPlacement == NotificationsListPlacement::LockedScreen) && params->showNotificationsWhenLocked();

    if ((listPlacement == NotificationsListPlacement::Desktop) || showOnLocked) {
        tetheringOn = hasTetheringNotification(params);
        for (const auto &notification : params->getNotifications()) {
            if (not tetheringOn && typeid(*notification) == typeid(notifications::NotSeenSMSNotification)) {
                auto sms = static_cast<const notifications::NotSeenSMSNotification *>(notification.get());
                internalData.push_back(create(sms));
            }
            else if (not tetheringOn && typeid(*notification) == typeid(notifications::NotSeenCallNotification)) {
                auto call = static_cast<const notifications::NotSeenCallNotification *>(notification.get());
                internalData.push_back(create(call));
            }
            else if (typeid(*notification) == typeid(notifications::TetheringNotification)) {
                auto tethering = static_cast<const notifications::TetheringNotification *>(notification.get());
                internalData.push_back(create(tethering));
            }
        }
    }

    list->rebuildList(listview::RebuildType::InPlace);
    list->prepareRebuildCallback = nullptr;
}

void NotificationsModel::clearAll()
{
    list->reset();
    eraseInternalData();
}

bool NotificationsModel::isTetheringOn() const noexcept
{
    return tetheringOn;
}
