// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "NotificationsModel.hpp"
#include <ListView.hpp>

using namespace gui;

namespace
{

    bool hasTetheringNotification(app::manager::actions::NotificationsChangedParams *params)
    {
        const auto &notifications = params->getNotifications();
        const auto it = std::find_if(std::begin(notifications), std::end(notifications), [](const auto &notification) {
            return notification->getType() == notifications::NotificationType::Tethering;
        });
        return it != std::end(notifications);
    }

    bool hasPhoneLockTime(app::manager::actions::NotificationsChangedParams *params)
    {
        const auto &notifications = params->getNotifications();
        const auto it = std::find_if(std::begin(notifications), std::end(notifications), [](const auto &notification) {
            return notification->getType() == notifications::NotificationType::PhoneLock;
        });
        return it != std::end(notifications);
    }
} // namespace

NotificationsModel::NotificationsModel(std::shared_ptr<NotificationsPresenter> notificationsPresenter,
                                       NotificationsListPlacement listPlacement)
    : notificationsPresenter{std::move(notificationsPresenter)}, listPlacement{listPlacement}
{}

NotificationsModel::NotificationsModel(NotificationsListPlacement listPlacement) : listPlacement{listPlacement}
{}

void NotificationsModel::attachPresenter(std::shared_ptr<NotificationsPresenter> notificationsPresenter)
{
    this->notificationsPresenter = notificationsPresenter;
}

bool NotificationsModel::hasDismissibleNotification() const noexcept
{
    if (notificationsPresenter == nullptr) {
        LOG_ERROR("Presenter not attached!");
        return false;
    }
    return notificationsPresenter->hasDismissibleNotification();
}

void NotificationsModel::dismissAll()
{
    if (notificationsPresenter == nullptr) {
        LOG_ERROR("Presenter not attached!");
        return;
    }
    notificationsPresenter->dismissAll();
}

bool NotificationsModel::isEmpty() const noexcept
{
    if (notificationsPresenter == nullptr) {
        LOG_ERROR("Presenter not attached!");
        return false;
    }
    return notificationsPresenter->isEmpty();
}

void NotificationsModel::updateData(app::manager::actions::NotificationsChangedParams *params)
{
    if (notificationsPresenter == nullptr) {
        LOG_ERROR("Presenter not attached!");
        return;
    }

    const auto showOnLocked =
        (listPlacement == NotificationsListPlacement::LockedScreen) && params->showNotificationsWhenLocked();
    phoneTimeLock = hasPhoneLockTime(params);
    tetheringOn   = hasTetheringNotification(params);
    const auto callAndSMSVisibility =
        ((listPlacement == NotificationsListPlacement::Desktop) || showOnLocked) && not tetheringOn;

    notificationsPresenter->updateData(params, callAndSMSVisibility);
}


bool NotificationsModel::isTetheringOn() const noexcept
{
    return tetheringOn;
}

bool NotificationsModel::isPhoneTimeLock() const noexcept
{
    return phoneTimeLock;
}
