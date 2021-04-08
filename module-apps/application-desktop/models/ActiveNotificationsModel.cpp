// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ActiveNotificationsModel.hpp"
#include <module-db/queries/notifications/QueryNotificationsClear.hpp>
#include <service-appmgr/Controller.hpp>
#include "application-messages/ApplicationMessages.hpp"

using namespace gui;

ActiveNotificationsModel::ActiveNotificationsModel(AppWindow *parent) : parent{parent}
{}

void ActiveNotificationsModel::setParentBottomBar(const UTF8 &left, const UTF8 &center, const UTF8 &right)
{
    parent->setBottomBarText(left, BottomBar::Side::LEFT);
    parent->setBottomBarText(center, BottomBar::Side::CENTER);
    parent->setBottomBarText(right, BottomBar::Side::RIGHT);
}
auto ActiveNotificationsModel::create(const notifications::NotSeenSMSNotification *notification)
    -> NotificationListItem *
{
    auto item                  = NotificationsModel::create(notification);
    item->focusChangedCallback = [this](gui::Item &_item) {
        if (_item.focus) {
            setParentBottomBar({}, utils::localize.get("app_desktop_show"), utils::localize.get("app_desktop_clear"));
            return true;
        }
        return false;
    };
    item->activatedCallback = [this]([[maybe_unused]] gui::Item &_item) {
        return app::manager::Controller::sendAction(parent->getApplication(),
                                                    app::manager::actions::Launch,
                                                    std::make_unique<app::ApplicationLaunchData>(app::name_messages));
    };
    item->inputCallback = [this](Item &, const InputEvent &inputEvent) {
        if (inputEvent.isShortPress()) {
            if (inputEvent.is(KeyCode::KEY_RF)) {
                DBServiceAPI::GetQuery(
                    parent->getApplication(),
                    db::Interface::Name::Notifications,
                    std::make_unique<db::query::notifications::Clear>(NotificationsRecord::Key::Sms));
                return true;
            }
        }
        return false;
    };
    item->setDismissible(true);
    return item;
}
auto ActiveNotificationsModel::create(const notifications::NotSeenCallNotification *notification)
    -> NotificationListItem *
{
    auto item                  = NotificationsModel::create(notification);
    item->focusChangedCallback = [this](gui::Item &_item) {
        if (_item.focus) {
            setParentBottomBar({}, utils::localize.get("app_desktop_show"), utils::localize.get("app_desktop_clear"));
        }
        return true;
    };
    item->activatedCallback = [this]([[maybe_unused]] gui::Item &_item) {
        LOG_DEBUG("Clear Call notifications");
        return app::manager::Controller::sendAction(parent->getApplication(), app::manager::actions::ShowCallLog);
    };
    item->inputCallback = [this](Item &, const InputEvent &inputEvent) {
        if (inputEvent.isShortPress()) {
            if (inputEvent.is(KeyCode::KEY_RF)) {
                DBServiceAPI::GetQuery(
                    parent->getApplication(),
                    db::Interface::Name::Notifications,
                    std::make_unique<db::query::notifications::Clear>(NotificationsRecord::Key::Calls));
                return true;
            }
        }
        return false;
    };
    item->setDismissible(true);
    return item;
}
